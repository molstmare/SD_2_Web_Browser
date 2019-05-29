#include <middleware.hpp>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>

static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

std::string base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = ( char_array_4[0] << 2       ) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = 0; j < i; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

int Middleware::decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}


int Middleware::encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

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
  /* A 256 bit key */
  unsigned char *key = (unsigned char *)"01234567890123456789012345678901";

  /* A 128 bit IV */
  unsigned char *iv = (unsigned char *)"0123456789012345";

  int bufferSize = 0;
  char* buffer = NULL; // allocate a char
  char plaintext[128];

  int bytesRecv = 0, nmrRead = 1;
  sleep(2);

  do{
    bufferSize += nmrRead;
    buffer = (char*) realloc(buffer, bufferSize);
    buffer[bufferSize-nmrRead] = '\0';
    bytesRecv = 0;
    bytesRecv = recv(client, &buffer[bufferSize-nmrRead], nmrRead, 0);
    ioctl(client, FIONREAD, &nmrRead);
  }while(nmrRead > 0);

  std::cout << bufferSize << std::endl;


  std::string to_decrypt = base64_decode(std::string(buffer));

  std::cout << to_decrypt << std::endl;

  int size = decrypt((unsigned char*)to_decrypt.c_str(), to_decrypt.size(), key, iv, (unsigned char*)plaintext);
  //int size = decrypt((unsigned char*)buffer, bufferSize, key, iv, (unsigned char*)plaintext);
  

  std::cout << std::string(plaintext, size) << std::endl;


  // Now we have the information needed
  return std::string(plaintext, size);
}

void Middleware::writeToSocket(int client, std::string request){ 
  unsigned char ciphertext[128] ;
  /* A 256 bit key */
  unsigned char *key = (unsigned char *)"01234567890123456789012345678901";

  /* A 128 bit IV */
  unsigned char *iv = (unsigned char *)"0123456789012345";
  
  int size = encrypt((unsigned char*)request.c_str(), request.size(), key, iv, ciphertext);

  std::string b64 = base64_encode(ciphertext, size);

  send(client, b64.c_str(), b64.size(), 0);

  sleep(1);
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
