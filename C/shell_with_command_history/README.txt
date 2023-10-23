Name : Sunit Singh
Date : May 13, 2023

Instructions :

Compiling: run `make`

Running: The compilation output of `make` will create an executable `mysh`. To run the executable run `./mysh` . This will run the terminal created in the setup file.

History of upto 10 commands will be stored in the shell. To view history enter `Ctrl+C`.
To run a command from history, enter `r x` where x is the first letter of the command from history.

NOTE : Please enter valid shell commands.


### Example (if the history is of length 3):

.
.
.
COMMAND->`Ctrl+C`
pwd
ls -al
ls lab0

COMMAND->r l    <------- will run `ls -al` command from history and the history will be updated.

COMMAND->`Ctrl+C`
ls -al
ls lab0
ls -al

COMMAND->ls lab1

COMMAND->`Ctrl+C`
ls lab0
ls -al
ls lab1

COMMAND->pwd

COMMAND->`Ctrl+C`
ls -al
ls lab1
pwd

COMMAND->r p      <------- will run `pwd` command from history and the history will be updated.

COMMAND->`Ctrl+C`
ls lab1
pwd
pwd
