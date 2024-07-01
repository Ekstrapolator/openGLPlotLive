//
// Created by wolek on 29.06.24.
//
// based on https://beej.us/guide/bgnet/html/split/index.html

// Standard Includes
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <netdb.h>

// Project Includes
#include "udpServer.hpp"
#include "msg.hpp"
#include "qformat.hpp"

namespace ut = utilities;

namespace udp {
UdpServer::UdpServer(uint16_t udpPort) {

  port = std::to_string(udpPort);

  struct addrinfo hints = {}, *servinfo, *p;
  int status;

  struct sockaddr_storage their_addr;

  socklen_t addr_len;
  char s[INET6_ADDRSTRLEN];

  hints.ai_family = AF_INET; //IPv4
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  //TODO: some way to handle errors
  if ((status = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    //return 1;
  }

  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((socketFd = socket(p->ai_family, p->ai_socktype,
                           p->ai_protocol)) == -1) {
      perror("listener: socket");
      continue;
    }

    if (bind(socketFd, p->ai_addr, p->ai_addrlen) == -1) {
      close(socketFd);
      perror("listener: bind");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "listener: failed to bind socket\n");
  }

  freeaddrinfo(servinfo);

}
void UdpServer::reciveData(uint8_t *buff, size_t buffLeght) {

  printf("listener: waiting to recvfrom...\n");
  struct sockaddr_storage clientAddr;
  socklen_t addrLen = sizeof(clientAddr);
  size_t numBytes;
  if ((numBytes = recvfrom(socketFd, buff, buffLeght, 0,
                           (struct sockaddr *)&clientAddr, &addrLen)) == 0) {
    perror("recvfrom");
    exit(1);
  }
  char clientAddrText[INET_ADDRSTRLEN];
  printf("listener: got packet from %s\n",
         inet_ntop(clientAddr.ss_family, &((struct sockaddr_in *)&clientAddr)->sin_addr,
                   clientAddrText, INET_ADDRSTRLEN));
  printf("listener: packet is %zd bytes long\n", numBytes);

  /* Print 60 bytes to check what have been sent */
  for (size_t i{0}; i < 12; i++)
  {
    printf("%x\t", *(buff + i));
  }
  printf("\n");

  auto* log = reinterpret_cast<LogPacket*>(buff);
  if(log->msgHeader.msgPattern == 0xDEADBEEF)
  {
    printf("Correct message pattern detected\n");
  }
  else if(log->msgHeader.msgId == MsgId::mImuData)
  {
    printf("Correct message id detected, data length: %u\n", log->msgHeader.dataLength);
  }
  auto* data = reinterpret_cast<int32_t*>(&log->msgPayload.data[0]);
  for(size_t i{0}; i < 10; i++)
  {
    float fdata = ut::q16_16ToFloat(*data);
    data ++;
    printf("Recived pitch angle in deg: %f", fdata);
  }

}

UdpServer::~UdpServer() {
  close(socketFd);
}
} // udp