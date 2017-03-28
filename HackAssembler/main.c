//
// assembler for the Hack computer, project 6 of the nand2tetris course
//

#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "HackAssembler.h"

// program options struct
typedef struct {
    char *in_filename;
    char *out_filename;
} Options;


// parses command line arguments, returns an options structure containing the
// specified input and output filenames or NULL if none are given
Options parse_args(int argc, char *argv[]);

// prints to the specified output stream a small description of program usage
void print_help(FILE *stream, const char *exec_name);

// signals a file handling error message, aborts program execution
void file_error(const char *error_msg, const char *error_val);

// creates the output filename from the input filename, by adding or replacing
// the .hack extension. The output filename is allocated on the heap, should be
// freed if necessary
char *make_out_filename(const char *in_filename, const char *extension);

int main(int argc, char *argv[]) {
    Options opts = parse_args(argc, argv);

    FILE *in_file;
    if (!(in_file = fopen(opts.in_filename, "r"))) {
        file_error("could not open file for reading", opts.in_filename);
    }
    if (opts.out_filename == NULL) { // write to stdout by default
        opts.out_filename = make_out_filename(opts.in_filename, "hack");
    }
    FILE *out_file;
    if (!(out_file = fopen(opts.out_filename, "w"))) {
        file_error("could not open file for writing", opts.out_filename);
    }

    assemble_file(in_file, out_file);

    return 0;
}

Options parse_args(int argc, char *argv[]) {
    Options opts = {.out_filename = 0, .in_filename = 0};
    char *exec_name = argv[0];
    bool args_error = false;
    int optchar;
    while ((optchar = getopt(argc, argv, "ho:")) != -1) {
        switch (optchar) {
            case 'h': // print help
                print_help(stdout, exec_name);
                exit(EXIT_SUCCESS);
                break;
            case 'o': // specify output file
                opts.out_filename = optarg;
                break;
            case ':': // -o without operand
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                args_error = true;
                break;
            case '?': // unrecognized option
                args_error = true;
                break;
        }
    }
    if (args_error || argc == 1) {
        print_help(stderr, exec_name);
        exit(EXIT_FAILURE);
    }
    if (argc > optind+1) { // too many input files
        fprintf(stderr, "Too many input files, using %s\n", opts.in_filename);
    }
    opts.in_filename = argv[optind];
    return opts;
}

void print_help(FILE *stream, const char *exec_name) {
    fprintf(stream, "Usage: %s: ", exec_name);
    fprintf(stream, "[-h] [-o out_file] in_file\n");
}

void file_error(const char *error_msg, const char *error_val) {
    fprintf(stderr, "Error: %s: %s\n", error_msg, error_val);
    exit(EXIT_FAILURE);
}

char *make_out_filename(const char *in_filename, const char *extension) {
    char *out_filename =
        malloc((strlen(in_filename)+strlen(extension))*sizeof(char));
    // dirname may modify the given path, or return a pointer to static storage
    // an aux string is used as an intermediate buffer
    char *aux = malloc(strlen(in_filename)*sizeof(char));
    strcpy(aux, in_filename);
    char *out_dirname = dirname(aux);
    strcpy(out_filename, out_dirname);
    strcat(out_filename, "/");
    // basename may modify the given path, or return a pointer to static storage
    // an aux string is used as an intermediate buffer
    strcpy(aux, in_filename);
    char *out_basename = basename(aux);
    char *current_extension = strrchr(out_basename, '.'); // find last .
    if (current_extension) {
        *current_extension = '\0'; // remove current extension, if any exists
    }
    strcat(out_filename, out_basename);
    strcat(out_filename, ".");
    strcat(out_filename, extension); // add the desired extension
    free(aux);
    return out_filename;
}
