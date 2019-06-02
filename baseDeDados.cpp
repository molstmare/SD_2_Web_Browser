#include <middleware.hpp>
#include <sqlite3.h>
#include <thread>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

void insertIntoDatabase(std::string db, std::string query){
  sqlite3* database;
  if(sqlite3_open(std::string(db+".db").c_str(), &database))
    return;

    sqlite3_exec(database, query.c_str(), NULL, 0, NULL);
    sqlite3_close(database);
    return;
}

std::map<std::string, double> getDatabaseInfo(std::string db_path, std::string query){
  // First we have to open database
  sqlite3* database; // To open database
  sqlite3_stmt *stmt; // To check results

  std::map<std::string, double> info;

  db_path.erase(std::remove(db_path.begin(), db_path.end(), '\"'), db_path.end());

  //std::cout << db_path << std::endl;

  // Open database and link it to sqlite3
  if(sqlite3_open(std::string(db_path + ".db").c_str(), &database) != SQLITE_OK)
    throw "Error opening DB";

  // prepare to run stmt
  if(sqlite3_prepare_v2(database, query.c_str(), -1, &stmt, 0) != SQLITE_OK)
    throw "Error preparing sql stmt";

  // Check if query returned something
  while(sqlite3_step(stmt) == SQLITE_ROW){
    std::string str = std::string( reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)) );

    //insert into map case positive
    info.insert( std::pair<std::string, double>(str , sqlite3_column_double(stmt, 1) ) );
  }

  return info; // return the structure created
}

void handleClient(int client){

  std::string JSON_info, who, SQL_QUERY, athing, student, nota;
  std::map<std::string, double> mapInfo;
  nlohmann::json parsed_json;
  int size;
  std::ifstream file;

  // Check operation
  int operation = std::stoi( Middleware::readFromSocket(client) );

  std::cout << "Operation: " << operation << std::endl;

  switch(operation){
    case 0:
      //Receive JSON info
      JSON_info = Middleware::readFromSocket(client);

      // clear some trash
      while(JSON_info[JSON_info.size()-1] != '}')
        JSON_info = JSON_info.substr(0, JSON_info.size()-1);

      JSON_info += "\n";
      JSON_info.erase(std::remove(JSON_info.begin(), JSON_info.end(), '\\'), JSON_info.end());

      // Now JSON_info is ready to get parsed
      parsed_json = nlohmann::json::parse(JSON_info.c_str());

      who = parsed_json["user"].dump();
      who.erase(std::remove(who.begin(), who.end(), '\"'), who.end());

      SQL_QUERY = "INSERT INTO Aluno VALUES ("+ who +", "+parsed_json["name"].dump()+");";

      athing = parsed_json["type"].dump();
      athing.erase(std::remove(athing.begin(), athing.end(), '\"'), athing.end());


      insertIntoDatabase(athing, SQL_QUERY);
      break;

    case 1:
      // First we have to read from the socket from what informatio we want to take:
      JSON_info = Middleware::readFromSocket(client);

      // First clear some errors that transmission could retrieve:
      while(JSON_info[JSON_info.size()-1] != '}')
        JSON_info = JSON_info.substr(0, JSON_info.size()-1);

      JSON_info += "\n";
      JSON_info.erase(std::remove(JSON_info.begin(), JSON_info.end(), '\\'), JSON_info.end());

      // Now JSON_info is ready to get parsed
      parsed_json = nlohmann::json::parse(JSON_info.c_str());

      who = parsed_json["name"].dump();
      who.erase(std::remove(who.begin(), who.end(), '\"'), who.end());

      // end Information process:
      // Now that we have the information needed, we will build our SQL_QUERY:
      SQL_QUERY = "SELECT d.Nome, da.Nota FROM Disciplina d, DisciplinaAluno da WHERE da.CodAluno="+ who +" AND d.CodDisciplina = da.CodDisciplina;";

      try{
        // Now that query is constructed let sqlite3 process it:
        mapInfo = getDatabaseInfo(std::string(parsed_json["type"].dump()), SQL_QUERY);
      } catch(const char* e){
        std::cout << e << std::endl;
      }

      size = mapInfo.size();

      // Inform the client how many records we are sending:
      Middleware::writeToSocket( client, std::to_string(size) );
      sleep(1);

      for(auto& m : mapInfo){
        Middleware::writeToSocket(client, m.first);
        sleep(1);
        Middleware::writeToSocket(client, std::to_string(m.second));
        sleep(1);
      }
      break;

    case 2:

    //Receive JSON info
    JSON_info = Middleware::readFromSocket(client);

    // clear some trash
    while(JSON_info[JSON_info.size()-1] != '}')
      JSON_info = JSON_info.substr(0, JSON_info.size()-1);

    JSON_info += "\n";
    JSON_info.erase(std::remove(JSON_info.begin(), JSON_info.end(), '\\'), JSON_info.end());

    // Now JSON_info is ready to get parsed
    parsed_json = nlohmann::json::parse(JSON_info.c_str());

    who = parsed_json["disc"].dump();
    who.erase(std::remove(who.begin(), who.end(), '\"'), who.end());

    student = parsed_json["aluno"].dump();
    student.erase(std::remove(student.begin(), student.end(), '\"'), student.end());

    nota = parsed_json["nota"].dump();
    nota.erase(std::remove(nota.begin(), nota.end(), '\"'), nota.end());

    SQL_QUERY = "INSERT INTO DisciplinaAluno VALUES ("+ who +", "+parsed_json["aluno"].dump()+", "+nota+");";

    athing = parsed_json["type"].dump();
    athing.erase(std::remove(athing.begin(), athing.end(), '\"'), athing.end());


    insertIntoDatabase(athing, SQL_QUERY);
    break;

    case 3:
      switch(std::stoi(Middleware::readFromSocket(client))){
        case 1:
          file.open("licenciatura.db");
          while(!file.eof()){
            std::string ask;
            file >> ask;
            Middleware::writeToSocket(client, ask);
          }
        break;
        case 2:
        file.open("mestrado.db");
        while(!file.eof()){
          std::string ask;
          file >> ask;
          Middleware::writeToSocket(client, ask);
        }
        break;
        case 3:
        file.open("doutoramento.db");
        while(!file.eof()){
          std::string ask;
          file >> ask;
          Middleware::writeToSocket(client, ask);
        }
        break;
      }

    break;
    default:
      Middleware::writeToSocket(client, "-1");

  }

  close(client);


}

void askDBInfo(int type1, int type2){
  Middleware type1m, type2m;
  type1m.getKnownAdresses(type1);

  for(auto &t1 : type1m.knownAdresses){
    int server_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Vector of string to save tokens
    std::vector <std::string> token;

    // stringstream class check1
    std::stringstream check1(t1);

    std::string intermediate;

    // Tokenizing w.r.t. space ' '
    while(getline(check1, intermediate, ':'))
        token.push_back(intermediate);

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(std::stoi(token[1]));
    server.sin_addr.s_addr = inet_addr(token[0].c_str());

    if(connect(server_s, (struct sockaddr*)&server, sizeof(server)) < 0){
       std::cout << "Oops" << std::endl;
       close(server_s);
       continue;
    }

    Middleware::writeToSocket(server_s, "3");
    sleep(1);
    Middleware::writeToSocket(server_s, std::to_string(type1));

    std::string file = Middleware::readFromSocket(server_s);

    if(type1 == 1){
      std::ofstream out("licenciatura.db");
      out << file;
      out.close();
    }
    else if(type1 == 2){
      std::ofstream out("mestrado.db");
      out << file;
      out.close();
    }
    else{
      std::ofstream out("doutoramento.db");
      out << file;
      out.close();
    }
    close(server_s);
    break;
  }

  type2m.getKnownAdresses(type2);

  for(auto &t2 : type2m.knownAdresses){
    int server_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Vector of string to save tokens
    std::vector <std::string> token;

    // stringstream class check1
    std::stringstream check1(t2);

    std::string intermediate;

    // Tokenizing w.r.t. space ' '
    while(getline(check1, intermediate, ':'))
        token.push_back(intermediate);

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(std::stoi(token[1]));
    server.sin_addr.s_addr = inet_addr(token[0].c_str());

    if(connect(server_s, (struct sockaddr*)&server, sizeof(server)) < 0){
       std::cout << "Oops" << std::endl;
       close(server_s);
       continue;
    }

    Middleware::writeToSocket(server_s, "3");
    sleep(1);

    Middleware::writeToSocket(server_s, std::to_string(type2));

    std::string file = Middleware::readFromSocket(server_s);

    if(type1 == 1){
      std::ofstream out("licenciatura.db");
      out << file;
      out.close();
    }
    else if(type1 == 2){
      std::ofstream out("mestrado.db");
      out << file;
      out.close();
    }
    else{
      std::ofstream out("doutoramento.db");
      out << file;
      out.close();
    }
    close(server_s);
    break;
  }
}

int main(int argc, char** argv){
    if(argc > 3){
      int type1 = std::stoi(std::string(argv[1]));
      int type2 = std::stoi(std::string(argv[2]));
      askDBInfo(type1, type2);
    }

    Middleware* m = new Middleware(12346);

    m->startGlobalServer();

    while(1){
        struct ClientInfo clientInfo = m->waitClient();
        std::cout << "client loggedin" << std::endl;
        // Log server
        std::thread(handleClient, clientInfo.clientSocket).detach();

    }

}
