/**
 * @file test_all.c
 */

#include <stddef.h>  // for size_t
#include <stdio.h>   // for printf, fprintf, fopen, fclose, remove, fwrite, perror
#include <stdlib.h>  // for EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>  // for strcmp

#include "core/args.h"
#include "core/prepare.h"

static int parse_args(void);
static int write_header(void);

/**
 * @brief Entry-point of the test application.
 *
 * @param argc Number of command-line arguments (e.g., "2").
 * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
 *
 * @return EXIT_SUCCESS if the test application ran successfully, EXIT_FAILURE otherwise.
 */
int main(int argc,
         char **argv)
{
    // Define the formatted help message
    const char help_message[] =
        "Usage: tests <test>\n\n"
        "Run automatic tests.\n\n"
        "Positional arguments:\n"
        "  test  name of the test to run ('all' to run all tests)\n";

    // If no arguments, print help message and exit
    if (argc == 1) {
        fprintf(stderr, "%s\n", help_message);
        return EXIT_SUCCESS;
    }

    // Otherwise, define argument to function mapping
    struct Test {
        const char *name;
        int (*func)(void);
    } tests[] = {
        {"parse_args", parse_args},
        {"write_header", write_header},
    };
    const size_t num_tests = sizeof(tests) / sizeof(tests[0]);

    // Get the test name from the command-line arguments
    const char *test_name = argv[1];

    // Run all tests sequentially and print the results
    if (strcmp(test_name, "all") == 0) {
        int all_passed = 1;

        for (size_t i = 0; i < num_tests; ++i) {
            printf("Running test: %s\n", tests[i].name);
            if (tests[i].func() != EXIT_SUCCESS) {
                fprintf(stderr, "Test failed: %s\n", tests[i].name);
                all_passed = 0;
            }
        }

        return all_passed ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else {
        for (size_t i = 0; i < num_tests; ++i) {
            if (strcmp(test_name, tests[i].name) == 0) {
                return tests[i].func();
            }
        }

        fprintf(stderr, "Error: Invalid test name: '%s'\n\n%s\n", test_name, help_message);
        return EXIT_FAILURE;
    }
}

static int parse_args(void)
{
    printf("Running test: parse_args\n");

    const char *argv[] = {"asset-packer", "-i", "input.bin", "-o", "output.hpp"};
    const int argc = 5;

    const args__args_t args = args__parse_args(argc, (char **)argv);

    if (!args.input_filepath || strcmp(args.input_filepath, "input.bin") != 0) {
        fprintf(stderr, "Test failed: input_filepath expected 'input.bin', got '%s'\n", args.input_filepath);
        return EXIT_FAILURE;
    }

    if (!args.output_filepath || strcmp(args.output_filepath, "output.hpp") != 0) {
        fprintf(stderr, "Test failed: output_filepath expected 'output.hpp', got '%s'\n", args.output_filepath);
        return EXIT_FAILURE;
    }

    printf("Test passed: parse_args\n");
    return EXIT_SUCCESS;
}

static int write_header(void)
{
    printf("Running test: write_header\n");

    const char *input_filepath = "test_input.bin";
    const char *output_filepath = "test_output.hpp";

    // Create a dummy input file
    FILE *input_file = fopen(input_filepath, "wb");
    if (!input_file) {
        perror("Failed to create test input file");
        return EXIT_FAILURE;
    }
    const unsigned char data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    fwrite(data, sizeof(unsigned char), sizeof(data), input_file);
    fclose(input_file);

    // Run the function
    prepare__write_header(input_filepath, output_filepath);

    // Verify the output file
    FILE *output_file = fopen(output_filepath, "r");
    if (!output_file) {
        fprintf(stderr, "Test failed: Output file '%s' was not created.\n", output_filepath);
        return EXIT_FAILURE;
    }
    fclose(output_file);

    // Cleanup
    remove(input_filepath);
    remove(output_filepath);

    printf("Test passed: write_header\n");
    return EXIT_SUCCESS;
}
