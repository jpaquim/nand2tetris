#include "HackAssembler.h"
#include "SymbolTable.h"

#include <stdlib.h>
#include <ctype.h>


// constants
const size_t buf_size = 128; // maximum line length = 126 + '\n' + '\0'
const size_t instr_size = 18; // 16 bits + '\n' + '\0'
const size_t table_size = 128; // initial symbol table size

void assemble_file(FILE *in_file, FILE *out_file) {
    SymbolTable *st = initial_symbol_table(table_size);
    read_label_symbols(in_file, st);
    rewind(in_file);

    char asm_instr[buf_size];
    char bin_instr[instr_size];
    bin_instr[instr_size-2] = '\n';
    bin_instr[instr_size-1] = '\0';
    int line_num = 0;
    while (fgets(asm_instr, buf_size, in_file)) {
        ++line_num;
        bool skip_line = parse_comments_and_whitespace(asm_instr);
        if (skip_line || asm_instr[0] == '(') {
            continue;
        }
        if (asm_instr[0] == '@') { // A instruction
            uint16_t value;
            parse_A_instruction(asm_instr, line_num, &value, st);
            interpret_A_instruction(value, bin_instr);
        } else { // C instruction
            DestToken dest;
            CompToken comp;
            JumpToken jump;
            parse_C_instruction(asm_instr, line_num, &comp, &dest, &jump);
            interpret_C_instruction(comp, dest, jump, bin_instr);
        }
        fputs(bin_instr, out_file); // write instruction to file
    }
}

void read_label_symbols(FILE *in_file, SymbolTable *st) {
    char asm_instr[buf_size];
    char key[buf_size];
    uint16_t instr_addr = 0;
    while (fgets(asm_instr, buf_size, in_file)) {
        bool skip_line = parse_comments_and_whitespace(asm_instr);
        if (skip_line) {
            continue;
        }
        if (1 > sscanf(asm_instr, "(%[^)])", key)) {
            ++instr_addr; // regular instruction, increment current instruction
        } else { // found (label)
            insert_symbol(st, key, instr_addr); 
        }
    }
}

bool parse_comments_and_whitespace(char *asm_instr) {
    char *comment_beginning = strstr(asm_instr, "//");
    if (comment_beginning) {
        *comment_beginning = '\0'; // terminate line at comment beginning
    }
    // strip spaces from instruction
    for (size_t i = 0, j = 0; asm_instr[j] != '\0'; ++i) {
        if (!isspace(asm_instr[i])) {
            asm_instr[j++] = asm_instr[i];
        }
    }
    if (*asm_instr)
        return false;
    else
        return true; // there's nothing left, skip line
}

void parse_A_instruction(const char *asm_instr, int line_num, uint16_t *value,
        SymbolTable *st) {
    static uint16_t highest_var_addr = 16;
    uint16_t value_tmp;
    char key[buf_size];
    if (1 == sscanf(asm_instr, "@%" SCNu16, &value_tmp)) {
        if (value_tmp >> 15) {
            parse_error(line_num, "value should be 15-bits", asm_instr);
        }
        ;
    } else if (1 == sscanf(asm_instr, "@%s", key)) {
        if (lookup_key(st, key, &value_tmp)) {
            ;
        } else {
            value_tmp = highest_var_addr++;
            insert_symbol(st, key, value_tmp);
        }
    }
    *value = value_tmp;
}

void parse_C_instruction(const char *asm_instr, int line_num,
        CompToken *comp, DestToken *dest, JumpToken *jump) {
    char dest_str[buf_size] = "";
    char comp_str[buf_size] = "";
    char jump_str[buf_size] = "";
    if (3 == sscanf(asm_instr, "%[^=]=%[^;];%s", dest_str, comp_str, jump_str)) {
    } else if (2 == sscanf(asm_instr, "%[^;];%s", comp_str, jump_str)) {
        *dest_str = '\0';
    } else if (2 == sscanf(asm_instr, "%[^=]=%s", dest_str, comp_str)) {
        *jump_str = '\0';
    } else {
        strcpy(comp_str, asm_instr);
        *dest_str = '\0';
        *jump_str = '\0';
    }
    // printf("dest: %s, comp: %s, jump: %s\n", dest_str, comp_str, jump_str);
    if      (!strcmp(dest_str, ""))    *dest = DEST_NULL;
    else if (!strcmp(dest_str, "M"))   *dest = DEST_M;
    else if (!strcmp(dest_str, "D"))   *dest = DEST_D;
    else if (!strcmp(dest_str, "MD"))  *dest = DEST_MD;
    else if (!strcmp(dest_str, "A"))   *dest = DEST_A;
    else if (!strcmp(dest_str, "AM"))  *dest = DEST_AM;
    else if (!strcmp(dest_str, "AD"))  *dest = DEST_AD;
    else if (!strcmp(dest_str, "AMD")) *dest = DEST_AMD;
    else parse_error(line_num, "unknown dest token", dest_str);

    if      (!strcmp(comp_str, "0"))   *comp = COMP_0;
    else if (!strcmp(comp_str, "1"))   *comp = COMP_1;
    else if (!strcmp(comp_str, "-1"))  *comp = COMP_NEG_1;
    else if (!strcmp(comp_str, "D"))   *comp = COMP_D;
    else if (!strcmp(comp_str, "A"))   *comp = COMP_A;
    else if (!strcmp(comp_str, "M"))   *comp = COMP_M;
    else if (!strcmp(comp_str, "!D"))  *comp = COMP_NOT_D;
    else if (!strcmp(comp_str, "!A"))  *comp = COMP_NOT_A;
    else if (!strcmp(comp_str, "!M"))  *comp = COMP_NOT_M;
    else if (!strcmp(comp_str, "-D"))  *comp = COMP_NEG_D;
    else if (!strcmp(comp_str, "-A"))  *comp = COMP_NEG_A;
    else if (!strcmp(comp_str, "-M"))  *comp = COMP_NEG_M;
    else if (!strcmp(comp_str, "D+1")) *comp = COMP_D_PLUS_1;
    else if (!strcmp(comp_str, "A+1")) *comp = COMP_A_PLUS_1;
    else if (!strcmp(comp_str, "M+1")) *comp = COMP_M_PLUS_1;
    else if (!strcmp(comp_str, "D-1")) *comp = COMP_D_MINUS_1;
    else if (!strcmp(comp_str, "A-1")) *comp = COMP_A_MINUS_1;
    else if (!strcmp(comp_str, "M-1")) *comp = COMP_M_MINUS_1;
    else if (!strcmp(comp_str, "D+A")) *comp = COMP_D_PLUS_A;
    else if (!strcmp(comp_str, "D+M")) *comp = COMP_D_PLUS_M;
    else if (!strcmp(comp_str, "D-A")) *comp = COMP_D_MINUS_A;
    else if (!strcmp(comp_str, "D-M")) *comp = COMP_D_MINUS_M;
    else if (!strcmp(comp_str, "A-D")) *comp = COMP_A_MINUS_D;
    else if (!strcmp(comp_str, "M-D")) *comp = COMP_M_MINUS_D;
    else if (!strcmp(comp_str, "D&A")) *comp = COMP_D_AND_A;
    else if (!strcmp(comp_str, "D&M")) *comp = COMP_D_AND_M;
    else if (!strcmp(comp_str, "D|A")) *comp = COMP_D_OR_A;
    else if (!strcmp(comp_str, "D|M")) *comp = COMP_D_OR_M;
    else parse_error(line_num, "unknown comp token", comp_str);

    if      (!strcmp(jump_str, ""))    *jump = JUMP_NULL;
    else if (!strcmp(jump_str, "JGT")) *jump = JUMP_JGT;
    else if (!strcmp(jump_str, "JEQ")) *jump = JUMP_JEQ;
    else if (!strcmp(jump_str, "JGE")) *jump = JUMP_JGE;
    else if (!strcmp(jump_str, "JLT")) *jump = JUMP_JLT;
    else if (!strcmp(jump_str, "JNE")) *jump = JUMP_JNE;
    else if (!strcmp(jump_str, "JLE")) *jump = JUMP_JLE;
    else if (!strcmp(jump_str, "JMP")) *jump = JUMP_JMP;
    else parse_error(line_num, "unknown jump token", jump_str);
}

void interpret_A_instruction(uint16_t value, char *bin_instr) {
    uint16_t mask = 1 << 15; // 1000 0000 0000 0000
    for (; mask != 0; mask >>= 1) {
        *bin_instr++ = ((value & mask) == mask) ? '1' : '0';
    }
}

// constants
const int num_op_bits = 3;
const int num_comp_bits = 7;
const int num_dest_bits = 3;
const int num_jump_bits = 3;

void interpret_C_instruction(CompToken comp, DestToken dest, JumpToken jump,
        char *bin_instr) {
    memcpy(bin_instr, "111", num_op_bits);
    bin_instr += num_op_bits;
    switch (comp) {
        case COMP_0:         memcpy(bin_instr, "0101010", num_comp_bits); break;
        case COMP_1:         memcpy(bin_instr, "0111111", num_comp_bits); break;
        case COMP_NEG_1:     memcpy(bin_instr, "0111010", num_comp_bits); break;
        case COMP_D:         memcpy(bin_instr, "0001100", num_comp_bits); break;
        case COMP_A:         memcpy(bin_instr, "0110000", num_comp_bits); break;
        case COMP_M:         memcpy(bin_instr, "1110000", num_comp_bits); break;
        case COMP_NOT_D:     memcpy(bin_instr, "0001101", num_comp_bits); break;
        case COMP_NOT_A:     memcpy(bin_instr, "0110001", num_comp_bits); break;
        case COMP_NOT_M:     memcpy(bin_instr, "1110001", num_comp_bits); break;
        case COMP_NEG_D:     memcpy(bin_instr, "0001111", num_comp_bits); break;
        case COMP_NEG_A:     memcpy(bin_instr, "0110011", num_comp_bits); break;
        case COMP_NEG_M:     memcpy(bin_instr, "1110011", num_comp_bits); break;
        case COMP_D_PLUS_1:  memcpy(bin_instr, "0011111", num_comp_bits); break;
        case COMP_A_PLUS_1:  memcpy(bin_instr, "0110111", num_comp_bits); break;
        case COMP_M_PLUS_1:  memcpy(bin_instr, "1110111", num_comp_bits); break;
        case COMP_D_MINUS_1: memcpy(bin_instr, "0001110", num_comp_bits); break;
        case COMP_A_MINUS_1: memcpy(bin_instr, "0110010", num_comp_bits); break;
        case COMP_M_MINUS_1: memcpy(bin_instr, "1110010", num_comp_bits); break;
        case COMP_D_PLUS_A:  memcpy(bin_instr, "0000010", num_comp_bits); break;
        case COMP_D_PLUS_M:  memcpy(bin_instr, "1000010", num_comp_bits); break;
        case COMP_D_MINUS_A: memcpy(bin_instr, "0010011", num_comp_bits); break;
        case COMP_D_MINUS_M: memcpy(bin_instr, "1010011", num_comp_bits); break;
        case COMP_A_MINUS_D: memcpy(bin_instr, "0000111", num_comp_bits); break;
        case COMP_M_MINUS_D: memcpy(bin_instr, "1000111", num_comp_bits); break;
        case COMP_D_AND_A:   memcpy(bin_instr, "0000000", num_comp_bits); break;
        case COMP_D_AND_M:   memcpy(bin_instr, "1000000", num_comp_bits); break;
        case COMP_D_OR_A:    memcpy(bin_instr, "0010101", num_comp_bits); break;
        case COMP_D_OR_M:    memcpy(bin_instr, "1010101", num_comp_bits); break;
    }
    bin_instr += num_comp_bits;

    switch (dest) {
        case DEST_NULL: memcpy(bin_instr, "000", num_dest_bits); break;
        case DEST_M:    memcpy(bin_instr, "001", num_dest_bits); break;
        case DEST_D:    memcpy(bin_instr, "010", num_dest_bits); break;
        case DEST_MD:   memcpy(bin_instr, "011", num_dest_bits); break;
        case DEST_A:    memcpy(bin_instr, "100", num_dest_bits); break;
        case DEST_AM:   memcpy(bin_instr, "101", num_dest_bits); break;
        case DEST_AD:   memcpy(bin_instr, "110", num_dest_bits); break;
        case DEST_AMD:  memcpy(bin_instr, "111", num_dest_bits); break;
    }
    bin_instr += num_dest_bits;

    switch (jump) {

        case JUMP_NULL: memcpy(bin_instr, "000", num_jump_bits); break;
        case JUMP_JGT:  memcpy(bin_instr, "001", num_jump_bits); break;
        case JUMP_JEQ:  memcpy(bin_instr, "010", num_jump_bits); break;
        case JUMP_JGE:  memcpy(bin_instr, "011", num_jump_bits); break;
        case JUMP_JLT:  memcpy(bin_instr, "100", num_jump_bits); break;
        case JUMP_JNE:  memcpy(bin_instr, "101", num_jump_bits); break;
        case JUMP_JLE:  memcpy(bin_instr, "110", num_jump_bits); break;
        case JUMP_JMP:  memcpy(bin_instr, "111", num_jump_bits); break;
    }
}

void parse_error(int line_num, const char *error_msg, const char *error_val) {
    fprintf(stderr, "Error: parsing line %d: %s: %s\n",
            line_num, error_msg, error_val);
    exit(EXIT_FAILURE);
}

