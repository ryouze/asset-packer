# asset-packer

[![CI](https://github.com/ryouze/asset-packer/actions/workflows/ci.yml/badge.svg)](https://github.com/ryouze/asset-packer/actions/workflows/ci.yml)
[![Release](https://github.com/ryouze/asset-packer/actions/workflows/release.yml/badge.svg)](https://github.com/ryouze/asset-packer/actions/workflows/release.yml)
![Release version](https://img.shields.io/github/v/release/ryouze/asset-packer)

asset-packer is a *nix CLI tool for embedding assets (e.g., images, sounds, fonts) into C++ headers.


## Motivation

This project aims to automate the process of embedding assets/resources (e.g., images, sounds, fonts) into C++ headers.

For simple C++ projects (e.g., [SFML](https://github.com/SFML/SFML) games), it is often more convenient to embed resources directly into the source code rather than manage them separately as external files. This allows the project to be self-contained; the user gets a single executable that contains everything it needs to run. This approach becomes infeasible for larger projects, but I haven't gotten there yet.

By all means, existing tools, such as [xxd](https://linux.die.net/man/1/xxd) can achieve this goal, but I wanted to have some degree of control over the generated headers without wrapping them in a custom shell script.


## Features

- Written in modern C (C17).
- Comprehensive documentation with doxygen-style comments.


## Tested Systems

This project has been tested on the following systems:

- macOS 15.2 (Sequoia)
<!-- - Manjaro 24.0 (Wynsdey) -->

Automated testing is also performed on the latest versions of macOS and GNU/Linux using GitHub Actions.


## Pre-built Binaries

Pre-built binaries are available for macOS (ARM64) and GNU/Linux (x86_64). You can download the latest version from the [Releases](../../releases) page.

To remove macOS quarantine, use the following commands:

```sh
xattr -d com.apple.quarantine asset-packer-macos-arm64
chmod +x asset-packer-macos-arm64
```


## Requirements

To build and run this project, you'll need:

- C17 or higher
- CMake


## Build

Follow these steps to build the project:

1. **Clone the repository**:

    ```sh
    git clone https://github.com/ryouze/asset-packer.git
    ```

2. **Generate the build system**:

    ```sh
    cd asset-packer
    mkdir build && cd build
    cmake ..
    ```

    Optionally, you can disable compile warnings by setting `ENABLE_COMPILE_FLAGS` to `OFF`:

    ```sh
    cmake .. -DENABLE_COMPILE_FLAGS=OFF
    ```

3. **Compile the project**:

    To compile the project, use the following command:

    ```sh
    cmake --build . --parallel
    ```

After successful compilation, you can run the program using `./asset-packer`. However, it is highly recommended to install the program, so that it can be run from any directory. Refer to the [Install](#install) section below.

> [!TIP]
> The mode is set to `Release` by default. To build in `Debug` mode, use `cmake .. -DCMAKE_BUILD_TYPE=Debug`.


## Install

If not already built, follow the steps in the [Build](#build) section and ensure that you are in the `build` directory.

To install the program, use the following command:

```sh
sudo cmake --install .
```

On macOS, this will install the program to `/usr/local/bin`. You can then run the program from any directory using `asset-packer`.


## Usage

To run the program, use the following command:

```sh
asset-packer
```

The program expects at least one argument, which is the path to the file to embed.

```sh
asset-packer image.png
```

The program will generate a header file with the same name as the input file, but with a `.hpp` extension. The header file will contain the embedded data as a `const unsigned char[]` array.

In addition:
- The header file is wrapped in an include guard.
- The `cstdint` header is included for `std::size_t`.
- The namespace is the same as the input file name, but without the file extension (e.g., `image`).
- The raw data is called `data`.
- The size of the data is called `size`.

As a result, the header can be used as is with SFML, for example:

```cpp
#pragma once

#include <cstddef>  // for std::size_t

namespace image {

constexpr unsigned char data[] = {0x00, 0x00, 0x01, 0x00, 0x01, ..., 0x60, 0x82};

constexpr std::size_t size = sizeof(data);

}  // namespace image
```

```cpp
#include <SFML/Graphics.hpp>

#include "image.hpp"

int main()
{
    sf::Texture texture;
    texture.loadFromMemory(image::data, image::size);
    return 0;
}
```

You can also explicitly set the input and output file paths using the `-i` and `-o` flags, respectively.

```sh
asset-packer -i image.png -o spaceship.hpp
```


## Flags

```sh
[~] $ asset-packer --help
Usage: asset-packer [-h] [-v] [-i PATH] [-o PATH] path...

Embed assets (e.g., images, sounds, fonts) into C++ headers.

Positional arguments:
  path           input filepath (if -i is not used)

Optional arguments:
  -h, --help     prints help message and exits
  -v, --version  prints version and exits
  -i PATH        explicit input filepath
  -o PATH        explicit output filepath
```


## Testing

Tests are included in the project but are not built by default.

To enable and build the tests manually, run the following commands from the `build` directory:

```sh
cmake .. -DBUILD_TESTS=ON
cmake --build . --parallel
ctest --output-on-failure
```


## Credits

- None, as no external libraries were used.


## Contributing

All contributions are welcome.


## License

This project is licensed under the MIT License.
