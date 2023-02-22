[![CMake](https://github.com/EmbeddedCat/File-Sorter-Core/actions/workflows/File-Sorter-Core%20Build.yml/badge.svg?branch=main)](https://github.com/EmbeddedCat/File-Sorter-Core/actions/workflows/File-Sorter-Core%20Build.yml)

# Introduction

The file-sorter-core is a service provided for Linux systems. It helps organize files that are out of folders and is a flexible and lightweight service. It works based on a config file, in which the user can give specific orders to the file-sorter service, a simple example of an order, in human language, is the following "check for files in the directory **/home/username/Desktop/** and if any file ends with **.py** move it to **/home/username/py** folder".

# Installation requirements

The following packages must be installed.<br>

```
sudo apt-get install build-essential cmake git gcc
```

# Download & Build

First download the program from GitHub and go to the File-Sorter-Core folder.

```
% git clone https://github.com/EmbeddedCat/file-sorter-core.git
% cd file-sorter-core/
```

After installation the program must be built. In order to build the program, the following instructions must be
followed.<br>

```
% mkdir build
% cd build/
% cmake ../
% make
% make install
```

`
The cmake should be from version 3.19 and above, if you do not have this version please go to the following site and download the latest version:
https://cmake.org/download/
`

After this the program will be installed and ready to run.

# Config

File-sorter-core reads the following config file to know what it has to do. The basic job of the file sorter is to check for files, in the user-defined locations, and after that to check if there is any file that has an extension, like .py, that is defined in the config rules and move it to the right location. In short, the config file consists of 3 segments. The first segment is the [check], in this segment, there are the user-defined locations that the file-sorter must monitor for files. Each location is defined in a new line between [check] and [done].<br>
The 2nd segment is the [target], in this segment, there are user-defined rules for each location defined in the [check] segment. Each rule is defined in a new line between [target] and [done].<br>
Lastly, the 3d segment is the [exclude], in this segment, there are also rules, but the rules in this segment tells the file-sorter to ignore some files in a specific location, defined in [check]. The usage of this segment comes in handy when you had a rule in the [target] segment, but you don't want to apply it in all the locations defined in the [check] segment. Each rule is defined in a new line between [exclude] and [done].

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

The config is stored in the following location:<br>
`~/.local/share/file_sorter/config/config.conf`

The information is as follows.<br>

 Field                    | Description                                                                                                                                                                                                                                                                                                                                                                                                                                  
--------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 `check_interval`         | The time ( seconds ) between checks (for new files etc).                                                                                                                                                                                                                                                                                                                                                                                     
 `debug_log`              | For debugging. To enter debug mode, the value of the debugLog field must be changed to 1.                                                                                                                                                                                                                                                                                                                                                    
 `default_dir_path`       | If a file is found outside of a folder and no specific location has been specified to which it should be moved, then it will go to default_dir_path.                                                                                                                                                                                                                                                                                         
 `enable_default_path`    | If this option is enabled, then any file that do not have a specific location to which they should be sent will be sent to the default.                                                                                                                                                                                                                                                                                                      
 `move_files_without_ext` | If this option is enabled, then the sorter will move files that has no extension, otherwise those files will be ignored.                                                                                                                                                                                                                                                                                                                     
 `[check]`                | This field includes all locations where the program will look for files. Each location that enters this field must be entered before `[done_check]`.                                                                                                                                                                                                                                                                                         
 `[targets]`              | This field contains all the file extensions and all the locations that these files should be sent to. Each line in this field consists of two elements that are separated by a space. The first element is the extension of the file and the second is the location where this file should be sent. Also each new line must be entered before `[done_targets]`.                                                                              
 `[exclude]`              | This field contains all the file extensions and all the locations that these files should be ignored. Each line in this field consists of two elements that are separated by a space. The first element is the extension of the file and the second is the location where it must ignore a file. If a file has to be ignored in any location, then * must be placed instead of location. Also each new line must be entered before `[done]`. 

An example of `[check]`:<br>

```
[check]
/home/username/
/home/username/Desktop/
...
[done]
```

An example of `[targets]`:<br>

```
[targets]
.py /home/username/Documents/py/
.cpp /home/username/Documents/cpp/
...
[done]
```

In the example above the first part consists of the extensions `.py` and `.cpp` and the second part of the
locations `/home/username/ Documents/py/` and `/home/username/Documents/cpp/`.

```
[exclude]
.py /home/username/Documents/py/
.c *
...
[done]
```

In the example above the first part consists of the extensions `.py` and `.c` and the second part of the
locations `/home/username/Documents/py/` and `*`.

# Commands

Command |  Description
---------|--------------
`file-sorter --start-sorter` | Start the sorter service
`file-sorter --help` | Displays all available commands. 
`file-sorter --set-check-interval [value]`  | Changes the value of the check interval field to the value `[value]`. 
`file-sorter --set-default-dir-path [path]` | Changes the default location to `[path]`.
`file-sorter --set-enable-default-dir [value]` | 0:1 Enable the to transfer files in default dir.
`file-sorter --set-debug-log [value]` | Changes the logs status based on the value `[value]`. It can get the values `1` `(debug mode)` and `0` `(normal mode)`.
`file-sorter --set-mv-without-ext [value]` | 0:1 Enable the program to move files without extention.
`file-sorter --add-check [path]` | Adds `[path]` to the list of locations that the program looks at.
`file-sorter --add-target [ext] [path]` | Adds to the list the information that the program must send the file with extension `[ext]` to the location `[path]` if such a file is found. 
`file-sorter --remove-check [row number]` | Deletes the item in the line `[row_number]`, the line number appears with the command `--list-checks` or `--list-targets`. 
`file-sorter --list-checks` | Displays all locations that the program looks at.
`file-sorter --list-targets` | Displays all the elements that describe what the program does for a file. 
`file-sorter --list-options` | Displays the program options.


# Service

The program can be run using `systemd`. To start the program from `systemd` the following must be performed.<br>
``` systemctl --user start file-sorter```

# Compatible operating systems

- [X] Linux
- [ ] Windows
- [ ] Mac

# Contributors

<a href="https://github.com/EmbeddedCat/file-sorter-core/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=EmbeddedCat/file-sorter-core" />
</a>
