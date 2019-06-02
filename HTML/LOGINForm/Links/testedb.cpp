#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "nlohmann/json.hpp"
#include <algorithm>

int main(int argc, char* argv[]) {

  std::string JSON;
  std::getline(std::cin, JSON);

  JSON.erase(std::remove(JSON.begin(), JSON.end(), '\\'), JSON.end());

  nlohmann::json j = nlohmann::json::parse(JSON.c_str());

   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
    struct sqlite3_stmt *selectstmt;

   rc = sqlite3_open("login.db", &db);

    std::string mysql_query = "SELECT * FROM Users WHERE Username = '"+ j["name"].dump() +"' AND  Password = '"+ j["password"].dump() +"';";

    int result = sqlite3_prepare_v2(db, mysql_query.c_str(), -1, &selectstmt, NULL);

    if(result == SQLITE_OK){
        //std::cout << "OK"<< std::endl;
        if(sqlite3_step(selectstmt) == SQLITE_ROW) return 0;
    }

   sqlite3_close(db);
   return 1;
}
