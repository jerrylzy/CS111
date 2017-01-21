# Lab 1A: Simpleton Shell
## File description
* main.c: Source code of our program
* Makefile: Makefile, supports clean, check, dist options
* README: The readme file that provides extra information.
* test.sh: Shell script for testing basic functionality.

## Function tutorials / examples
I mainly checked linux manuals for required functions. I used examples from GNU C Library and Zhaoxing, my TA. I also consulted stackoverflow when I write shellscripts to check exit status in Makefile. For example, to access extra options: [Reference](http://stackoverflow.com/questions/3939157/c-getopt-multiple-value)

## `make check` Methology
This option invokes the file `test.sh` to perform basic functionality checks.

## Limitations
This program is by no way robust. Please conform to standard specified here: [Specification](http://web.cs.ucla.edu/classes/winter17/cs111/assign/lab1.html)