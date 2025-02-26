# Sublime Text / Sublime Merge license patcher

This is a simple patcher for [Sublime Text](https://www.sublimetext.com/) and [Sublime Merge](https://www.sublimemerge.com/) that allows you to bypass the license check.   
It works by patching the license check functions in the binary to accept any license key.

## Usage

1. Compile the patcher using `make`, `cmake`, or your favorite C++ compiler.
2. Run the patcher with the path to the `sublime_text` or `sublime_merge` binary as the first argument.
3. If the patch was successful, the binary will be backed up with a `.bak` extension.
4. Run Sublime Text or Sublime Merge and enter any license key.
5. Enjoy!

The patcher has a few CLI options:

- `-h`, `--help`: Show the help message
- `-s`, `--scan-fs`: Scan filesystem for Sublime binaries
- `-l`, `--log-level`: Set log level (trace, debug, info, warn, error, critical)
- `-b`, `--no-backup`: Do not create a backup of the original binary

By default, it will only check the current directory for the binary.  
Using the `-s` option will try and locate the binary in the default expected locations.  
_Paths documented in [Sublime Text](https://www.sublimetext.com/docs/command_line.html) and [Sublime Merge](https://www.sublimemerge.com/docs/command_line) CLI documentation._

If you want to revert the patch, simply delete the patched binary and rename the backup to the original name.

## Supported versions

The patcher has been tested with Sublime Text 4 and Sublime Merge 2.   
It works on the Windows, macOS, and Linux editions.

If you encounter any issues, please open an issue or a pull request.  
Be sure to mention the version of Sublime Text or Sublime Merge you are using, and your operating system.

## Contributing

Contributions are welcome. If you have any improvements or bug fixes, feel free to open a pull request.  
Make sure to follow the style of the existing code (as set by `.clang-format`), and to test your changes.

## Special thanks

- The Sublime Text and Sublime Merge developers for creating such great software.
- [spdlog](https://github.com/gabime/spdlog) for the simple C++ logging library.
- [CLI11](https://github.com/CLIUtils/CLI11) for the command-line parser.
- [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) for the CMake dependency manager.
- [Defuse](https://defuse.ca/) for the useful Online x86 / x64 Assembler and Disassembler.
- [HxD](https://mh-nexus.de/en/hxd/) for the awesome hex editor.
- [sublime-text-4-patcher](https://github.com/rainbowpigeon/sublime-text-4-patcher) for a Python version of the patcher from which I took some inspiration.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
