/* 
 * https://github.com/kavorite/micromap
 *
 * Sometimes, you just want a hashmap. This is the simplest dictionary
 * implementation that I'm aware could have been conceived. It uses an
 * open-addressing scheme with linear probing, and returns an error on
 * insertions or reallocations when the load factor exceeds 70%. It's designed
 * to be easily audited, easily reused, and easily read. Use as you wish. Or
 * write a better one. No skin off my back.
 *
 */

#include <string.h>
#include <stdbool.h>
#include "micromap.h"

#define FNV_OFFSET_BASIS (0xcbf29ce484222325)
#define FNV_PRIME (0x100000001b3)

// fnv1a
size_t ledgerHash(int n, const char* s) {
    size_t x = FNV_OFFSET_BASIS;
    for (int i = 0; (s[i] != '\0' &&
                     (i < n || n < 0)); i++) {
        x ^= s[i];
        x *= FNV_PRIME;
    }
    return x;
}

const char* tbStrError(tbStatus stat) {
    switch (stat) {
        case TB_STAT_OK:
            return "operation success";
        case TB_STAT_OMEM:
            return "out of memory";
        case TB_STAT_KEY_TOO_LONG:
            return "key exceeds maximum length";
        case TB_STAT_OVERLOAD:
            return "operation exceeds maximum load factor; please resize ledger";
        default:
            return NULL;
    }
            
}

bool tbKeyEmpty(const tbkey key) {
    return key[0] == '\0';
}

bool tbCellEmpty(const tbcell* cell) {
    return tbKeyEmpty(cell->key) && cell->ptr != NULL;
}

tbStatus tbKeySet(tbkey key, const char* src, int srcLen) {
    if (src != NULL) {
        if (srcLen < 0) {
            srcLen = strlen(src);
        }
        if (srcLen > LEDGER_MAX_KEYLEN-1) {
            return TB_STAT_KEY_TOO_LONG;
        }
    }
    memset(key, 0, LEDGER_MAX_KEYLEN*sizeof(*key));
    if (src != NULL) {
        strncpy(key, src, srcLen);
    }
    return TB_STAT_OK;
}

tbStatus tbGrow(ledger* old, size_t ncap) {
    size_t pcap = old->cap;
    // size_t dcap = cap - old->cap;
    if (0.7*(double)(old->len+1) > (double)(ncap)) {
        return TB_STAT_OVERLOAD;
    }
    ledger new = {.cells = NULL, .len = 0, .cap = ncap};
    new.cells = calloc(ncap, sizeof(tbcell));
    if (new.cells == NULL) {
        return TB_STAT_OMEM;
    }
    tbStatus stat;
    for (int i = 0; i < pcap; i++) {
        if (!tbCellEmpty(old->cells+i)) {
            stat = tbSet(&new, old->cells[i].key, old->cells[i].ptr);
            if (stat != TB_STAT_OK) {
                return stat;
            }
        }
    }
    tbFree(old);
    *old = new;
    return TB_STAT_OK;
}

tbStatus tbInit(ledger* map, size_t cap) {
    *map = (ledger){.cells = NULL, .cap = 0, .len = 0};
    tbStatus stat = tbGrow(map, cap);
    return stat;
}

size_t tbProbe(const ledger* map, const char* key) {
    // hash for slot index
    size_t i = ledgerHash(-1, key) % map->cap;
    // linear probe; find first empty cell or key match
    while(!tbCellEmpty(map->cells+i) &&
          strcmp(key, map->cells[i].key) != 0) {
        i = (i + 1) % map->cap;
    }
    return i;
}

void tbFreeCell(ledger* map, size_t i) {
    if (!tbCellEmpty(map->cells+i)) {
        map->len--;
    }
    map->cells[i].ptr = NULL;
    map->cells[i].key[0] = '\0';
}

void tbDel(ledger* map, const char* key) {
    size_t i = tbProbe(map, key);
    tbFreeCell(map, i);
}

void tbFree(ledger* map) {
    for (int i = 0; i < map->cap; i++) {
        tbFreeCell(map, i);
    }
    free(map->cells);
    *map = (ledger){.cells = NULL, .cap = 0, .len = 0};
}

tbStatus tbSet(ledger* map, const char* key, const void* ptr) {
    if ((double)(map->len+1) > 0.7*(double)(map->cap)) {
        return TB_STAT_OVERLOAD;
    }
    size_t i = tbProbe(map, key);
    if (tbCellEmpty(map->cells+i)) {
        map->len++;
    }
    tbFreeCell(map, i);
    map->cells[i].ptr = ptr;
    tbStatus stat = tbKeySet(map->cells[i].key, key, -1);
    return stat;
}

void* tbGet(const ledger* map, const char* key) {
    return (void*)(map->cells[tbProbe(map, key)].ptr);
}
