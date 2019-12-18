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

#include "micromap.h"
#include <string.h>

#define FNV_OFFSET_BASIS (0xcbf29ce484222325)
#define FNV_PRIME (0x100000001b3)
// fnv1a
size_t ledgerHash(int n, const char* s) {
    size_t x = FNV_OFFSET_BASIS;
    for (int i = 0; i < n || n < 0; i++) {
        x ^= s[i];
        x *= FNV_PRIME;
        if (s[i] == '\0') {
            break;
        }
    }
    return x;
}

const char* tbStrError(tbStatus stat) {
    switch (stat) {
        case TB_STAT_OK:
            return "operation success";
        case TB_STAT_OMEM:
            return "out of memory";
        case TB_STAT_OVERLOAD:
            return "operation exceeds maximum load factor; please rehash";
        default:
            return NULL;
    }
            
}

char* strdup(const char* src) {
    int n = strlen(src);
    char* dst = malloc(n*sizeof(*dst));
    if (dst == NULL) {
        return NULL;
    }
    strcpy(dst, src);
    return dst;
}

tbStatus tbGrow(ledger* old, size_t ncap) {
    size_t pcap = old->cap;
    // size_t dcap = cap - old->cap;
    if ((float)(ncap) < 0.7*(float)(pcap)) {
        return TB_STAT_OVERLOAD;
    }
    ledger new = {.cells = NULL, .len = 0, .cap = ncap};
    new.cells = malloc(sizeof(tbcell)*ncap);
    if (new.cells == NULL) {
        return TB_STAT_OMEM;
    }
    memset(new.cells, 0, sizeof(tbcell)*ncap);
    for (int i = 0; i < pcap; i++) {
        if (old->cells[i].ptr != NULL) {
            tbSet(&new, old->cells[i].key, old->cells[i].ptr);
        }
    }
    tbFree(old);
    *old = new;
    return TB_STAT_OK;
}

size_t probe(const ledger* map, const char* key) {
    // hash for slot index
    size_t i = ledgerHash(-1, key) % map->cap;
    // linear probe; find first empty cell or key match
    while(map->cells[i].ptr != NULL) {
        if (strcmp(map->cells[i].key, key) == 0) {
            break;
        }
        i = (i + 1) % map->cap;
    }
    return i;
}

void tbFreeCell(ledger* map, size_t i) {
    if (map->cells[i].ptr != NULL) {
        map->len--;
    }
    if (map->cells[i].key != NULL) {
        free(map->cells[i].key);
    }
}

void tbDel(ledger* map, const char* key) {
    size_t i = probe(map, key);
    tbFreeCell(map, i);
    map->cells[i] = (tbcell){NULL, NULL};
}

void tbFree(ledger* map) {
    for (int i = 0; i < map->cap; i++) {
        tbFreeCell(map, i);
    }
    free(map->cells);
}

tbStatus tbSet(ledger* map, const char* key, const void* ptr) {
    if ((float)(map->len) > 0.7*(float)(map->cap)) {
        return TB_STAT_OVERLOAD;
    }
    size_t i = probe(map, key);
    if (map->cells[i].ptr == NULL) {
        map->len++;
    }
    tbFreeCell(map, i);
    map->cells[i].ptr = ptr;
    if ((map->cells[i].key = strdup(key)) == NULL) {
        return TB_STAT_OMEM;
    }
    return TB_STAT_OK;
}

const void* tbGet(const ledger* map, const char* key) {
    return map->cells[probe(map, key)].ptr;
}
