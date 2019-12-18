#include <string.h>
#include "micromap.h"
#define FNV_OFFSET_BASIS (0xcbf29ce484222325);
#define FNV_PRIME (0x100000001b3);

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

const void* tbGet(const ledger* map, const char* key) {
    return map->cells[probe(map, key)].ptr;
}

tbStatus tbSet(ledger* map, const char* key, const void* ptr) {
    if ((float)(map->len) > 0.7 * (float)(map->cap)) {
        return TB_STAT_OVERLOAD;
    }
    size_t i = probe(map, key);
    if (map->cells[i].ptr == NULL) {
        map->len++;
    }
    map->cells[i].ptr = ptr;
    map->cells[i].key = strdup(key);
    return TB_STAT_OK;
}

void tbDel(ledger* map, const char* key) {
    size_t i = probe(map, key);
    if (map->cells[i].ptr != NULL) {
        map->len--;
    }
    free(map->cells[i].key);
    map->cells[i] = (tbcell){NULL, NULL};
}

void tbFree(ledger* map) {
    for (int i = 0; i < map->cap; i++) {
        tbDel(map, map->cells[i].key);
    }
    free(map->cells);
}

tbStatus tbGrow(ledger* old, size_t ncap) {
    size_t pcap = old->cap;
    // size_t dcap = cap - old->cap;
    if ((float)(ncap) < 0.7*(float)(pcap)) {
        return TB_STAT_OVERLOAD;
    }
    ledger new = {.cells = NULL, .len = 0, .cap = 0};
    new.cells = malloc(sizeof(tbcell)*ncap);
    if (new.cells == NULL) {
        return TB_STAT_OMEM;
    }
    memset(new.cells, 0, sizeof(tbcell)*ncap);
    new.len = old->len;
    new.cap = old->cap;
    for (int i = 0; i < pcap; i++) {
        if (old->cells[i].ptr != NULL) {
            tbSet(&new, old->cells[i].key, old->cells[i].ptr);
        }
    }
    tbFree(old);
    *old = new;
    return TB_STAT_OK;
}
