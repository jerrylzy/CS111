# Lab 0: Input, output redirection
## File description
* bt.png: The screenshot of backtrace after the program crashed in gdb.
* ptr.png: The screenshot of the value of pointer before crash.
* input.txt: The input file we selected in our test.
* main.c: Source code of our program
* Makefile: Makefile, supports clean, check, dist options
* README: The readme file that provides extra information.

## Function tutorials / examples
I mainly checked linux manuals for required functions. I used examples from GNU C Library and Zhaoxing, my TA. I also consulted stackoverflow when I write shellscripts to check exit status in Makefile. 

## `make check` Methology
This functionality is limited but useful. It checks whether the program is functioning correctly. It mainly checks the exit status of the run and it reports success if the program returns expected values and copies a given input to a given output for all required cases.

## Limitations
The program assumes there is no aliasing problem. For example, input file refers to the same file as output file.