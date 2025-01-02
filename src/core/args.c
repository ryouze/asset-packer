/**
 * @file args.c
 */

#include <stdio.h>   // for fputs, stderr, stdout, puts
#include <stdlib.h>  // for exit, EXIT_FAILURE, EXIT_SUCCESS
#include <string.h>  // for strcmp

#include "args.h"
#include "version.h"

args__args_t args__parse_args(const int argc,
                              char **argv)
{
    // Define the help message
    const char help_message[] =
        "Usage: asset-packer [-h] [-v] [-i PATH] [-o PATH] path...\n"
        "\n"
        "Embed assets (e.g., images, sounds, fonts) into C++ headers.\n"
        "\n"
        "Positional arguments:\n"
        "  path           input filepath (if -i is not used)\n"
        "\n"
        "Optional arguments:\n"
        "  -h, --help     prints help message and exits\n"
        "  -v, --version  prints version and exits\n"
        "  -i PATH        explicit input filepath\n"
        "  -o PATH        explicit output filepath\n\n";

    // Validate basic arguments
    if (argc <= 0 || argv == NULL) {
        fputs("Error: Invalid arguments provided\n", stderr);
        fputs(help_message, stderr);
        exit(EXIT_FAILURE);
    }

    // Initialize arguments structure with default values
    args__args_t args = {.input_filepath = NULL, .output_filepath = NULL};

    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == NULL) {
            fputs("Error: Null argument encountered\n\n", stderr);
            fputs(help_message, stderr);
            exit(EXIT_FAILURE);
        }

        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            fputs(help_message, stdout);
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            puts(PROJECT_VERSION);
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 < argc && argv[i + 1] != NULL && argv[i + 1][0] != '-') {
                args.input_filepath = argv[++i];
            }
            else {
                fputs("Error: Missing or invalid value for '-i'\n\n", stderr);
                fputs(help_message, stderr);
                exit(EXIT_FAILURE);
            }
        }
        else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc && argv[i + 1] != NULL && argv[i + 1][0] != '-') {
                args.output_filepath = argv[++i];
            }
            else {
                fputs("Error: Missing or invalid value for '-o'\n\n", stderr);
                fputs(help_message, stderr);
                exit(EXIT_FAILURE);
            }
        }
        else if (!args.input_filepath) {
            args.input_filepath = argv[i];
        }
        else {
            fputs("Error: Invalid argument '", stderr);
            fputs(argv[i], stderr);
            fputs("'\n\n", stderr);
            fputs(help_message, stderr);
            exit(EXIT_FAILURE);
        }
    }

    // If no input file was provided, exit
    if (!args.input_filepath) {
        fputs("Error: Missing input file\n\n", stderr);
        fputs(help_message, stderr);
        exit(EXIT_FAILURE);
    }

    return args;
}
