CC = clang 
CFLAGS = -g -Wall -Wextra -Werror -Wpedantic $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)


all: keygen encrypt decrypt

keygen: keygen.o rsa.o randstate.o numtheory.o
	$(CC) -g -o keygen keygen.o rsa.o randstate.o numtheory.o $(LFLAGS)

encrypt: encrypt.o rsa.o randstate.o numtheory.o
	$(CC) -g -o encrypt encrypt.o rsa.o randstate.o numtheory.o $(LFLAGS)

decrypt: decrypt.o rsa.o randstate.o numtheory.o
	$(CC) -g -o decrypt decrypt.o rsa.o randstate.o numtheory.o $(LFLAGS)

decrypt.o: decrypt.c
	$(CC) $(CFLAGS) -c decrypt.c

encrypt.o: encrypt.c
	$(CC) $(CFLAGS) -c encrypt.c

keygen.o: keygen.c
	$(CC) $(CFLAGS) -c keygen.c

rsa.o: rsa.c
	$(CC) $(CFLAGS) -c rsa.c

numtheory.o: numtheory.c
	$(CC) $(CFLAGS) -c numtheory.c 

randstate.o: randstate.c
	$(CC) $(CFLAGS) -c randstate.c 

clean:
	rm -f keygen encrypt decrypt rsa.pub rsa.priv *.o

format:
	clang-format -i -style=file *.[c,h]
