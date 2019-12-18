#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include "micromap.h"

int main(void) {
    ledger dict = { .cells = NULL, .len = 0, .cap = 0};
    tbGrow(&dict, 32);
    srand(time(0));
    char keys[22][9];
    unsigned int vals[22];
    tbStatus stat;
    for (int i = 0; i < 22; i++) {
        unsigned int x = rand();
        // 64 bits = 8 bytes + null terminator
        vals[i] = x;
        sprintf(keys[i], "%08x", x);
        if((stat = tbSet(&dict, keys[i], vals+i)) != TB_STAT_OK) {
            fprintf(stderr, "fatal: failed ledger insertion: %s\n", tbStrError(stat));
            return -1;
        }
    }
    printf("load factor pre-growth: %f\n", (float)(dict.len) / (float)(dict.cap));
    // try doing a brain transplant
    if ((stat = tbGrow(&dict, 64)) != TB_STAT_OK) {
        fprintf(stderr, "fatal: failed ledger rehash/reallocation: %s\n", tbStrError(stat));
        return -2;
    }
    if (dict.len != 22) {
        fprintf(stderr, "fatal: length does not match\n");
        return -3;
    }
    printf("load factor post-growth: %f\n", (float)(dict.len) / (float)(dict.cap));

    // check that cells are actually populated with our assigned values
    for (int i = 0; i < dict.len; i++) {
        if (vals+i != tbGet(&dict, keys[i])) {
            fprintf(stderr, "reference inequality on element %d (%s)\n", i+1, keys[i]);
            return -3;
        }
    }

    // range over the table, printing stored key/value pairs
    for (int i = 0; i < dict.cap; i++) {
        tbcell cell = dict.cells[i];
        if (cell.key == NULL) {
            continue;
        }
        printf("%s = %d\n", cell.key, *(int*)(cell.ptr));
    }
    tbFree(&dict);
    printf("OK\n");
    return 0;
}
