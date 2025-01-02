/**
 * @file main.c
 */

#include <stdlib.h>  // for EXIT_SUCCESS, EXIT_FAILURE

#include "core/args.h"
#include "core/prepare.h"

/**
 * @brief Entry-point of the application.
 *
 * @param argc Number of command-line arguments (e.g., "2").
 * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
 *
 * @return EXIT_SUCCESS if the application ran successfully, EXIT_FAILURE otherwise.
 */
int main(int argc,
         char **argv)
{
    // Parse command-line arguments
    const args__args_t args = args__parse_args(argc, argv);

    // Generate the header file (infer output filepath if NULL)
    prepare__write_header(args.input_filepath,  // Mandatory
                          args.output_filepath  // Optional
    );

    return EXIT_SUCCESS;
}
