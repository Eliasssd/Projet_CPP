#include <iostream>
#include "/usr/include/sqlite3.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "jsoncpp/json/json.h"

static int callback(void *NotUsed, int argc, char **argv, char **ColName)
{
  for(int i = 0; i < argc; i++)
  {
    std::cout << ColName[i] << " = " << argv[i] << std::endl;
  }

  std::cout << "\n";

  return 0;

}

int addUser(const char* email, const char* username, const char* passwordH, sqlite3* db)
{
    const char* request = "INSERT INTO User (email, username, passwordH) VALUES (?, ?, ?)";
    
    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, passwordH, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    std::cout << "Data inserted successfully." << std::endl;

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

int main()
{
    sqlite3 *db;//objet base de donnée
    char *ErrMsg = 0;

    int rc = sqlite3_open("User.db", &db);//ouverture de la base de donnée

    if(rc)//Test de l'ouverture de la base de données
    {
        std::cout << "Can't open database\n";
    } else {
        std::cout << "Open database successfully\n";
    }

    const char *pSQL[3];

    pSQL[0] = "CREATE TABLE User(email varchar(50) primary key , username varchar(30) unique not null, passwordH varchar(57) not null)";
    pSQL[1] = "CREATE TABLE User(email varchar(50) primary key , username varchar(30) unique not null, passwordH varchar(57) not null)";
    pSQL[2] = "INSERT INTO User(email, username, passwordH) VALUES ('Woody@email.a', 'CrazyBet', 'knenobuecviefgzhube')";

    // execute sql
    for(int i = 0; i < 2; i++)
    {
        rc = sqlite3_exec(db, pSQL[i], callback, 0, &ErrMsg);
        if(rc != SQLITE_OK)
        {
            std::cout << "SQL Error: " << ErrMsg << std::endl;
            sqlite3_free(ErrMsg);
            //break;
        }
    }

    addUser("aof@fnef.c","firstMan","ucebiivbeibvoe",db);

    // close database
    if(db)
    {
        sqlite3_close(db);
    }
    return 0;
}

// g++ -o bdd bdd.cpp -lsqlite3 -lpthread -ljsoncpp
