[![CMake](https://github.com/constarg/File-Sorter-Core/actions/workflows/File-Sorter-Core%20Build.yml/badge.svg?branch=main)](https://github.com/constarg/File-Sorter-Core/actions/workflows/File-Sorter-Core%20Build.yml)


# Introduction

Welcome to file-sorter-core, your new ally in conquering the chaos of file organization on Linux systems. This lightweight and flexible service is designed to rescue your files from the abyss of disorder, operating seamlessly based on a straightforward config file. Imagine giving specific orders to your file-sorter like – "Hey, check /home/username/Desktop/, and if you find any *.py files, move them to /home/username/py!" It's that simple.

# Installation Requirements

Before diving into the file-sorting fun, make sure to install the necessary packages:

```bash
sudo apt-get install build-essential cmake git gcc
```

# Download & Build

Get started by cloning the repository and navigating to the File-Sorter-Core folder:
```
git clone https://github.com/constarg/file-sorter-core.git
cd file-sorter-core/
```
Now, let's build the program:

```
mkdir build
cd build/
cmake ../
make
make install
```

`
Make sure your cmake version is 3.19 or above. If not, grab the latest version from here: https://cmake.org/download/
`

After this the program will be installed and ready to run.

# Config
Customize your file-sorter experience using the config file located at ~/.local/share/file_sorter/config/config.conf. Set check intervals, debug modes, default paths, and more. The config file has three segments: [check], [targets], and [exclude], allowing you to define locations, file extensions, and rules effortlessly.

# Commands
Command                              | Description                                     | Comments
--------------------------------------|-------------------------------------------------|---------
file-sorter --start-sorter           | Start the sorter service                        | -
file-sorter --help                   | Displays all available commands.                | -
file-sorter --set-check-interval [value] | Changes the check interval.                  | -
file-sorter --set-default-dir-path [path] | Changes the default location.               | -
file-sorter --set-enable-default-dir [value] | Enable/disable transfers to default dir. | `0:1 Enable the to transfer files in default dir.`
file-sorter --set-debug-log [value]  | Switches between debug and normal mode.         | `1 (debug mode) and 0 (normal mode)`
file-sorter --set-mv-without-ext [value] | Enable/disable moving files without extension. | `0:1 Enable the program to move files without extension.`
file-sorter --add-check [path]        | Adds `[path]` to the list of locations.        | -
file-sorter --add-target [ext] [path] | Adds info for file extension `[ext]` to `[path]`. | -
file-sorter --remove-check [row number] | Deletes item at line `[row_number]`.         | -
file-sorter --list-checks             | Displays all locations the program looks at.    | -
file-sorter --list-targets            | Displays all elements describing file actions. | -
file-sorter --list-options            | Displays program options.                      | -

# Service
Run the program as a service using systemd:
```
systemctl --user start file-sorter
```

# Compatible operating systems

- [X] Linux
- [ ] Windows
- [ ] Mac

# Config Examples
An example of [check]:
```
[check]
/home/username/
/home/username/Desktop/
...
[done]
```

An example of [targets]:
```
[targets]
.py /home/username/Documents/py/
.cpp /home/username/Documents/cpp/
...
[done]
```

An example of [exclude]:
```
[exclude]
.py /home/username/Documents/py/
.c *
...
[done]
```

An example of the complete config file:
```
[basic_config]
check_interval 3000
debug_log 0
default_dir_path /home/username/default_sorter_path/
enable_default_path 1
move_files_without_ext 0

[check]

[done]

[targets]

[done]

[exclude]

[done]
```
Let the file-sorting adventure begin!

# Contributors

<a href="https://github.com/constarg/file-sorter-core/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=constarg/file-sorter-core" />
</a>
