CC = gcc
CFLAGS = -std=gnu11 -O2
ID = jerry-liu
ARCHIVE = lab1-$(ID).tar.gz
sources = Makefile simpsh.c command.c command.h file.c file.h \
signal.c signal.h utility.c utility.h README* test.sh

all: simpsh

simpsh_file = simpsh.o command.o file.o signal.o utility.o
simpsh: $(simpsh_file)
	@$(CC) $(CFLAGS) -o $@ $(simpsh_file)

simpsh.o command.o utility.o: command.h
simpsh.o file.o utility.o: file.h
simpsh.o signal.o: signal.h
simpsh.o command.o utility.o file.o signal.o: utility.h

check: simpsh
	@chmod +x test.sh
	@./test.sh

dist:
	@tar -czf $(ARCHIVE) $(sources)

clean:
	@rm -f simpsh *.o

