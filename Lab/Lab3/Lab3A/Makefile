CC = gcc
CFLAGS = -std=gnu11 -O2 -Wall -Wextra
ID = 404474229
ARCHIVE = lab3a-$(ID).tar.gz
sources = Makefile filedump.c README* test.sh

all: lab3a

lab3a_file = filedump.c
lab3a: $(lab3a_file)
	@$(CC) $(CFLAGS) -o $@ $(lab3a_file)

dist:
	@tar -czf $(ARCHIVE) $(sources)

check: all
	@chmod +x test.sh
	@./test.sh

clean:
	@rm -f lab3a *.o *.csv

