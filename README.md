[![CMake](https://github.com/constarg/File-Sorter-Core/actions/workflows/File-Sorter-Core%20Build.yml/badge.svg?branch=main)](https://github.com/constarg/File-Sorter-Core/actions/workflows/File-Sorter-Core%20Build.yml)


# Introduction

Welcome to sushi-shuffle, your new ally in conquering the chaos of file organization. This lightweight and flexible service is designed to rescue your files from the abyss of disorder, operating seamlessly based on a straightforward config file. Imagine giving specific orders to your file-sorter like – "Hey, check /home/username/Desktop/, and if you find any *.py files, move them to /home/username/py!" It's that simple.

# Installation Requirements

Before diving into the sushi-shuffle fun, make sure to install the necessary packages:

```bash
sudo apt-get install build-essential cmake git gcc
```

# Download & Build

Get started by cloning the repository and navigating to the File-Sorter-Core folder:
```
git clone https://github.com/constarg/sushi-shuffle.git
cd sushi-shuffle/
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
Customize your sushi-shuffle experience using the config file located at ~/.local/share/sushi-shuffle/config/config.conf. Set check intervals, debug modes, default paths, and more. The config file has three segments: [check], [targets], and [exclude], allowing you to define locations, file extensions, and rules effortlessly.

# Commands
Command                              | Description                                     | Comments
--------------------------------------|-------------------------------------------------|---------
sushi-shuffle --start-sorter           | Start the sushi shuffle service                      | -
sushi-shuffle --help                   | Displays all available commands.                | -
sushi-shuffle --set-check-interval [value] | Changes the check interval.                  | -
sushi-shuffle --set-default-dir-path [path] | Changes the default location.               | -
sushi-shuffle --set-enable-default-dir [value] | Enable/disable transfers to default dir. | `0:1 Enable the to transfer files in default dir.`
sushi-shuffle --set-debug-log [value]  | Switches between debug and normal mode.         | `1 (debug mode) and 0 (normal mode)`
sushi-shuffle --set-mv-without-ext [value] | Enable/disable moving files without extension. | `0:1 Enable the program to move files without extension.`
sushi-shuffle --add-check [path]        | Adds `[path]` to the list of locations.        | -
sushi-shuffle --add-target [ext] [path] | Adds info for file extension `[ext]` to `[path]`. | -
sushi-shuffle --remove-check [row number] | Deletes item at line `[row_number]`.         | -
sushi-shuffle --list-checks             | Displays all locations the program looks at.    | -
sushi-shuffle --list-targets            | Displays all elements describing file actions. | -
sushi-shuffle --list-options            | Displays program options.                      | -

# Service
Run the program as a service using systemd:
```
systemctl --user start sushi-shuffle
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

# Contributors

<a href="https://github.com/constarg/file-sorter-core/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=constarg/file-sorter-core" />
</a>
