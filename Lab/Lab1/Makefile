CC = gcc
CFLAGS = -std=c11 -O2
ID = jerry-liu
ARCHIVE = lab1-$(ID).tar.gz
sources = Makefile simpsh.c README* test.sh

all: simpsh

simpsh_file = simpsh.c
simpsh: $(simpsh_file)
	@$(CC) $(CFLAGS) -o $@ $(simpsh_file)

check: simpsh
	@chmod +x test.sh
	@./test.sh

dist:
	@tar -czf $(ARCHIVE) $(sources)

clean:
	@rm -f simpsh
