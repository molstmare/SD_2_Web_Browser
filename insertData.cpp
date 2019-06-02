#include <iostream>
#include <middleware.hpp>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

void addAluno(std::string nome, std::string passwd, std::string type){
  sqlite3 *db;

  std::string SQL_QUERY = "INSERT INTO Users VALUES ('"+nome+"', '"+passwd+"', '"+type+"');";

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
  //std::string nome, passwd, type, Nome;

  std::string JSON;
  std::getline(std::cin, JSON);
  while(JSON[JSON.size()-1] != '}')
    JSON = JSON.substr(0, JSON.size()-1);

  //std::cout << JSON << std::endl;

  JSON += "\n";
  JSON.erase(std::remove(JSON.begin(), JSON.end(), '\\'), JSON.end());

  nlohmann::json json_parsed = nlohmann::json::parse(JSON.c_str());

  std::string type = json_parsed["type"].dump();
  type.erase(std::remove(type.begin(), type.end(), '\"'), type.end());

  std::string user = json_parsed["user"].dump();
  user.erase(std::remove(user.begin(), user.end(), '\"'), user.end());

  std::string password = json_parsed["passwd"].dump();
  password.erase(std::remove(password.begin(), password.end(), '\"'), password.end());

  std::string name = json_parsed["name"].dump();
  name.erase(std::remove(name.begin(), name.end(), '\"'), name.end());

  if(type == "licenciatura"){
    addAluno(user, password, "L");
    m.getKnownAdresses(1);
  } else if(type == "mestrado"){
    addAluno(user, password, "M");
    m.getKnownAdresses(2);
  } else if(type == "doutoramento"){
    addAluno(user, password, "D");
    m.getKnownAdresses(3);
  }

  for(auto &s : m.knownAdresses){
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

    Middleware::writeToSocket(server, "{\"type\":\""+type+"\",\"user\":"+user+",\"name\":\""+name+"\"}");

    std::cout<<"Success";

    close(server);
  }


  /*do{
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
        std::cout << "Tipo: ";
        std::cin >> type;

        if(type == "licenciatura"){
            addAluno(nome, passwd, "L");
            m.getKnownAdresses(1);
        }
        else if(type == "mestrado"){
          addAluno(nome, passwd, "M");
          m.getKnownAdresses(2);
        }
        else if(type == "doutoramento")
        {
           addAluno(nome, passwd, "M");
            m.getKnownAdresses(3);
        }


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
  } while(opt != 0);*/
}
