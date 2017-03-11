# Lab 3B: File System Analysis
UID: 404474229
## File description
* `Makefile`: a Makefile that supports `run, check, dist, clean` options.
* `lab3b.py`: python 3 script that examines file system errors.
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

4. `make run`

        invokes python 3 to run the script.

## Usage
`make run` or `python3 lab3b.py`

## Note about Python version
This is tested on SEASNet server `lnxsrv09` with path `/usr/local/cs/bin` included.

`python3 --version` should output `3.5.2`.

## Note about `make check` and `test.sh`
The provided sample output file is sorted with the below command:
```
sort < lab3b_check.txt > correct.txt
```
Please check `test.sh` for more details.
