CC = python3
ID = 404474229
ARCHIVE = lab3b-$(ID).tar.gz
sources = Makefile lab3b.py README* test.sh

all: lab3b

run: lab3b

lab3b:
	@$(CC) $@.py

dist:
	@tar -czf $(ARCHIVE) $(sources)

check: all
	@chmod +x test.sh
	@./test.sh

clean:
	@rm -f lab3b_check.txt
