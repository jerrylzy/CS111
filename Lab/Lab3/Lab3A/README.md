# Lab 3A: File System Dump
UID: 404474229
## File description
* `Makefile`: a Makefile that supports `check, dist, clean` options.
* `filedump.c`: C source module that examines a disk image.
* `README`: A markdown readme file without extension.

## Makefile options
1. `make`

        builds the necessary program.
        
2. `make dist`

        builds the distribution tarball.
3. `make clean`

        cleans up any output file and already-built program.
4. `make check`

        invokes the file `test.sh` to perform specified test cases.

## Usage
`./lab3a "disk-file-name"`

For example

`./lab3a disk-image`

## Note about GCC and libc
This is tested on SEASNet server `lnxsrv09` with path `/usr/local/cs/bin` included.

`gcc --version` should output `6.3.0`.

## Note about `make check` and `test.sh`
All provided sample output files are sorted with the below command:
```
sort < filename.csv > filename1.csv
mv filename1.csv filename.csv
```
Files are included under `sample` directory. Please check `test.sh` for more details.
