#include "mapGenerator.h"
#include "game.h"
#include "mapFile.h"

Camera camera = {0.0f, 0.0f, 1.0f};
float *noiseMap = NULL;
int tile_size = BASE_TILE_SIZE;
TileType selectedType = TILE_EMPTY;
Tile tileMap[GRID_ROWS][GRID_COLS];
bool mouseDown = false;
char* targetOut = "out.map";//for later saving of maps

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
    int end_y = (int)((camera.y + GAME_HEIGHT / camera.zoom) / BASE_TILE_SIZE) + 1;

    for (int y = start_y; y < end_y; y++){
        for (int x = start_x; x < end_x; x++){
            Tile tile = tileMap[y][x];  
            SDL_Rect rect;
            rect.x = (x * BASE_TILE_SIZE - camera.x) * camera.zoom;
            rect.y = (y * BASE_TILE_SIZE - camera.y) * camera.zoom;
            rect.w = tile_size;
            rect.h = tile_size;
            //holy air ball - fix when not sleep deprived
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
    tileMap[y][x].type = type;
}

void generate_tile_map() {
    noiseMap = createFastNoiseMap(GRID_COLS, GRID_ROWS);
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            float noise_value = noiseMap[y * GRID_COLS + x];
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

void paintTile(){
    static int last_x = -1, last_y = -1;

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    if (my > GAME_HEIGHT) return; //don't paint on UI
    int grid_x = (camera.x + mx / camera.zoom) / BASE_TILE_SIZE;
    int grid_y = (camera.y + my / camera.zoom) / BASE_TILE_SIZE;

    if (grid_x != last_x || grid_y != last_y ) {
        place_tile(grid_x, grid_y, selectedType);
        last_x = grid_x;
        last_y = grid_y;
    }
}

void render_UI(SDL_Renderer *renderer) {
    SDL_Rect panel = {0, GAME_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 136); // dark semi-transparent
    SDL_RenderFillRect(renderer, &panel);
}


void game_loop(SDL_Renderer *renderer, int saveFlag) {
    bool running = true;
    SDL_Event event;
    //center the screen before gameloop
    camera.x = GRID_COLS * BASE_TILE_SIZE / 2 - WINDOW_WIDTH / 2;
    camera.y = GRID_ROWS * BASE_TILE_SIZE / 2 - GAME_HEIGHT / 2;
    while (running) {
        tile_size = BASE_TILE_SIZE * camera.zoom;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_MOUSEWHEEL){
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
                printf("Zoom: %.2f, Tile Size: %d\n", camera.zoom, tile_size);
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                mouseDown = true;
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                mouseDown = false;
            } else if (event.type == SDL_KEYDOWN){
                switch (event.key.keysym.sym) {
                    case SDLK_1: selectedType = TILE_GRASS; break;
                    case SDLK_2: selectedType = TILE_WATER; break;
                    case SDLK_3: selectedType = TILE_FOREST; break;
                    case SDLK_4: selectedType = TILE_MOUNTAIN; break;
                    case SDLK_5: selectedType = TILE_ROAD; break;
                    case SDLK_6: selectedType = TILE_BUILDING; break;
                    case SDLK_0: selectedType = TILE_EMPTY; break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        if (mouseDown){
            paintTile();
        }

        render_grid(renderer);
        render_UI(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);//approx 62 FPS
    }
    if (saveFlag == 1) saveMap(&tileMap[0][0]);
}

void cleanup(SDL_Window *window, SDL_Renderer *renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}