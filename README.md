
# Introduction
This program transfers all files outside of folders 
to the specific folders to which they belong. 


# Download & Build
First download the program from github and go to the File-Sorter-Core folder.
```
% git clone https://github.com/rounnus/File-Sorter-Core.git
% cd File-Sorter-Core/
```
After installation the program must be built. In order to build the program, the following instructions must be followed.<br>
```
% mkdir build
% cd build/
% cmake ../
% make
% sudo make install
```
After this the program will be installed and ready to run.

# Config
The program gets the information it needs from a config file, which is like the following 

```
[basic_config]
checkInterval 3000
parseInterval 5000
debugLog 0
defaultDirPath /home/username/default_sorter_path/

[check]

[done_check]

[targets]

[done_targets]
```
The information is as follows.<br>
`- checkInterval`:  The time between checks (for new files etc).<br>
`- parseInterval`:  The time to read the config file again for any changes.<br>
`- debugLog`:  For debugging. To enter debug mode, the value of the debugLog field must be changed to 1.<br>
`- defaultDirPath`:  If a file is found outside of a folder and no specific location has been specified to which it should be moved, then it will go to defaultDirPath.<br>
`- [check]`:   This field includes all locations where the program will look for files. Each location that enters this field must be entered before `[done_check]`. An example is the following.
```
[check]
/home/username/
/home/username/Desktop/
...
[done_check]
```
`- [targets]`:   This field consists of two parts. The first part refers to the file extension and the second part refers to the location that each file should send with this extension. An example is the following.<br>
```
[targets]
.py /home/username/Documents/py/
.cpp /home/username/Documents/cpp/
...
[done_targets]
```
In the example above the first part consists of the extensions `.py` and `.cpp` and the second part of the locations `/home/username/ Documents/py/` and `/home/username/Documents/cpp/`. Also all data must be entered before `[done_targets]`.

# Service
The program can be run using `systemd`. To start the program from `systemd` the following must be performed.<br>
``` systemctl --user start file-sorter```
