#include <middleware.hpp>
#include <thread>
#include <fstream>
#include <ctime>
#include <sqlite3.h>
#include "nlohmann/json.hpp"

static int callback(void *data, int argc, char **argv, char **azColName){
  int *c = (int*)data;
  *c = atoi(argv[0]);
  return 0;
}

std::string testDB(nlohmann::json j){

   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
  struct sqlite3_stmt *selectstmt;

   rc = sqlite3_open("login.db", &db);
   if(rc){
     std::cout << "couldn't retrive information" << std::endl;
     return std::to_string(1);
   }

   //std::cout << j["name"].dump() << j["password"].dump() << std::endl;

    std::string mysql_query = "SELECT Type FROM Users WHERE Username = "+ j["name"].dump() +" AND Password = "+ j["password"].dump() +";";


    if(sqlite3_prepare_v2(db, mysql_query.c_str(), -1, &selectstmt, 0) != SQLITE_OK)
      return std::to_string(1);

    if(sqlite3_step(selectstmt) == SQLITE_ROW){
      std::string str = std::string( reinterpret_cast<const char*>(sqlite3_column_text(selectstmt, 0)) );
      return str;
    }

   return std::to_string(1);
}

void entryLog(std::string info){
  std::ofstream logFile("middleware.log");
  std::time_t t = std::time(0);
  char cstr[128];
  std::strftime( cstr, sizeof(cstr), "%I:%M:%S %p %Z", std::localtime(&t) );

  logFile << std::string(cstr) + "-" + info + " " + std::string("\n");
}

void handleClient(struct ClientInfo c){
  std::string type;
  std::string s = Middleware::readFromSocket(c.clientSocket);
  //std::cout << s << std::endl;
  while(s[s.size()-1] != '}')
    s = s.substr(0, s.size()-1);

    s += "\n";
    s.erase(std::remove(s.begin(), s.end(), '\\'), s.end());

    nlohmann::json j = nlohmann::json::parse(s.c_str());

  std::string res = testDB(j);
  //Middleware::writeToSocket(c.clientSocket, res);

  if(res == "1"){
    Middleware::writeToSocket(c.clientSocket, std::to_string(0));
    return;
  }

  if(res == "L")
  type = "licenciatura";
  else if(res == "M")
  type = "mestrado";
  else if(res == "D")
  type = "doutoramento";

  Middleware m;

  std::string name = j["name"].dump();
  name.erase(std::remove(name.begin(), name.end(), '\"'), name.end());

  m.getKnownAdresses(1);
  std::map<std::string, double> info = m.askInfo(type, name);

  Middleware::writeToSocket(c.clientSocket, std::to_string(info.size()));
  sleep(2);

  for(auto& m: info){
    Middleware::writeToSocket(c.clientSocket, m.first);
    sleep(2);
    Middleware::writeToSocket(c.clientSocket, std::to_string(m.second));
    sleep(1);
  }

}

int main(){
  Middleware *m = new Middleware(12345);

  m->startLocalServer();
  while(1){
      struct ClientInfo c = m->waitClient();
      std::cout << "client loggedin" << std::endl;
      std::thread(&handleClient, c).detach();
  }
}
