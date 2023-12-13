#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <iostream>
#include "crow.h"

int main() {

    crow::SimpleApp app;

    const int port=18080;

    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();//instance du pilote mysql pour demander la connexion
        std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "serveruser", "serveruserpwd"));//établit la connexion avec la base de données
        con->setSchema("test_db");//selection de la base de données

        std::unique_ptr<sql::Statement> stmt(con->createStatement());//création de l'objet statement nécessaire pour faire des requetes la base
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM users"));//exécution d'une requete

        while (res->next()) {//parcours des lignes retournées par la requete 
            std::cout << res->getString("your_column") << std::endl;
        }
    } catch (sql::SQLException &e) {//gestion d'erreur
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    CROW_ROUTE(app, "/api/")//exemple de route pour la mise en place du REST API
    ([](){
        return "Hello, world!";
    });

    std::cout<<"Listening on port : "<<port<<std::endl;
    app.port(port).multithreaded().run();//server à l'écoute en gestion de requete multithreading

    return 0;
}

//g++ -o crowserver crowserver.cpp -I/home/user18/ProjetCPP/crow/include -lmysqlcppconn -lboost_system -lpthread