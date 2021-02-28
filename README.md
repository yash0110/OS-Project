# My Shell
This is a self developed basic version of Bourne shell ("Bash") with multiple similar functionalities as well as many new ones.

## How To Run
Step 1. Download the source code to system in zip format or clone repo.

Step 2. On linux shell navigate to
```
"Download-Location"/OS-Project-main/OS-Project-main
```
Step 3. compile the shell using command
```
g++ myShell.cpp -o myShell
```
Step 4. run using command
```
./myShell
```
## Tech Stack Used
All the functionalities are programmed in C++ language with the help of libraries such as "sys/wait.h", "sys/types.h", "signal.h" ... etc.

## Important Logic Used
Basic functionality such as...

- Executing commands and processed by forking the main process.
- Changing directory using "cd" command.
- Exiting the shell.
- showing various builtin commands using the help command
- Running and monitoring process in background.
- Manual as well as auto killing background process using signals.

All these functionalities work smooth and bug free.

## New Additions
Some new additions made to this personalised shell include...
- creating aliases for various commands which makes it very easy to handle long commands
- added the feature to bring a background to foreground using its process id
- added the "history" feature to show the history of commands which have been entered(up to the latest 999 entries)

