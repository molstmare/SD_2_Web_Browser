#pragma once
#ifndef __MIDDLEWARE__
#define __MIDDLEWARE__
#endif

#include <iostream>

#ifndef __APPLE__
#include <string.h>
#endif

#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <vector>
#include <map>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <stdio.h>

struct ClientInfo {
  struct sockaddr_in clientInfo;
  socklen_t size;
  int clientSocket;
};

/**
* This class is for middleware only
*/
class Middleware{

  // we must know on what port we are listening to
  int port;
  // we first have to define our server structures
  struct sockaddr_in server;

  // we have our sockets. Max 100 clients permitted.
  int server_socket;

  static int decrypt(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);
  static int encrypt(unsigned char*, int, unsigned char*, unsigned char*, unsigned char*);

public:

  // addresses like 127.0.0.1:6061
  std::vector<std::string> knownAdresses;
  
  Middleware();
  Middleware(int);

  void startGlobalServer();
  void startLocalServer();
  static std::string readFromSocket(int&);
  static void writeToSocket(int, std::string);
  void getKnownAdresses(int);
  std::map<std::string, double> askInfo(std::string, std::string);
  void setPort(int);
  struct ClientInfo waitClient();


};
