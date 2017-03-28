#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#define KEY_SIZE 64

// data types

// entry in symbol table, key: value pair (char[]: uint16_t)
typedef struct TableEntry {
    uint16_t value;
    char key[KEY_SIZE];
} TableEntry;

// symbol table, holds array of entries and its size
typedef struct SymbolTable {
    TableEntry *table;
    size_t size;
    size_t num_entries;
} SymbolTable;


// functions

// creates a new empty symbol table
SymbolTable *new_symbol_table(size_t size);

// deletes symbol table from memory
void delete_symbol_table(SymbolTable *st);

// computes the djb2 hash of a string
size_t compute_hash(const char *key);

// inserts a symbol key: value pair into the symbol table st, grows if necessary
void insert_symbol(SymbolTable *st, const char *key, uint16_t value);

// grows table to acommodate new entries
void grow_table(SymbolTable *st, double factor);

// looks up a key in the table, if found, places value in *value, returns true
// if not found, returns false
bool lookup_key(SymbolTable *st, const char *key, uint16_t *value);

// prints an error message and exits
void symbol_table_error(const char *error_msg, const char *error_val);

// prints the symbol table to stdout
void print_table(SymbolTable *st);

// creates the initial symbol table for the Hack assembler
SymbolTable *initial_symbol_table(size_t size);

#endif
