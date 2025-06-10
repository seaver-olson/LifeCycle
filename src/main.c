#include "game.h"
#include "mapFile.h"

int saveFlag = 0;

int main(int argc, char *argv[]) {
    srand(time(NULL));
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (!init(&window, &renderer)) {
        return 1;
    }
    if (argc == 1) printf("no args; running normally\n");
    else if (argc == 2) {
        if (strcmp(argv[1], "0") == 0) {
            printf("Running without save mode\n");
            generate_tile_map();
        } else if (strcmp(argv[1], "1") == 0) {
            printf("Running in save mode\n");
            saveFlag = 1;
            generate_tile_map();
        } else if (strcmp(argv[1], "2") == 0) {
            printf("Error: to load map please follow first arg with map file\n");
            loadMap(argv[2], &tileMap[0][0]);
            return 1;
        } else {
            printf("Error: unrecognized arg\n");
            return 1;
        }
    } else if (argc == 3 && strcmp(argv[1], "2") == 0){
        printf("Attempting to load map from %s\n", argv[2]);
    } else {
        printf("invalid usage of arguments");
        return 1;        
    }
    game_loop(renderer, saveFlag);
    cleanup(window, renderer);
    return 0;
}

