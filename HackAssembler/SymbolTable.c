#include "SymbolTable.h"

#include <stdio.h>
#include <stdlib.h>

SymbolTable *new_symbol_table(size_t size) {
    SymbolTable *st = malloc(sizeof(*st));
    st->table = malloc(size*sizeof(*st->table));
    st->size = size;
    st->num_entries = 0;
    return st;
}

void delete_symbol_table(SymbolTable *st) {
    free(st->table);
    st->size = 0;
    st->num_entries = 0;
    // free(st);
}

size_t compute_hash(const char *key) {
    size_t hash = 5381;
    char c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void insert_symbol(SymbolTable *st, const char *key, uint16_t value) {
    size_t hash = compute_hash(key) % st->size;
    size_t i = hash;
    if (st->num_entries > 0.7*st->size) {
        grow_table(st, 2);
    }
    while (st->table[i].key[0] != '\0') { // traverse until no collision
        if (i == hash - 1) { // wrapped around to starting index
            symbol_table_error("no space left", key);
        }
        if (!strcmp(key, st->table[i].key)) { // name collision
            symbol_table_error("key already present", key);
        }
        i = (i + 1) % st->size; // increment, wrapping around
    }
    // copy key, value pair to the empty table entry
    strcpy(st->table[i].key, key);
    st->table[i].value = value;
    ++st->num_entries;
}

void grow_table(SymbolTable *st, double factor) {
    size_t new_st_size = factor*st->size;
    SymbolTable *new_st = new_symbol_table(new_st_size);
    // printf("growing from %zd to %zd\n", st->size, new_st_size);
    for (size_t i = 0; i < st->size; ++i) {
        if (st->table[i].key[0] == '\0') { // skip empty entries
            continue;
        }
        insert_symbol(new_st, st->table[i].key, st->table[i].value);
    }
    delete_symbol_table(st);
    *st = *new_st;
}

bool lookup_key(SymbolTable *st, const char *key, uint16_t *value) {
    size_t hash = compute_hash(key) % st->size;
    size_t i = hash;
    while (strcmp(key, st->table[i].key)) {
        if (i == hash - 1) { // wrapped around to starting index, not found
            return false;
        }
        i = (i + 1) % st->size; // increment, wrapping around
    }
    *value = st->table[i].value;
    return true;
}

void symbol_table_error(const char *error_msg, const char *error_val) {
    fprintf(stderr, "Error in symbol table: %s: %s\n", error_msg, error_val);
    exit(EXIT_FAILURE);
}

void print_table(SymbolTable *st) {
    for (size_t i = 0; i < st->size; ++i) {
        if (st->table[i].key[0] == '\0') { // skip empty entries
            continue;
        }
        printf("%zd %s: %" PRIu16 "\n",
                i, st->table[i].key, st->table[i].value);
    }
    putchar('\n');
}

SymbolTable *initial_symbol_table(size_t size) {
    const int num_symbols = 23;
    char keys[num_symbols][7] = {
        "SP", "LCL", "ARG", "THIS", "THAT", "SCREEN", "KBD",
        "R0", "R1", "R2",  "R3",  "R4",  "R5",  "R6",  "R7",
        "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15"};
    uint16_t values[num_symbols] = {
        0, 1, 2, 3, 4, 16384, 24576,
        0, 1, 2,  3,  4,  5,  6,  7,
        8, 9, 10, 11, 12, 13, 14, 15};
    SymbolTable *st = new_symbol_table(size);
    for (size_t i = 0; i < num_symbols; ++i) {
        insert_symbol(st, keys[i], values[i]);
    }
    return st;
}
