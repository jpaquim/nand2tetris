#ifndef HACKASSEMBLER_H
#define HACKASSEMBLER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// forward declaration of SymbolTable
struct SymbolTable;
typedef struct SymbolTable SymbolTable;


// data types

// comp tokens for C instruction
typedef enum CompToken {
    COMP_0,         COMP_1,         COMP_NEG_1,
    COMP_D,         COMP_A,         COMP_M,
    COMP_NOT_D,     COMP_NOT_A,     COMP_NOT_M,
    COMP_NEG_D,     COMP_NEG_A,     COMP_NEG_M,
    COMP_D_PLUS_1,  COMP_A_PLUS_1,  COMP_M_PLUS_1,
    COMP_D_MINUS_1, COMP_A_MINUS_1, COMP_M_MINUS_1,
    COMP_D_PLUS_A,  COMP_D_PLUS_M,
    COMP_D_MINUS_A, COMP_D_MINUS_M, COMP_A_MINUS_D, COMP_M_MINUS_D,
    COMP_D_AND_A,   COMP_D_AND_M,   COMP_D_OR_A,    COMP_D_OR_M
} CompToken;

// dest tokens for C instruction
typedef enum DestToken {
    DEST_NULL, DEST_M, DEST_D, DEST_MD, DEST_A, DEST_AM, DEST_AD, DEST_AMD
} DestToken;

// jump tokens for C instruction
typedef enum JumpToken {
    JUMP_NULL,
    JUMP_JGT, JUMP_JEQ, JUMP_JGE, JUMP_JLT, JUMP_JNE, JUMP_JLE, JUMP_JMP
} JumpToken;


// functions

// translates the Hack assembly instructions in in_file, to binary ASCII
// instructions in out_file
void assemble_file(FILE *in_file, FILE *out_file);

// reads the whole file and adds (label) symbols to the symbol table
void read_label_symbols(FILE *in_file, SymbolTable *st);

// 
bool parse_comments_and_whitespace(char *asm_instr);

// parses an instruction of type A, reading and writing the symbol table if
// needed, places the result into value
void parse_A_instruction(const char *asm_instr, int line_num, uint16_t *value,
        SymbolTable *st);


// parses an instruction of type C, placing the tokens into comp, dest, and jump
void parse_C_instruction(const char *asm_instr, int line_num,
        CompToken *comp, DestToken *dest, JumpToken *jump);

// signals an error in parsing, aborts program execution
void parse_error(int line_num, const char *error_msg, const char *error_val);

// translates the A instruction value to its binary representation
void interpret_A_instruction(uint16_t value, char *bin_instr);

// interprets the C instruction tokens, and constructs the corresponding binary
// string instruction
void interpret_C_instruction(CompToken comp, DestToken dest, JumpToken jump,
        char *bin_instr);

#endif
