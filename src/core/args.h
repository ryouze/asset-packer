/**
 * @file args.h
 *
 * @brief Process command-line arguments.
 */

#pragma once

/**
 * @brief Struct that represents parsed command-line arguments.
 */
typedef struct {
    /**
     * @brief Mandatory input filepath.
     */
    const char *input_filepath;

    /**
     * @brief Optional output filepath (might be NULL).
     */
    const char *output_filepath;
} args__args_t;

/**
 * @brief Parse command-line arguments.
 *
 * If the help message, version, or an error is encountered, the program will exit.
 *
 * @param argc Number of command-line arguments (e.g., "2").
 * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
 *
 * @return args_t Parsed arguments.
 */
args__args_t args__parse_args(const int argc,
                              char **argv);
