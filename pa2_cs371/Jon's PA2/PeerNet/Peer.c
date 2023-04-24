/* 
 * Copyright (c) 2023 Yang Xiao, University of Kentucky
 * All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"


// PROTOTYPES
bool RecvIsJoining(const int clientSock);
void SendIsJoining(const int serverSock, const bool isJoining);
bool RecvHasMessage(const int clientSock);
void SendHasMessage(const int serverSock, const bool hasMessage);
void SendNumPeers(const int clientSock);
int RecvNumPeers(const int serverSock);
void SendNextPeer(const int clientSock);
struct peerInfo RecvNextPeer(const int serverSock);
void SendMessage(const int serverSock, const char *message);
bool CheckIsPeer(struct peerInfo *client);


// GLOBALS
static const int MAXPENDING = 5; // Maximum outstanding connection requests

static const char* ACK = "ack";
static const char* TRUE_STR = "true";
static const char* FALSE_STR = "false";

int numPeers = 0;
struct peerInfo peers[MAXPEERS];


int main(int argc, char *argv[]) {

  // INIT
  bool isJoining = false;
  bool hasMessage = false; // If true, send message (prompt input) to peer
  char *peerIP;
  char *peerPort;
  char *message;

  // ---- Handle command line arguments ----
  if (argc < 2 || argc > 8) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)", "<Port> [-i <Peer IP> -p <Peer Port>] [-s <Message>]");

  char *this_portAsStr = argv[1];
  in_port_t servPort = atoi(this_portAsStr); // First arg:  local port

  int opt;

  while ((opt = getopt(argc, argv, "i:p:s")) != -1) {
    switch (opt) {
      case 'i':
        isJoining = true;
        peerIP = argv[3];
        break;
      case 'p':
        peerPort = argv[5];
        break;
      case 's':
        hasMessage = true;
        message = argv[7];
        break;
      default:
        DieWithUserMessage("Parameter(s)", "<Port> [-i <Peer IP> -p <Peer Port>] [-s <Message>]");
        break;
    }
  }


  // CLIENT SIDE
  // Create socket for outgoing connection (as client to a remote peer)
  if (isJoining) {
    struct peerInfo server;
    struct peerInfo nextPeer;

    server.sock = SetupTCPClientSocket(peerIP, peerPort, this_portAsStr);
    strcpy(server.IP, peerIP);
    strcpy(server.port, peerPort);

    SendIsJoining(server.sock, isJoining);
    SendHasMessage(server.sock, hasMessage);

    int serverNumPeers = RecvNumPeers(server.sock);
    if (serverNumPeers > 0) {
      nextPeer = RecvNextPeer(server.sock);
    }

    // NOTE: although this code works, it requires that the server send back the
    // last peer (index 1) rather than the first (index 0). This causes the peers to connect
    // in the opposite order, which would fail the test cases.

    // while (serverNumPeers >= MAXPEERS) {
    //   if (DEBUG)
    //     printf("could not connect to peer at %s:%s\n", server.IP, server.port);

    //   close(server.sock);
    //   server = nextPeer;

    //   server.sock = SetupTCPClientSocket(nextPeer.IP, nextPeer.port, this_portAsStr);

    //   SendIsJoining(server.sock, isJoining);
    //   SendHasMessage(server.sock, hasMessage);

    //   serverNumPeers = RecvNumPeers(server.sock);
    //   if (serverNumPeers > 0) {
    //     nextPeer = RecvNextPeer(server.sock);
    //   }
    // }

    // If the server is full, go to its next peer
    if (serverNumPeers >= MAXPEERS) {
      if (DEBUG)
        printf("could not connect to peer at %s:%s\n", server.IP, server.port);

      close(server.sock);
      server = nextPeer;

      server.sock = SetupTCPClientSocket(nextPeer.IP, nextPeer.port, this_portAsStr);

      SendIsJoining(server.sock, isJoining);
      SendHasMessage(server.sock, hasMessage);

      serverNumPeers = RecvNumPeers(server.sock);
      if (serverNumPeers > 0) {
        nextPeer = RecvNextPeer(server.sock);
      }
    }

    printf("Connecting to peer at %s:%s\n", server.IP, server.port);

    if (serverNumPeers > 0) {
      printf("Received ack, who has a peer at %s:%s\n", nextPeer.IP, nextPeer.port);
    }
    else {
      puts("Received ack");
    }

    peers[numPeers] = server;
    numPeers++;

    if (hasMessage) {
      SendMessage(server.sock, message);
    }

    close(server.sock);
  }


  // SERVER SIDE
  // Create socket for incoming connections
  int servSock; // Socket descriptor for server
  if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithSystemMessage("socket() failed");

  // Construct local address structure
  struct sockaddr_in servAddr;                  // Local address
  memset(&servAddr, 0, sizeof(servAddr));       // Zero out structure
  servAddr.sin_family = AF_INET;                // IPv4 address family
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
  servAddr.sin_port = htons(servPort);          // Local port

  // Bind to the local address
  if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0)
    DieWithSystemMessage("bind() failed");

  // Mark the socket so it will listen for incoming connections
  if (listen(servSock, MAXPENDING) < 0)
    DieWithSystemMessage("listen() failed");
  
  printf("This peer is established at 127.0.0.1:%d\n", servPort);


  // MAIN SERVER LOOP
  while (1) { // Run forever
    struct sockaddr_in clntAddr; // Client address
    // Set length of client address structure (in-out parameter)
    socklen_t clntAddrLen = sizeof(clntAddr);

    struct peerInfo client;

    // Wait for a client to connect
    client.sock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
    if (client.sock < 0)
      DieWithSystemMessage("accept() failed");

    // client.sock is connected to a client!
    const char *retVal = inet_ntop(AF_INET, &(clntAddr.sin_addr), client.IP, INET_ADDRSTRLEN);
    if (retVal == NULL)
      DieWithSystemMessage("inet_ntop() failed");

    // Receive the client's port
    Recv(client.sock, client.port, BUFSIZE, 0, "this_portAsStr");
    Send(client.sock, ACK, strlen(ACK), 0, "this_portAsStr ack");

    int clientIsJoining = RecvIsJoining(client.sock);
    int clientHasMessage = RecvHasMessage(client.sock);

    if (clientIsJoining) {
      SendNumPeers(client.sock);

      if (numPeers > 0) {
        SendNextPeer(client.sock);
      }

      const bool isPeer = CheckIsPeer(&client);

      // Reject new peers over the maximum
      if (!isPeer && numPeers >= MAXPEERS) {
        if (DEBUG)
          printf("attempted peer connection from %s:%d rejected: max peers reached\n", client.IP, ntohs(clntAddr.sin_port));
        close(client.sock);
        continue;
      }
      // Accepts new peers under the maximum
      else if (!isPeer) {
        printf("Connected from peer %s:%d, whose listening port is %s\n", client.IP, ntohs(clntAddr.sin_port), client.port);
        peers[numPeers] = client;
        numPeers++;
      }
    }

    if (clientHasMessage) {
      char message[BUFSIZE]; // Buffer for propagated message
      char hops[BUFSIZE];    // Buffer for number of hops

      // Receive message from client
      ssize_t numBytesRcvd = Recv(client.sock, message, BUFSIZE, 0, "message");
      Send(client.sock, ACK, strlen(ACK), 0, "message ack");

      // Receive number of hops from client
      Recv(client.sock, hops, BUFSIZE, 0, "hops");
      Send(client.sock, ACK, strlen(ACK), 0, "hops ack");

      printf("Received '%s' in %d hop(s)\n", message, atoi(hops));

      // Send the message to each peer except the peer we received it from
      for (int i = 0; i < numPeers; i++) {
        if (DEBUG) {
          printf("I = [%d]: %s, %s\n", i, peers[i].IP, client.IP);
          printf("I = [%d]: %s, %s\n", i, peers[i].port, client.port);
        }

        if (strcmp(peers[i].IP, client.IP) == 0 && strcmp(peers[i].port, client.port) == 0) {
          continue;
        }

        if (DEBUG)
          printf("propagating message to peer %s:%s\n", peers[i].IP, peers[i].port);

        int peerSock = SetupTCPClientSocket(peers[i].IP, peers[i].port, this_portAsStr);

        SendIsJoining(peerSock, false);
        SendHasMessage(peerSock, true);

        PropagateMessage(client.sock, peerSock, message, numBytesRcvd, atoi(hops));
      }
    }

    close(client.sock);
  }
  // NOT REACHED
}


bool RecvIsJoining(const int clientSock) {
  char _isJoining[BUFSIZE];

  Recv(clientSock, _isJoining, BUFSIZE, 0, "_isJoining");
  Send(clientSock, ACK, strlen(ACK), 0, "_isJoining ack");

  return strcmp(_isJoining, TRUE_STR) == 0;
}

void SendIsJoining(const int serverSock, const bool isJoining) {
	Send(serverSock, isJoining ? TRUE_STR : FALSE_STR, strlen(FALSE_STR), 0, "_isJoining");
	Recv(serverSock, ack, BUFSIZE, 0, "_isJoining ack");
}


bool RecvHasMessage(const int clientSock) {
  char _hasMessage[BUFSIZE];

  Recv(clientSock, _hasMessage, BUFSIZE, 0, "_hasMessage");
  Send(clientSock, ACK, strlen(ACK), 0, "_hasMessage ack");

  return strcmp(_hasMessage, TRUE_STR) == 0;
}

void SendHasMessage(const int serverSock, const bool hasMessage) {
	Send(serverSock, hasMessage ? TRUE_STR : FALSE_STR, strlen(FALSE_STR), 0, "_hasMessage");
	Recv(serverSock, ack, BUFSIZE, 0, "_hasMessage ack");
}


void SendNumPeers(const int clientSock) {
  char serverNumPeers[MAXSTRINGLENGTH];
  sprintf(serverNumPeers, "%d", numPeers);

  Send(clientSock, serverNumPeers, MAXSTRINGLENGTH, 0, "serverNumPeers");
  Recv(clientSock, ack, BUFSIZE, 0, "serverNumPeers ack");
}

int RecvNumPeers(const int serverSock) {
  char serverNumPeers[BUFSIZE];

  Recv(serverSock, serverNumPeers, BUFSIZE, 0, "serverNumPeers");
  Send(serverSock, ACK, strlen(ACK), 0, "serverNumPeers ack");

  return atoi(serverNumPeers);
}


void SendNextPeer(const int clientSock) {
  // Send(clientSock, peers[numPeers - 1].IP, strlen(peers[numPeers - 1].IP), 0, "nextPeerIP");
  Send(clientSock, peers[0].IP, strlen(peers[0].IP), 0, "nextPeerIP");
  Recv(clientSock, ack, BUFSIZE, 0, "nextPeerIP ack");

  // Send(clientSock, peers[numPeers - 1].port, strlen(peers[numPeers - 1].port), 0, "nextPeerPort");
  Send(clientSock, peers[0].port, strlen(peers[0].port), 0, "nextPeerPort");
  Recv(clientSock, ack, BUFSIZE, 0, "nextPeerPort ack");
}

struct peerInfo RecvNextPeer(const int serverSock) {
  char nextPeerIP[BUFSIZE];
  char nextPeerPort[BUFSIZE];

  Recv(serverSock, nextPeerIP, BUFSIZE, 0, "nextPeerIP");
  Send(serverSock, ACK, strlen(ACK), 0, "nextPeerIP ack");

  Recv(serverSock, nextPeerPort, BUFSIZE, 0, "nextPeerPort");
  Send(serverSock, ACK, strlen(ACK), 0, "nextPeerPort ack");

  struct peerInfo nextPeer;
  strcpy(nextPeer.IP, nextPeerIP);
  strcpy(nextPeer.port, nextPeerPort);

  return nextPeer;
}


void SendMessage(const int serverSock, const char *message) {
  Send(serverSock, message, strlen(message), 0, "message");
  Recv(serverSock, ack, BUFSIZE, 0, "message ack");

  Send(serverSock, "1", 1, 0, "hops");
  Recv(serverSock, ack, BUFSIZE, 0, "hops ack");
}


bool CheckIsPeer(struct peerInfo *client) {
  for (int i = 0; i < numPeers; i++) {
    if (DEBUG) {
      printf("I = [%d]: %s, %s\n", i, peers[i].IP, client->IP);
      printf("I = [%d]: %s, %s\n", i, peers[i].port, client->port);
    }

    if (strcmp(peers[i].IP, client->IP) == 0 && strcmp(peers[i].port, client->port) == 0) {
      if (DEBUG)
        puts("is a peer");
      return true;
    }
  }

  return false;
}