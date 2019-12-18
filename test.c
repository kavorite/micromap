#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include "micromap.h"

int main(int argc, char** argv) {
    ledger dict = { .cells = NULL, .len = 0, .cap = 0};
    tbGrow(&dict, 32);
    srand(time(0));
    char** keys;
    keys = malloc(sizeof(*keys)*22);
    for (int i = 0; i < 22; i++) {
        unsigned int rnd = rand();
        // 64 bits = 8 bytes + null terminator
        keys[i] = malloc(sizeof(char)*9);
        sprintf(keys[i], "%x", rnd);
        tbStatus stat;
        if((stat = tbSet(&dict, keys[i], keys[i])) != TB_STAT_OK) {
            fprintf(stderr, "fatal: failed ledger insertion: %s\n", tbStrError(stat));
            return -1;
        }
    }
    if (dict.len != 22) {
        fprintf(stderr, "fatal: length does not match\n");
        return -2;
    }
    for (int i = 0; i < dict.len; i++) {
        if (keys[i] != tbGet(&dict, keys[i])) {
            fprintf(stderr, "fatal: reference inequality on element %d\n", i+1);
            return -3;
        }
    }
}
