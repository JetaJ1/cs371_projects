CC = gcc
CFLAGS = -g -Wall

all: MulticastSender MulticastReceiver

MulticastSender: MulticastSender.c DieWithMessage.c
	$(CC) $(CFLAGS) -o $@ -std=gnu99 $^ 

MulticastReceiver: MulticastReceiver.c DieWithMessage.c
	$(CC) $(CFLAGS) -o $@ -std=gnu99 $^ 

clean:
	-rm -f *.o *~ *core* MulticastSender MulticastReceiver
