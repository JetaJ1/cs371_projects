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

static const int MAXPENDING = 5; // Maximum outstanding connection requests
// static const int MAXPEERS = 2; // Maximum number of peers

int main(int argc, char *argv[]) {
  printf("starting..");
  bool isJoining = false;
  bool hasMessage = false; // If true, send message (prompt input) to peer
  char *peerPort;
  char *peerIP;
  char *msgString;

  // ---- Handle command line arguments ----
  if (argc < 2 || argc > 8) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)", "<Port> [-i <Peer IP> -p <Peer Port>] [-s <Message>]");

  in_port_t servPort = atoi(argv[1]); // First arg:  local port

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
        msgString = argv[7];
        break;
      default:
        DieWithUserMessage("Parameter(s)", "<Port> [-i <Peer IP> -p <Peer Port>] [-s <Message>]");
        break;
    }
  }

  // Create socket for outgoing connection (as client to a remote peer)
  if (isJoining) {
    printf("isJoining\n");
    int sock = SetupTCPClientSocket(peerIP, peerPort);

    if (hasMessage) {
      size_t msgStringLen = strlen(msgString); // Determine input length

      // Send the string to the server
      ssize_t numBytes = send(sock, msgString, msgStringLen, 0);
      // ssize_t numBytes = send(sock, MsgInfo, sizeof(MsgInfo), 0);

      if (numBytes < 0)
        DieWithSystemMessage("send() failed");
      else if (numBytes != msgStringLen)
        DieWithUserMessage("send()", "sent unexpected number of bytes");

      // Receive the same string back from the server
      unsigned int totalBytesRcvd = 0; // Count of total bytes received
      fputs("Received: ", stdout);     // Setup to print the echoed string
      while (totalBytesRcvd < msgStringLen) {
        char buffer[BUFSIZE]; // I/O buffer
        // Receive up to the buffer size (minus 1 to leave space for a null terminator) bytes from the sender
        numBytes = recv(sock, buffer, BUFSIZE - 1, 0);
        if (numBytes < 0)
          DieWithSystemMessage("recv() failed");
        else if (numBytes == 0)
          DieWithUserMessage("recv()", "connection closed prematurely");
        totalBytesRcvd += numBytes; // Keep tally of total bytes
        buffer[numBytes] = '\0';    // Terminate the string!
        fputs(buffer, stdout);      // Print the buffer
      }

      fputc('\n', stdout); // Print a final linefeed
    }

    close(sock);
  }

  printf("Create sockert for incoming connections..");
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

  while (1) { // Run forever
    struct sockaddr_in clntAddr; // Client address
    // Set length of client address structure (in-out parameter)
    socklen_t clntAddrLen = sizeof(clntAddr);

    // Wait for a client to connect
    int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
    if (clntSock < 0)
      DieWithSystemMessage("accept() failed");

    // clntSock is connected to a client!
    printf("This peer is established at 127.0.0.1:%d\n", servAddr.sin_port);

    char clntName[INET_ADDRSTRLEN]; // String to contain client address
    if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
      printf("Handling client %s:%d\n", clntName, ntohs(clntAddr.sin_port));
    else
      puts("Unable to get client address");

    HandleTCPClient(clntSock);
    // printf("This server has been contacted %d time(s).\n", visits);
  }
  // NOT REACHED

}
