CC=gcc
CFLAGS=-Wall -g

all: RUDP_Sender RUDP_Receiver

RUDP_Sender: sender.o api.o
	$(CC) $(CFLAGS) -o RUDP_Sender sender.o api.o

RUDP_Receiver: receiver.o api.o
	$(CC) $(CFLAGS) -o RUDP_Receiver receiver.o api.o

sender.o: sender.c api.h
	$(CC) $(CFLAGS) -c sender.c

receiver.o: receiver.c api.h
	$(CC) $(CFLAGS) -c receiver.c

api.o: api.c api.h
	$(CC) $(CFLAGS) -c api.c

clean:
	rm -f RUDP_Sender RUDP_Receiver *.o