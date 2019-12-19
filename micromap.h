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

#ifndef H_MICROMAP_LEDGER
#define H_MICROMAP_LEDGER
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

size_t ledgerHash(int n, const char* s);

typedef enum {
    TB_STAT_OK,
    TB_STAT_OMEM,
    TB_STAT_OVERLOAD,
} tbStatus;

const char* tbStrError(tbStatus);

typedef struct {
    char*       key;
    const void* ptr;
} tbcell;

typedef struct {
    tbcell* cells;
    size_t  len, cap;
} ledger;

tbStatus tbInit(ledger* map, size_t cap);
void     tbFree(ledger* map);
tbStatus tbGrow(ledger* map, size_t ncap);
tbStatus tbSet(ledger* map, const char* key, const void* ptr);
void*    tbGet(const ledger* map, const char* key);
void     tbDel(ledger* map, const char* key);
size_t   tbProbe(const ledger* map, const char* key);

#endif
