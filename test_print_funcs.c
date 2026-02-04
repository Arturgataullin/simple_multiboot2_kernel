#include <stdio.h>
#include <stdlib.h>

// заполенние буфера
void memset32(int* fdest, int color, size_t count) {
    for (size_t i = 0; i < count; i++) {
        fdest[i] = color;
    }
}

void draw_rect(int* fdest, int fwidth, int fheight, int x, int y, int width, int height, size_t color) {
    if ((x + width) > fwidth || (y + height) > fheight) return;
    for (int cy = y; cy < y + height; cy++) {
        memset32(fdest + fwidth * cy + x, color, width);
    }
}

void print_map(int* fdest, int w, int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            printf("%d ", fdest[i * h + j]);
        }
        printf("\n");
    } 
}

int main() {
    int w = 10, h = 10;
    int* image = malloc(sizeof(int) * 100);
    draw_rect(image, w, h, 3, 3, 2, 2, 3);
    print_map(image, w, h);
    free(image);
    return 0;
}