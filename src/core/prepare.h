/**
 * @file prepare.h
 *
 * @brief Infer filepaths and embed data into C++ headers.
 */

#pragma once

/**
 * @brief Embed binary data into a C++ header.
 *
 * @param input_filepath Path to the input file (e.g., "image.png").
 *
 * @param output_filepath Path to the output C++ header file (e.g., "car.hpp"). If NULL, infer it from input by replacing the extension with ".hpp".
 */
void prepare__write_header(const char *input_filepath,
                           const char *output_filepath);
