#include "game.h"
#include "mapFile.h"

int saveFlag = 0;
int loadFlag = 0;

int main(int argc, char *argv[]) {
    srand(time(NULL));
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (!init(&window, &renderer)) {
        return 1;
    }
    if (argc == 1) {
        printf("no args; running normally\n");
        generate_tile_map();
    } else if (argc >= 2) {
        if (strcmp(argv[1], "-s") == 0) {
            printf("Running in save mode\n");
            saveFlag = 1;
        } else {
            printf("Error: unrecognized arg\n");
            return 1;
        }
    } if (argc >= 4){
        if (strcmp(argv[2], "-l") == 0 || strcmp(argv[1], "-l") == 0){
            if (argc == 3) loadFlag = 2; //reusing loadFlag lol
            else loadFlag = 3;
            if (strstr(argv[loadFlag], ".map") != NULL){
                printf("Attempting to load map from %s\n", argv[loadFlag]);
                loadMap(argv[loadFlag], &tileMap[0][0]);
            } else {
                printf("Error: Please include a .map file to continue");
                return 1;
            }
        }
    }
    if (loadFlag == 0) generate_tile_map();
    game_loop(renderer, saveFlag);
    cleanup(window, renderer);
    return 0;
}

