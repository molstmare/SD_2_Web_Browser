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

std::string testDB(std::string JSON){

  JSON.erase(std::remove(JSON.begin(), JSON.end(), '\\'), JSON.end());

  nlohmann::json j = nlohmann::json::parse(JSON.c_str());

   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
    struct sqlite3_stmt *selectstmt;

   rc = sqlite3_open("login.db", &db);
   if(rc){
     std::cout << "couldn't retrive information" << std::endl;
     return std::to_string(1);
   }

    std::string mysql_query = "SELECT count(*) FROM Users WHERE Username = "+ j["name"].dump() +" AND  Password = "+ j["password"].dump() +";";

    int count = -1;

    rc = sqlite3_exec(db, mysql_query.c_str(), callback, &count, NULL);
    if(rc != SQLITE_OK){
      std::cout << "Error while executing things" <<std::endl;
      return std::to_string(1);
    } else {
      if(count > 0) return std::to_string(0);
    }

   sqlite3_close(db);
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
  std::string s = Middleware::readFromSocket(c.clientSocket);
  while(s[s.size()-1] != '}')
    s = s.substr(0, s.size()-1);

  std::string res = testDB(s+"\n");
  Middleware::writeToSocket(c.clientSocket, res);

}

int main(){
  Middleware *m = new Middleware(12345);

  m->startLocalServer();
  while(1){
      struct ClientInfo c = m->waitClient();
      entryLog(std::string(inet_ntoa(c.clientInfo.sin_addr)));
      std::thread(&handleClient, c).detach();
  }
}
