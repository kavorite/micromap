#include <strings.h>
#include "micromap.h"
#define FNV_OFFSET_BASIS (0xcbf29ce484222325)
#define FNV_PRIME (0x100000001b3)

// fnv1a
unsigned long long ledgerHash(int n, const char* s) {
    unsigned long long x = FNV_OFFSET_BASIS;
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

int keycmp(int n, const char* a, const char* b) {
    int c = 0;
    for (int i = 0; a[i] != '\0' && (i < n || n < 0); i++) {
        if (a[i] == b[i]) {
            c += a[i] == b[i] ? 0 ? a[i] > b[i] : 1 : -1;
        }
    }
    return c == 0 ? 0 ? c > 0 : 1 : -1;
}

void* tbGet(ledger* map, char* key) {
    size_t i = ledgerHash(-1, key);
    while(map->cells[i].ptr != NULL) {
        if (keycmp(-1, map->cells[i].key, key) == 0) {
            break;
        }
        i = (i+1)%map->cap;
    }
    return i != map->cap ? map->cells[i].ptr : NULL;
}

tbStatus tbSet(ledger* map, char* key, void* ptr) {
    if ((float)(map->len) > 0.7 * (float)(map->cap)) {
        return TB_STAT_OVERLOAD;
    }
    size_t i = ledgerHash(-1, key) % map->cap;
    while (map->cells[i].ptr != NULL) {
        if (keycmp(-1, map->cells[i].key, key) == 0) {
            break;
        }
        i = (i+1)%map->cap;
    }
    map->cells[i].ptr = ptr;
    map->len++;
    return TB_STAT_OK;
}

void tbFree(ledger* map) {
    free(map->cells);
    free(map);
}

tbStatus tbGrow(ledger* old, size_t ncap) {
    size_t pcap = old->cap;
    // size_t dcap = cap - old->cap;
    if ((float)(ncap) < 0.7*(float)(pcap)) {
        return TB_STAT_OVERLOAD;
    }
    ledger* new = malloc(sizeof(ledger));
    if (new == NULL) {
        return TB_STAT_OMEM;
    }
    new->cells = malloc(sizeof(tbcell)*ncap);
    if (new->cells == NULL) {
        return TB_STAT_OMEM;
    }
    new->len = old->len;
    new->cap = old->cap;
    for (int i = 0; i < pcap; i++) {
        if (old->cells[i].ptr != NULL) {
            tbSet(new, old->cells[i].key, old->cells[i].ptr);
        }
    }
    tbFree(old);
    *old = *new;
    return TB_STAT_OK;
}
