/**
 * @file prepare.c
 */

#include <stdbool.h>  // for bool, true, false
#include <stddef.h>   // for size_t
#include <stdio.h>    // for FILE, fputs, fopen, fclose, perror, snprintf, fread, ferror, clearerr
#include <stdlib.h>   // for exit, EXIT_FAILURE, EXIT_SUCCESS, malloc, free
#include <string.h>   // for strrchr, strlen, strcat, strncpy

#include "prepare.h"

/**
 * @brief Create output file path by replacing the input filepath's extension with ".hpp".
 *
 * @param input_filepath Input file path (e.g., "image.png").
 *
 * @return Output file path (e.g., "image.hpp").
 *
 * @note The caller is responsible for freeing the returned memory.
 */
static char *create_output_filepath_d(const char *input_filepath)
{
    if (!input_filepath) {
        perror("NULL input_filepath provided");
        exit(EXIT_FAILURE);
    }

    // Locate the position of the last dot in the input filepath
    const char *dot_position = strrchr(input_filepath, '.');
    const char *extension = ".hpp";
    const size_t base_length = dot_position ? (size_t)(dot_position - input_filepath) : strlen(input_filepath);

    // Get extension length for later use
    const size_t extension_len = strlen(extension);

    // Allocate memory for the output file path
    char *output_filepath_d = malloc(base_length + extension_len + 1);
    if (!output_filepath_d) {
        perror("Error allocating memory for output file path");
        exit(EXIT_FAILURE);
    }

    // Copy the base name
    snprintf(output_filepath_d, base_length + 1, "%s", input_filepath);

    // Append the new extension
    snprintf(output_filepath_d + base_length, extension_len + 1, "%s", extension);

    return output_filepath_d;
}

/**
 * @brief Extract the base filename (without extension) for use as a namespace name.
 *
 * @param filepath Full file path.
 *
 * @return Base filename (e.g., "image" from "image.png").
 *
 * @note The caller is responsible for freeing the returned memory.
 */
static char *extract_base_filename_d(const char *filepath)
{
    if (!filepath) {
        perror("NULL filepath provided");
        exit(EXIT_FAILURE);
    }

    // Locate the last slash and dot in the file path
    const char *last_slash = strrchr(filepath, '/');
    const char *filename = last_slash ? last_slash + 1 : filepath;

    const char *dot_position = strrchr(filename, '.');
    const size_t base_length = dot_position ? (size_t)(dot_position - filename) : strlen(filename);

    // Allocate memory for the base filename
    char *base_name_d = malloc(base_length + 1);
    if (!base_name_d) {
        perror("Error allocating memory for base filename");
        exit(EXIT_FAILURE);
    }

    // Copy the base name and null-terminate
    strncpy(base_name_d, filename, base_length);
    base_name_d[base_length] = '\0';

    return base_name_d;
}

void prepare__write_header(const char *input_filepath,
                           const char *output_filepath)
{
    if (!input_filepath) {
        perror("NULL input_filepath provided");
        exit(EXIT_FAILURE);
    }

    // Infer the output file path if not provided
    char *dynamically_inferred_filepath_d = NULL;
    if (!output_filepath) {
        dynamically_inferred_filepath_d = create_output_filepath_d(input_filepath);
        output_filepath = dynamically_inferred_filepath_d;
    }

    // Extract the namespace name from the output file path
    char *namespace_name_d = extract_base_filename_d(output_filepath);

    // Open input and output files
    FILE *input_file = fopen(input_filepath, "rb");
    if (!input_file) {
        perror("Error opening input file");
        free(dynamically_inferred_filepath_d);
        free(namespace_name_d);
        exit(EXIT_FAILURE);
    }

    FILE *output_file = fopen(output_filepath, "w");
    if (!output_file) {
        perror("Error opening output file");
        fclose(input_file);
        free(dynamically_inferred_filepath_d);
        free(namespace_name_d);
        exit(EXIT_FAILURE);
    }

    // Write C++ header content
    fputs("#pragma once\n\n#include <cstddef>  // for std::size_t\n\n", output_file);

    // Prepare and write namespace opening
    const size_t namespace_open_len = strlen("namespace ") + strlen(namespace_name_d) + strlen(" {\n\n") + 1;
    char *namespace_opening_d = malloc(namespace_open_len);
    if (!namespace_opening_d) {
        perror("Error allocating memory for namespace opening");
        fclose(input_file);
        fclose(output_file);
        free(dynamically_inferred_filepath_d);
        free(namespace_name_d);
        exit(EXIT_FAILURE);
    }

    snprintf(namespace_opening_d, namespace_open_len, "namespace %s {\n\n", namespace_name_d);
    fputs(namespace_opening_d, output_file);
    free(namespace_opening_d);

    fputs("constexpr unsigned char data[] = {", output_file);

    // Write the binary data as a byte array
    const size_t buffer_size = 1024;
    unsigned char *data_buffer_d = malloc(buffer_size);
    if (!data_buffer_d) {
        perror("Error allocating memory for data buffer");
        fclose(input_file);
        fclose(output_file);
        free(dynamically_inferred_filepath_d);
        free(namespace_name_d);
        exit(EXIT_FAILURE);
    }

    bool is_first_byte = true;
    size_t bytes_read = 0;

    while ((bytes_read = fread(data_buffer_d, 1, buffer_size, input_file)) > 0) {
        for (size_t i = 0; i < bytes_read; ++i) {
            if (!is_first_byte) {
                fputs(", ", output_file);
            }
            is_first_byte = false;

            char byte_string[8];  // enough for "0xFF\0"
            snprintf(byte_string, sizeof(byte_string), "0x%02X", data_buffer_d[i]);
            fputs(byte_string, output_file);
        }
    }

    if (ferror(input_file)) {
        perror("Error reading input file");
        clearerr(input_file);  // Reset the error state
        fclose(input_file);
        fclose(output_file);
        free(data_buffer_d);
        free(dynamically_inferred_filepath_d);
        free(namespace_name_d);
        exit(EXIT_FAILURE);
    }

    // Close the array declaration
    fputs("};\n\n", output_file);

    // Write the array size
    fputs("constexpr std::size_t size = sizeof(data);\n\n", output_file);

    // Write the namespace closing
    const size_t namespace_close_len = strlen("}  // namespace ") + strlen(namespace_name_d) + 1;
    char *namespace_closing_d = malloc(namespace_close_len + 1);
    if (!namespace_closing_d) {
        perror("Error allocating memory for namespace closing");
        fclose(input_file);
        fclose(output_file);
        free(data_buffer_d);
        free(dynamically_inferred_filepath_d);
        free(namespace_name_d);
        exit(EXIT_FAILURE);
    }
    snprintf(namespace_closing_d, namespace_close_len + 1, "}  // namespace %s\n", namespace_name_d);
    fputs(namespace_closing_d, output_file);
    free(namespace_closing_d);

    // Cleanup
    fclose(input_file);
    fclose(output_file);
    free(data_buffer_d);
    free(dynamically_inferred_filepath_d);
    free(namespace_name_d);
}
