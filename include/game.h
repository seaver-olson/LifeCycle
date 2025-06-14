#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef enum{
    TILE_EMPTY,
    TILE_GRASS,
    TILE_WATER,
    TILE_FOREST,
    TILE_MOUNTAIN,
    TILE_ROAD,
    TILE_BUILDING,
} TileType;

typedef struct {
    TileType type;
} Tile;

typedef struct {
    float x;         
    float y;
    float zoom;      // e.g., 1.0 = 100%, 2.0 = zoomed in
} Camera;

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 1000

#define GAME_HEIGHT 800

#define ZOOM_MIN 1.0f
#define ZOOM_MAX 3.5f

#define WINDOW_TITLE "LifeCycle Simulator"
#define BASE_TILE_SIZE 20

#define GRID_COLS 200
#define GRID_ROWS 200

extern Tile tileMap[GRID_ROWS][GRID_COLS];
extern Camera camera;
extern int tile_size;
extern float *noiseMap;

bool init(SDL_Window **window, SDL_Renderer **renderer);
void game_loop(SDL_Renderer *renderer, int saveFlag);
void cleanup(SDL_Window *window, SDL_Renderer *renderer);
void render_grid(SDL_Renderer *renderer);
void place_tile(int x, int y, TileType type);
void generate_tile_map();

#endif