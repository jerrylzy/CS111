CC = gcc
CFLAGS = -g -O2
ID = 404474229
ARCHIVE = lab0-$(ID).tar.gz
sources = Makefile main.c input.txt README bt.png ptr.png

all:
	@$(CC) $(CFLAGS) -o lab0 main.c

check: clean all check-imp

check-imp:
	@./lab0 < input.txt > output.txt ; \
	if [ $$? -ne 0 ] ; \
	then \
		echo "Return value is incorrect! It is: $$?" ; \
		exit 1 ; \
	fi
	@diff -q input.txt output.txt
	@rm -f output.txt
	@./lab0 --input=input.txt > output.txt ; \
	if [ $$? -ne 0 ] ; \
	then \
		echo "Return value is incorrect! It is: $$?" ; \
		exit 1 ; \
	fi
	@diff -q input.txt output.txt
	@rm -f output.txt
	@./lab0 --output=output.txt < input.txt ; \
	if [ $$? -ne 0 ] ; \
	then \
		echo "Return value is incorrect! It is: $$?" ; \
		exit 1 ; \
	fi
	@diff -q input.txt output.txt
	@rm -f output.txt
	@./lab0 --input=input.txt --output=output.txt ; \
	if [ $$? -ne 0 ] ; \
	then \
		echo "Return value is incorrect! It is: $$?" ; \
		exit 1 ; \
	fi
	@diff -q input.txt output.txt
	@rm -f output.txt
	@./lab0 --input=nonexist.txt > output.txt 2> /dev/null ; \
	if [ $$? -ne 1 ] ; \
	then \
		echo "Return value for input error is incorrect! It is: $$?" ; \
		exit 1 ; \
	fi
	@echo "Test" > output.txt
	@chmod -w output.txt
	@./lab0 --output=output.txt < input.txt 2> /dev/null ; \
	if [ $$? -ne 2 ] ; \
	then \
		echo "Return value for output error is incorrect! It is: $$?" ; \
		exit 1 ; \
	fi
	@chmod +w output.txt
	@./lab0 --segfault --catch 2> /dev/null ; \
	if [ $$? -ne 3 ] ; \
	then \
		echo "Return value for --catch is incorrect! It is: $?" ; \
		exit 1 ; \
	fi
	@echo "All test cases passed"

dist:
	@tar -czf $(ARCHIVE) $(sources)

clean:
	@rm -f lab0
