#include "game.h"

#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"


float *createFastNoiseMap(int width, int height) {
    float *map = malloc(width * height * sizeof(float));
    if (!map) {
        fprintf(stderr, "Memory allocation failed!\n");//showing student how to push git file
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
            float noise = stb_perlin_noise3_seed(nx, ny, 0.0f, 0, 0, 0, rand() % (1001));

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
