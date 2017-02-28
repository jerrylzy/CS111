CC = gcc
CFLAGS = -std=gnu11 -pthread
GPERF_DIR = $(HOME)/gperftools
GPERF_PATH = $(GPERF_DIR)/bin
LIB_PATH = $(GPERF_DIR)/lib
LIBRARY = -g -L$(LIB_PATH) -lprofiler
ID = 404474229
ARCHIVE = lab2b-$(ID).tar.gz
sources = Makefile lab2_add.c lab2_list.c SortedList.c SortedList.h \
README* test.sh test_p1.sh *.png *.csv profile.gperf lab2.gp

all: build

build: lab2_list lab2_add

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
	@chmod +x test_p1.sh
	@./test_p1.sh

profile: profile.gperf

raw_file = raw.gperf
thread_func = run_thread
profile.gperf: build
	@mv lab2_list lab2_list_np
	@$(CC) $(CFLAGS) $(LIBRARY) -o lab2_list SortedList.o lab2_list.c
	@rm -f $(raw_file)
	@LD_PRELOAD=$(LIB_PATH)/libprofiler.so.0 \
	CPUPROFILE=$(raw_file) \
	./lab2_list --threads=12 --iterations=1000 --sync=s
	@$(GPERF_PATH)/pprof --text lab2_list $(raw_file) > $@
	@$(GPERF_PATH)/pprof --list=$(thread_func) lab2_list $(raw_file) >> $@
	@rm -f $(raw_file)
	@mv lab2_list_np lab2_list

graphs:
	@gnuplot lab2.gp

tarball:
	@tar -czf $(ARCHIVE) $(sources)

clean:
	@rm -f *.png *.csv *.gperf lab2_list lab2_add *.o *.gch

