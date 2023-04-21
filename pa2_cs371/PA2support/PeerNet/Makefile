CC = gcc
CFLAGS = -g -Wall

all: Peer

Peer: Peer.c PeerUtility.c DieWithMessage.c
	$(CC) $(CFLAGS) -o $@ -std=gnu99 $^ 

clean:
	-rm -f *.o *~ *core* Peer
