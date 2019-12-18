#ifndef H_LEDGER
#define H_LEDGER
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

unsigned long long ledgerHash(int n, const char* s);

typedef enum {
    TB_STAT_OK,
    TB_STAT_OMEM,
    TB_STAT_OVERLOAD,
} tbStatus;

typedef struct {
    char* key;
    void* ptr;
} tbcell;

typedef struct {
    tbcell* cells;
    size_t len, cap;
} ledger;

void tbFree(ledger* map);
tbStatus tbGrow(ledger* map, size_t cap);
tbStatus tbSet(ledger* map, char* key, void* ptr);
void tbDel(ledger* map, char* key);
void* tbGet(ledger* map, char* key);

const char* tbStrError(tbStatus);

#endif
