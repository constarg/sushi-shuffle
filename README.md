[![CMake](https://github.com/EmbeddedCat/File-Sorter-Core/actions/workflows/File-Sorter-Core%20Build.yml/badge.svg?branch=main)](https://github.com/EmbeddedCat/File-Sorter-Core/actions/workflows/File-Sorter-Core%20Build.yml)

# Introduction

Automatically transfer files, which are scattered outside of their folders, to the folders to which they belong using a
simple config file and a tool.

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
% sudo make install
```

`
The cmake should be from version 3.19 and above, if you do not have this version please go to the following site and download the latest version:
https://cmake.org/download/
`

After this the program will be installed and ready to run.

# Config

The program gets the information it needs from a config file, which is like the following.

```
[basic_config]
check_interval 3000
parse_interval 5000
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
 `parse_interval`         | The time ( seconds ) to read the config file again for any changes.                                                                                                                                                                                                                                                                                                                                                                          
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

# Tool

Due to the nature of this program and this config i made a tool that helps the user to do the following:<br>

- Add locations to the config check field.<br>
- Be able to add targets.<br>
- To be able to delete check and targets.<br>
- To be able to see the check and targets.<br>
- To be able to change the program settings.<br><br>

The tool described above can be found here:<br>
https://github.com/EmbeddedCat/file-sorter-terminal-tool

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
