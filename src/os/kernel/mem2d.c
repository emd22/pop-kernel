#include <kernel/mem2d.h>
#include <stdlib.h>

char **alloc_2d(int x, int y) {
    char **arr;
    arr = malloc(y*sizeof(char *));

    int i;
    for (i = 0; i < y; i++) {
        arr[i] = malloc(x);
    }
    return arr;
}

void free_2d(int y, char **arr) {
    int i;
    for (i = 0; i < y; i++) {
        free(arr[i]);
    }
    free(arr);
}