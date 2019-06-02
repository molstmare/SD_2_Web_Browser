#include <middleware.hpp>

Middleware::Middleware(){
  this->port = -1;
  memset(&server, 0, sizeof(server));

  // Start server socket for future usage
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

Middleware::Middleware(int port){
  this->port = port;
  memset(&server, 0, sizeof(server));

  // Start server socket for future usage
  server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

void Middleware::setPort(int port){
  this->port = port;
}

void Middleware::startGlobalServer(){
  if (this->port == -1) throw "Port it not set. Please set a port to listen to";

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(this->port);

  if(bind(server_socket, (struct sockaddr*)&server, sizeof(server)) < 0)
    throw("Error while binding socket");

  if(listen(server_socket, 255) < 0)
    throw("Could not liste on port");
}

/**
* This function will start to listen on localhost (127.0.0.1)
* It accepts an integer that is the port where it will listen to.
*/
void Middleware::startLocalServer(){
  if (this->port == -1) throw "Port it not set. Please set a port to listen to";

  // Fill the structure to contain the information needed
  // This is a TCP socket
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Listen on localhost
  server.sin_port = htons(this->port);

  // now we must bind the previously created socket to the corresponding structure
  if(bind(server_socket, (struct sockaddr*)&server, sizeof(server)) < 0)
    throw "Error bindig to server socket";

  // Now listen to the socket:
  if(listen(server_socket, 255) < 0) // we will accept a maximum of 255 clients at the same time
    throw "Could not listen on port";
}

struct ClientInfo Middleware::waitClient(){
  struct sockaddr_in clientStruct;
  socklen_t clientSize;
  int client = accept(this->server_socket, (struct sockaddr*)&clientStruct, &clientSize);

  struct ClientInfo c;
  c.clientInfo = clientStruct;
  c.size = clientSize;
  c.clientSocket = client;

  return c;
}

// Save memory, this function accepts a reference to a position
std::string Middleware::readFromSocket(int& client){
  int bufferSize = 0;
  char* buffer = NULL; // allocate a char

  int bytesRecv = 0, nmrRead = 0;

  do{
    buffer = (char*) realloc(buffer, ++bufferSize);
    bytesRecv = 0;
    bytesRecv = recv(client, &buffer[bufferSize-1], 1, 0);
    ioctl(client, FIONREAD, &nmrRead);
  }while(nmrRead > 0);

  // Now we have the information needed
  return std::string(buffer);
}

void Middleware::writeToSocket(int client, std::string request){
  send(client, request.c_str(), request.size(), 0);
}

void Middleware::getKnownAdresses(int x){

  if(x == 1){
    std::ifstream myReadFile("licenciatura.txt");
    std::string IP;
    if (myReadFile.is_open()) {
      while (!myReadFile.eof()){
        myReadFile >> IP;
        //std::cout << IP << std::endl;
        knownAdresses.push_back(IP);
        }
    }
    myReadFile.close();

  }
  else if(x == 2){
    std::ifstream myReadFile("mestrado.txt");
    std::string IP;
    if (myReadFile.is_open()) {
      while (!myReadFile.eof()){
        myReadFile >> IP;

        knownAdresses.push_back(IP);   
      }
    }
    myReadFile.close();
  }
  else if(x == 3){
    std::ifstream myReadFile("doutoramento.txt");
    std::string IP;
    if (myReadFile.is_open()) {
      while (!myReadFile.eof()){
        myReadFile >> IP;

        knownAdresses.push_back(IP);
      }
    }
    myReadFile.close();
  }
  else
  {
    std::cout << "ERRO, tipo não definido";
  }
}

void Middleware::askInfo(){
  std::string IP;
  std::string Porta;
  std::string s;
  
  int server_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    for (unsigned i=0; i< knownAdresses.size(); i++){
  
      s = knownAdresses.at(i);
      std::cout << s << std::endl;
      std::string IP;
      std::string Porta;

      // Vector of string to save tokens 
      std::vector <std::string> tokens; 
        
      // stringstream class check1 
      std::stringstream check1(s); 
        
      std::string intermediate; 
        
      // Tokenizing w.r.t. space ' ' 
      while(getline(check1, intermediate, ':')) 
      { 
          tokens.push_back(intermediate); 
      }  

      IP = tokens[0];
      Porta = tokens[1];

      //connect to server using TCP
      struct sockaddr_in server;

      memset(&server, 0, sizeof(server));

      std::cout<< IP << std::endl;
      std::cout << Porta << std::endl;

      server.sin_family = AF_INET;
      server.sin_port = htons(std::stoi(Porta));
      server.sin_addr.s_addr = inet_addr(IP.c_str());

      if(connect(server_s, (struct sockaddr*)&server, sizeof(server)) < 0){
         //printf("Error");
         continue;
      }

      Middleware::writeToSocket(server_s, "Hello World");

      Middleware::readFromSocket(server_s);

      close(server_s);  
      break; 
    }
}