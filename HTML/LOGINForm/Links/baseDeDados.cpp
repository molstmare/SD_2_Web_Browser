#include <middleware.hpp>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

int main(){
    Middleware* m = new Middleware();

    std::cout << "Getting known addresses" << std::endl;
    m->getKnownAdresses(1);
    std::cout << "Asking for information" << std::endl;
    m->askInfo();

    m->startGlobalServer();

    while(1){
        m->waitClient();
    }

}