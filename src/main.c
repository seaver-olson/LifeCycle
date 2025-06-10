#include "game.h"

int main(void) {
    srand(time(NULL));
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (!init(&window, &renderer)) {
        return 1;
    }

    game_loop(renderer);

    cleanup(window, renderer);
    return 0;
}

