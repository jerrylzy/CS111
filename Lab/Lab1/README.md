# Lab 1B: Simpleton Shell
## File description
* `simpsh.c`: Source code of the main routine.
* `command.*`: Source code to support operations on commands.
* `file.*`: Source code to support operations on file descriptors.
* `signal.*`: Source code to support signal / sigaction calls.
* `utility.*`: Source code of helper functions and header of definitions.
* `Makefile`: Makefile, supports clean, check, dist options.
* `README`: The readme markdown file that provides extra information.
* `test.sh`: Shell script for testing basic functionality.

## Function tutorials / examples
I mainly checked linux manuals for required functions. I used examples from GNU C Library and Zhaoxing, my TA.

## `make check` Methology
This option invokes the file `test.sh` to perform basic functionality checks.

## Limitations
* This program is by no way robust. Please conform to standard specified here: [Specification](http://web.cs.ucla.edu/classes/winter17/cs111/assign/lab1.html)
* Currently `glibc` assigns `O_RSYNC` to the same flag as `O_SYNC`. Thus in this implementation, it conforms to this status. I do not guarantee this will work in the future.
