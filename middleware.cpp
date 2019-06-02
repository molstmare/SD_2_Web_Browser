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
    buffer[bufferSize-1] = '\0';
    bytesRecv = 0;
    bytesRecv = recv(client, &buffer[bufferSize-1], 1, 0);
    ioctl(client, FIONREAD, &nmrRead);
  }while(nmrRead > 0);

  std::string cipher = decrypt(std::string(buffer));

  // Now we have the information needed
  return cipher;
}

void Middleware::writeToSocket(int client, std::string request){
  std::string cipher = encrypt(request);
  send(client, cipher.c_str(), cipher.size(), 0);
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

std::map<std::string, double> Middleware::askInfo(std::string type, std::string nome){
  std::string IP;
  std::string Porta;
  std::string s;

  int server_s;

  std::map<std::string, double> gotInfo;


    for (unsigned i=0; i< knownAdresses.size(); i++){
      server_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      s = knownAdresses.at(i);
      //std::cout << s << std::endl;
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

      server.sin_family = AF_INET;
      server.sin_port = htons(std::stoi(Porta));
      server.sin_addr.s_addr = inet_addr(IP.c_str());

      if(connect(server_s, (struct sockaddr*)&server, sizeof(server)) < 0){
         std::cout << "Oops" << std::endl;
         close(server_s);
         continue;
      }

      Middleware::writeToSocket(server_s, "1");
      sleep(1);

      Middleware::writeToSocket(server_s, "{\"type\":\""+type+"\",\"name\":\""+nome+"\"}");
      int quant = std::stoi(Middleware::readFromSocket(server_s));

      for(int i = 0; i < quant; i++){
        std::string s = Middleware::readFromSocket(server_s);
        double d = std::stod( Middleware::readFromSocket(server_s) );
        gotInfo.insert(std::pair<std::string, double> (s, d));
      }

      close(server_s);
      break;
    }

    return gotInfo;
}

std::string Middleware::encrypt(std::string plaintext){
  /*std::string encrypted_message{};
  for (char var: plaintext){
      size_t location = alphabet.find(var);
        if (location!= std::string::npos){
            char new_character = key[location];
              encrypted_message += new_character;
           }
        else
             encrypted_message += var;

        }
  return encrypted_message;*/

  std::string password = "0123456789abcdefghijklmnopqrstuvwxyz";
  std::string iv = "amcndoejie8397r10h0c1o3xj8103u1r90ud";
  SecByteBlock key(AES::MAX_KEYLENGTH+AES::BLOCKSIZE);
  std::string ciphertext;

  try{

    HKDF<SHA256> hkdf;
    hkdf.DeriveKey(key, key.size(), (const byte*)password.data(), password.size(), (const byte*)iv.data(), iv.size(), NULL, 0);

    CTR_Mode<AES>::Encryption enc;
    enc.SetKeyWithIV(key, AES::MAX_KEYLENGTH, key+AES::MAX_KEYLENGTH);

    StringSource encryptor(plaintext, true, new StreamTransformationFilter(enc, new StringSink(ciphertext)));

  }catch(const Exception& e){
    std::cout << "nothing works" << std::endl;
  }
  return ciphertext;
}

std::string Middleware::decrypt(std::string ciphertext){
  /*std::string decrypted_message{};

  for (char dec: ciphertext){
      size_t dec_location = key.find(dec);
        if (dec_location != std::string::npos){
            char final_char = alphabet[dec_location];
            decrypted_message+=final_char;
        }
        else decrypted_message+=dec;

      }
      return decrypted_message;*/

      std::string password = "0123456789abcdefghijklmnopqrstuvwxyz";
      std::string iv = "amcndoejie8397r10h0c1o3xj8103u1r90ud";
      SecByteBlock key(AES::MAX_KEYLENGTH+AES::BLOCKSIZE);
      std::string plaintext;

      try{

        HKDF<SHA256> hkdf;
        hkdf.DeriveKey(key, key.size(), (const byte*)password.data(), password.size(), (const byte*)iv.data(), iv.size(), NULL, 0);


        CTR_Mode<AES>::Encryption dec;
        dec.SetKeyWithIV(key, AES::MAX_KEYLENGTH, key+AES::MAX_KEYLENGTH);

        StringSource decryptor(ciphertext, true, new StreamTransformationFilter(dec, new StringSink(plaintext)));

      }catch(const Exception& e){
        std::cout << "nothing works" << std::endl;
      }
      return plaintext;

}
