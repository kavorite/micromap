#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include "micromap.h"

int main(void) {
    ledger dict;
    tbStatus stat = tbInit(&dict, 32);
    if (stat != TB_STAT_OK) {
        fprintf(stderr, "fatal: mkLedger: %s\n", tbStrError(stat));
        return -1;
    }
    srand(time(0));
    char keys[22][9];
    unsigned int vals[22];
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
            return -4;
        }
    }

    // range over the table, printing stored key/value pairs
    for (int i = 0; i < dict.cap; i++) {
        if (!tbCellEmpty(dict.cells+i)) {
            continue;
        }
        printf("%s = %d\n", dict.cells[i].key, *(int*)(dict.cells[i].ptr));
    }
    tbFree(&dict);
    printf("OK\n");
    return 0;
}
