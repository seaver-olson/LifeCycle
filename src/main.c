#include <SDL2/SDL.h>
#include <stdbool.h>
#define STB_PERLIN_IMPLEMENTATION
#include "../include/stb_perlin.h"
#include "../include/game.h"

Camera camera = {0.0f, 0.0f, 1.0f};
float *noiseMap = NULL;
int tile_size = BASE_TILE_SIZE;
Tile tileMap[GRID_ROWS][GRID_COLS];

//noiseMap generator
float *createFastNoiseMap(int width, int height) {
    float *map = malloc(width * height * sizeof(float));
    if (!map) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    float scale = 0.05f;  // controls frequency of terrain
    float center_x = width / 2.0f;
    float center_y = height / 2.0f;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            // 1. Generate Perlin noise
            float nx = x * scale;
            float ny = y * scale;
            float noise = stb_perlin_noise3(nx, ny, 0.0f, 0, 0, 0);

            // 2. Apply radial falloff
            float dx = (x - center_x) / center_x;
            float dy = (y - center_y) / center_y;
            float dist = sqrtf(dx * dx + dy * dy);
            float falloff = 1.0f - dist;
            falloff = fmaxf(0.0f, falloff); // clamp to [0, 1]

            // 3. Combine and bias the final value
            float final = noise * falloff;

            map[y * width + x] = final;
        }
    }
    return map;
}

void freeFastNoiseMap(float *map) {
    if (map) free(map);
}

bool init(SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    *window = SDL_CreateWindow("LifeCycle Simulator",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!*window) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

void render_grid(SDL_Renderer *renderer){
    int tile_size = BASE_TILE_SIZE * camera.zoom;

    int start_x = (int)(camera.x / BASE_TILE_SIZE);
    int start_y = (int)(camera.y / BASE_TILE_SIZE);
    int end_x = (int)((camera.x + WINDOW_WIDTH / camera.zoom) / BASE_TILE_SIZE) + 1;
    int end_y = (int)((camera.y + WINDOW_HEIGHT / camera.zoom) / BASE_TILE_SIZE) + 1;

    for (int y = start_y; y < end_y; y++){
        for (int x = start_x; x < end_x; x++){
            Tile tile = tileMap[y][x];  
            SDL_Rect rect;
            rect.x = (x * BASE_TILE_SIZE - camera.x) * camera.zoom;
            rect.y = (y * BASE_TILE_SIZE - camera.y) * camera.zoom;
            rect.w = tile_size;
            rect.h = tile_size;
            switch (tile.type){
                case TILE_EMPTY:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
                    break;
                case TILE_GRASS:
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); 
                    break;
                case TILE_WATER:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); 
                    break;
                case TILE_FOREST:
                    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); 
                    break;
                case TILE_MOUNTAIN:
                    SDL_SetRenderDrawColor(renderer, 139, 137, 137, 255); 
                    break;
                case TILE_ROAD:
                    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); 
                    break;
                case TILE_BUILDING:
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
                    break;
                default:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
                    break;
                }
            SDL_RenderDrawRect(renderer, &rect);
            SDL_RenderFillRect(renderer, &rect);
            }
        }

}

void place_tile(int x, int y, TileType type) {
    if (x < 0 || x >= GRID_COLS || y < 0 || y >= GRID_ROWS) return;
    Tile tile = tileMap[y][x];
    tile.type = type;
    tileMap[y][x] = tile;
}

void generate_tile_map() {
    noiseMap = createFastNoiseMap(WINDOW_WIDTH / tile_size, WINDOW_HEIGHT / tile_size);
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            float noise_value = noiseMap[y * (WINDOW_WIDTH / tile_size) + x];
            if (noise_value < -0.5f) {
                place_tile(x, y, TILE_FOREST);
            } else if (noise_value < 0.0f) {
                place_tile(x, y, TILE_GRASS);
            } else if (noise_value < 0.5f) {
                place_tile(x, y, TILE_WATER);
            } else {
                place_tile(x, y, TILE_MOUNTAIN);
            }
        }
    }
}

void game_loop(SDL_Renderer *renderer) {
    bool running = true;
    SDL_Event event;
    generate_tile_map();
    while (running) {
        tile_size = BASE_TILE_SIZE * camera.zoom;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        
            if (event.type == SDL_MOUSEWHEEL){
                int mx, my;
                SDL_GetMouseState(&mx, &my);

                float beforeZoomX = camera.x + mx / camera.zoom;
                float beforeZoomY = camera.y + my / camera.zoom;

                camera.zoom += 0.1f * event.wheel.y;
                camera.zoom = fmaxf(ZOOM_MIN, fminf(camera.zoom, ZOOM_MAX));

                float afterZoomX = camera.x + mx / camera.zoom;
                float afterZoomY = camera.y + my / camera.zoom;

                camera.x += (beforeZoomX - afterZoomX);
                camera.y += (beforeZoomY - afterZoomY);

            }
        }
        printf("Zoom: %.2f, Tile Size: %d\n", camera.zoom, tile_size);
        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        render_grid(renderer);


        SDL_RenderPresent(renderer);

        
        SDL_Delay(16);//60 FPS
    }
}

void cleanup(SDL_Window *window, SDL_Renderer *renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(void) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    tileMap[10][10].type = TILE_GRASS;
    tileMap[11][11].type = TILE_WATER;
    tileMap[12][12].type = TILE_FOREST;
    if (!init(&window, &renderer)) {
        return 1;
    }
    game_loop(renderer);
    cleanup(window, renderer);

    return 0;
}

