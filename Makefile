CFLAGS=-Wall -Werror
SRC=json.c
TESTSRC=test.c
LIBS=-lcunit
TARGET=test

test:
	$(CC) $(CFLAGS) $(SRC) $(TESTSRC) $(LIBS) -o test.out
	./test.out

clean:
	rm test.out