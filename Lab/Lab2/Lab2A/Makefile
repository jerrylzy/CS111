CC = gcc
CFLAGS = -std=gnu11 -pthread
ID = 404474229
ARCHIVE = lab2a-$(ID).tar.gz
sources = Makefile lab2_add.c lab2_list.c SortedList.c SortedList.h \
README* test.sh *.png *.csv

all: build

build: lab2_add lab2_list


lab2_list_file = SortedList.o lab2_list.o
lab2_list: $(lab2_list_file)
	@$(CC) $(CFLAGS) -o $@ $(lab2_list_file)

lab2_list.o SortedList.o: SortedList.h

lab2_add_file = lab2_add.c
lab2_add: $(lab2_add_file)
	@$(CC) $(CFLAGS) -o $@ $(lab2_add_file)

tests: build
	@chmod +x test.sh
	@-./test.sh

graphs:
	@gnuplot lab2_add.gp
	@gnuplot lab2_list.gp

tarball:
	@tar -czf $(ARCHIVE) $(sources)

clean:
	@rm -f lab2_add lab2_list *.o *.png *.csv

