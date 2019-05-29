#include <iostream>
#include <middleware.hpp>
#include <sqlite3.h>

void addAluno(std::string nome, std::string passwd){
  sqlite3 *db;

  std::string SQL_QUERY = "INSERT INTO Users VALUES ('"+nome+"', '"+passwd+"');";

  if(sqlite3_open("login.db", &db)){
    std::cout << "Error opening DB" << std::endl;
    return;
  }

  if(sqlite3_exec(db, SQL_QUERY.c_str(), NULL, 0, NULL) != SQLITE_OK)
    std::cout << "Error while adding" << std::endl;

  sqlite3_close(db);


  return;

}

int main(){
  int opt;
  Middleware m;
  std::string nome, passwd, type, Nome;

  do{
    std::cout << "Indique a opção" << std::endl;
    std::cout << "1 -> Adicionar Aluno" << std::endl << "2 -> Atribuir Nota" << std::endl << "0 -> Sair" << std::endl;
    std::cout << "Opção: ";
    std::cin >> opt;
    switch(opt){
      case 0: break;
      case 1:
        std::cout << "ID do aluno: ";
        std::cin >> nome;
        std::cout << "Password do aluno: ";
        std::cin >> passwd;
        std::cout << "Nome: ";
        std::cin >> Nome;
        addAluno(nome, passwd);

        std::cout << "Tipo: ";
        std::cin >> type;

        if(type == "licenciatura")
          m.getKnownAdresses(1);
        else if(type == "mestrado")
            m.getKnownAdresses(2);
        else if(type == "doutoramento")
          m.getKnownAdresses(3);


        for(auto& s : m.knownAdresses){
          int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
          std::vector <std::string> tokens;
          // stringstream class check1
          std::stringstream check1(s);
          std::string intermediate;
          // Tokenizing w.r.t. space ' '
          while(getline(check1, intermediate, ':')) tokens.push_back(intermediate);

          struct sockaddr_in serverinfo;

          memset(&serverinfo, 0, sizeof(serverinfo));

          serverinfo.sin_family = AF_INET;
          serverinfo.sin_addr.s_addr = inet_addr(tokens[0].c_str());
          serverinfo.sin_port = htons(std::stoi(tokens[1]));

          if(connect(server, (struct sockaddr*)&serverinfo, sizeof(serverinfo)) < 0) continue;

          Middleware::writeToSocket(server, "0");
          sleep(1);

          Middleware::writeToSocket(server, "{\"type\":\""+type+"\",\"user\":"+nome+",\"name\":\""+Nome+"\"}");

          close(server);
        }
      break;
      case 2:
        std::cout << "Indique o código da disciplina: ";
        std::cin >> nome;
        std::cout << "Indique o código do aluno: ";
        std::cin >> passwd;
        std::cout << "Nota: ";
        std::cin >> Nome;
        std::cout << "Tipo: " << std::endl;
        std::cin >> type;

        if(type == "licenciatura")
          m.getKnownAdresses(1);
        else if(type == "mestrado")
            m.getKnownAdresses(2);
        else if(type == "doutoramento")
          m.getKnownAdresses(3);

          for(auto& s : m.knownAdresses){
            int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            std::vector <std::string> tokens;
            // stringstream class check1
            std::stringstream check1(s);
            std::string intermediate;
            // Tokenizing w.r.t. space ' '
            while(getline(check1, intermediate, ':')) tokens.push_back(intermediate);

            struct sockaddr_in serverinfo;

            memset(&serverinfo, 0, sizeof(serverinfo));

            serverinfo.sin_family = AF_INET;
            serverinfo.sin_addr.s_addr = inet_addr(tokens[0].c_str());
            serverinfo.sin_port = htons(std::stoi(tokens[1]));

            if(connect(server, (struct sockaddr*)&serverinfo, sizeof(serverinfo)) < 0) continue;

            Middleware::writeToSocket(server, "2");
            sleep(1);

            Middleware::writeToSocket(server, "{\"type\":\""+type+"\",\"disc\":"+nome+",\"aluno\":"+passwd+",\"nota\":"+Nome+"}");

            close(server);
          }


      break;
      default:
        std::cout << "Opção inválida"<< std::endl;
        for(int i = 0; i < 100; i++)
          std::cout << std::endl;
    }
  } while(opt != 0);
}
