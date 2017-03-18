CC = gcc
# If you want to test on the student server, just remove the
# '#' character below
CFLAGS = -std=gnu11 -O2 -lm -lmraa -Wall -Wextra #-DSTUDENT_SERVER
PTFLAGS = -lpthread
SSLFLAGS = -lssl -lcrypto -DTLS_TEST_DRIVER
ID = 404474229
ARCHIVE = lab4-$(ID).tar.gz
sources = Makefile *.c *.h README* *.log *.jpg

all: lab4_1 lab4_2 lab4_3

lab4_1_file = temp_sensor_disp.o lab4_1.o
lab4_1: $(lab4_1_file)
	@$(CC) $(CFLAGS) -o $@ $(lab4_1_file)
temp_sensor_disp.o: temp_sensor.h
	$(CC) $(CFLAGS) -DNO_DISP -c -o $@ temp_sensor.c
lab4_1.o: temp_sensor.h

lab4_td_file = rgb_lcd.o temp_sensor.o
lab4_2_file = $(lab4_td_file) test_driver.o lab4_td.o
lab4_2: $(lab4_2_file) 
	@$(CC) $(CFLAGS) $(PTFLAGS) -o $@ $(lab4_2_file)
test_driver.o temp_sensor.o rgb_lcd.o: rgb_lcd.h
test_driver.o temp_sensor.o: temp_sensor.h
test_driver.o lab4_td.o: test_driver.h
lab4_td.o: test_driver.h

lab4_3_file = $(lab4_td_file) tls_test_driver.o tls_lab4_td.o
lab4_3: $(lab4_3_file)
	@$(CC) $(CFLAGS) $(PTFLAGS) $(SSLFLAGS) -o $@ $(lab4_3_file)
tls_test_driver.o: rgb_lcd.h temp_sensor.h test_driver.h
	$(CC) $(CFLAGS) $(PTFLAGS) $(SSLFLAGS) -c -o $@ test_driver.c
tls_lab4_td.o: test_driver.h
	$(CC) $(CFLAGS) $(PTFLAGS) $(SSLFLAGS) -c -o $@ lab4_td.c

dist:
	@tar -czf $(ARCHIVE) $(sources)

clean:
	@rm -f lab4_? *.o *.log

