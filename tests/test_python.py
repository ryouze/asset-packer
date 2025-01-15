"""Compare asset-packer output with a naive Python implementation."""

from difflib import SequenceMatcher
from pathlib import Path
from subprocess import CalledProcessError, CompletedProcess, run


def write_python_version(
    input_path: Path,
    output_path: Path,
) -> None:
    input_data: bytes = input_path.read_bytes()
    output_path.write_text(
        f"""#pragma once

#include <cstddef>  // for std::size_t

namespace fire_app {{

constexpr unsigned char data[] = {{{", ".join(f"0x{byte:02X}" for byte in input_data)}}};

constexpr std::size_t size = sizeof(data);

}}  // namespace fire_app
""",
        encoding="utf-8",
    )


def write_app_version(
    app_path: Path,
    input_path: Path,
    output_path: Path,
) -> None:
    try:
        print(f"Running '{app_path}'")
        result: CompletedProcess[str] = run(
            [str(app_path), "-i", str(input_path), "-o", str(output_path)],
            check=True,
            capture_output=True,
            text=True,
        )
        print(result.stdout)
    except CalledProcessError as e:
        print(e.stderr)
        raise


if __name__ == "__main__":
    # Get path to this script's directory (regardless of CWD)
    this_directory = Path(__file__).resolve().parent

    # Clean up old .hpp files
    for hpp_file in this_directory.glob("*.hpp"):
        print(
            f"Removing old file '{hpp_file.name}' ({len(hpp_file.read_text()):,} characters)"
        )
        hpp_file.unlink()

    # Define file paths
    input_path: Path = this_directory / "fire.png"
    output_python_path: Path = this_directory / "fire_python.hpp"
    output_app_path: Path = this_directory / "fire_app.hpp"
    app_path: Path = this_directory.parent / "build" / "asset-packer"

    # Generate files and compare
    write_python_version(input_path, output_python_path)
    write_app_version(app_path, input_path, output_app_path)

    python_data: str = output_python_path.read_text(encoding="utf-8")
    app_data: str = output_app_path.read_text(encoding="utf-8")

    # Calculate string similarity (0-100)
    similarity: float = round(
        SequenceMatcher(None, python_data, app_data).ratio() * 100, 2
    )

    print(f"Python data: {len(python_data):,} characters")
    print(f"App data: {len(app_data):,} characters")
    print(f"Similarity: {similarity}%")

    if similarity < 100:
        raise ValueError("Files are not identical")
