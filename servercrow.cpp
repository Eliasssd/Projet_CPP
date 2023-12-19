#include <iostream>
#include "crow.h"
#include "/usr/include/sqlite3.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "jsoncpp/json/json.h"

static int callback(void *NotUsed, int argc, char **argv, char **szColName)
{
  for(int i = 0; i < argc; i++)
  {
    std::cout << szColName[i] << " = " << argv[i] << std::endl;
  }

  std::cout << "\n";

  return 0;

}

std::string hexToStr(const std::string& hex) {
    std::string str;
    str.reserve(hex.length() / 2);

    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = static_cast<char>(std::stoul(byteString, nullptr, 16));
        str.push_back(byte);
    }

    return str;
}

int main() {

    crow::SimpleApp app;

    const int port=18080;

    sqlite3 *db;
    char *szErrMsg = 0;

    // open database
    int rc = sqlite3_open("Sqlite_Test.db", &db);

    if(rc)
    {
        std::cout << "Can't open database\n";
    } else {
        std::cout << "Open database successfully\n";
    }

    // prepare our sql statements
    const char *pSQL[6];
    pSQL[0] = "CREATE TABLE Employee(Firstname varchar(30), Lastname varchar(30), Age smallint)";
    pSQL[1] = "INSERT INTO Employee(Firstname, Lastname, Age) VALUES ('Woody', 'Alan', 45)";
    pSQL[2] = "INSERT INTO Employee(Firstname, Lastname, Age) VALUES ('Micheal', 'Bay', 38)";
    pSQL[3] = "SELECT * FROM Employee";

    // execute sql
    for(int i = 0; i < 4; i++)
    {
        rc = sqlite3_exec(db, pSQL[i], callback, 0, &szErrMsg);
        if(rc != SQLITE_OK)
        {
        std::cout << "SQL Error: " << szErrMsg << std::endl;
        sqlite3_free(szErrMsg);
        break;
        }
    }

    // close database
    if(db)
    {
        sqlite3_close(db);
    }

    CROW_ROUTE(app, "/api/db")//exemple de route pour la mise en place du REST API
    ([](){
        //return "Hello, world!";
        return "{a:3,b:3}";
    });

    CROW_ROUTE(app, "/api/db2")//exemple de route pour la mise en place du REST API
    ([](){
        //return "Hello, world!";
        return "{a:3,b:4}";
    });

    CROW_ROUTE(app, "/api/post").methods(crow::HTTPMethod::Post)([](const crow::request& req){
        crow::json::rvalue x = crow::json::load(req.body);//charge le corps de la requete post et on teste si elle est convenable
        if (!x)
            return crow::response(400, "Invalid JSON");//sinon on envoie un code d'erreur HTTP
        
        //std::cout<<req.body<<std::endl;//afficher la body (debogage)

        Json::Value obj;
        std::stringstream(req.body) >> obj;//on met req.body en objet JSON lisible par C++


        std::string value1 = obj["param1"].asString();//Lire une valeur et la mettre dans une variable pour l'utiliser
        std::cout<<"Value : "<<value1<<std::endl;
        
        // Create a response
        std::ostringstream os;
        os << "message de reponse" ;
        return crow::response(200,os.str());//on envoie un code de retour positif
    });


    std::cout<<"Listening on port : "<<port<<std::endl;
    app.port(port).multithreaded().run();//serveur à l'écoute en gestion de requete multithreading

    return 0;
}

//g++ -o cs crowserver.cpp  -I/home/user18/ProjetCPP/crow/include -lsqlite3 -lboost_system -lpthread -ljsoncpp  //Celle-ci