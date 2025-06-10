#include "mapFile.h"

//for now, will write to a default file I will add the ability to write to a specific file soon
void saveMap(Tile *tileMap){
    FILE *file = fopen("out.map", "w");
    if (!file){
        printf("Error: Failed top open map file\n");
        return;
    }
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            Tile tile = tileMap[y * GRID_COLS + x];
            fprintf(file, "%d", tile.type);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int loadMap(char *mapFile, Tile *tileMap){
    FILE *file = fopen(mapFile, "r");
    if (!file){
        printf("Error: Failed to open map file\n");
        return 1;
    }
    
    for (int y = 0; y < GRID_ROWS; y++) {
        for (int x = 0; x < GRID_COLS; x++) {
            int ch = fgetc(file);
            if (ch == EOF) break;

            if (ch == '\n') x--;

            int tileVal = ch - '0'; 
            tileMap[y * GRID_COLS + x].type = (TileType)tileVal;
        }
    }

    fclose(file);
    return 0;
}