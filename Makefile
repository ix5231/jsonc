CFLAGS=-Wall -Werror
SRC=json.c
TESTSRC=test.c
LIBS=-lcunit
TARGET=test

test:
	$(CC) $(CFLAGS) $(SRC) $(TESTSRC) $(LIBS) -o test.out
	./test.out

debug: debug_address

debug_address:
	$(CC) $(CFLAGS) $(SRC) $(TESTSRC) $(LIBS) -g -fsanitize=address -o test.out

debug_leak:
	$(CC) $(CFLAGS) $(SRC) $(TESTSRC) $(LIBS) -g -fsanitize=leak -o test.out

clean:
	rm test.out