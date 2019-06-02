#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/ioctl.h>

int main ( int argc, char ** argv) {
  std::string JSON;
  std::getline(std::cin, JSON);

    struct sockaddr_in server;
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    char inteiro;

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Before connecting first we encode for erlang support
    int index = 0;

    if(connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) printf("Error");

    send(s, JSON.c_str() , JSON.size(), 0);

    int bufferSize = 0;
    char* buffer = NULL; // allocate a char

    int bytesRecv = 0, nmrRead = 0;

    do{
      buffer = (char*) realloc(buffer, ++bufferSize);
      buffer[bufferSize-1] = '\0';
      bytesRecv = 0;
      bytesRecv = recv(s, &buffer[bufferSize-1], 1, 0);
      ioctl(s, FIONREAD, &nmrRead);
    }while(nmrRead > 0 || buffer[bufferSize-1] == '}');

    close(s);

    std::cout << std::string(buffer);

}
