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
    char* key;
    const void* ptr;
} tbcell;

typedef struct {
    tbcell* cells;
    size_t len, cap;
} ledger;

void tbFree(ledger* map);
tbStatus tbGrow(ledger* map, size_t cap);
tbStatus tbSet(ledger* map, const char* key, const void* ptr);
void tbDel(ledger* map, const char* key);
const void* tbGet(const ledger* map, const char* key);
#endif
