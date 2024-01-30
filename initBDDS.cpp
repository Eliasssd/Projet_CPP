#include <iostream>
#include "/usr/include/sqlite3.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "jsoncpp/json/json.h"
#include "sendmail.h"
#include "hashcode.h"
#include "bdd.h"

static int callback(void *NotUsed, int argc, char **argv, char **ColName)
{
  for(int i = 0; i < argc; i++)
  {
    std::cout << ColName[i] << " = " << argv[i] << std::endl;
  }

  std::cout << "\n";

  return 0;

}

int main()
{
    sqlite3 *db;//objet base de donnée
    char *ErrMsg = 0;

    int rc = sqlite3_open("Database.db", &db);//ouverture de la base de donnée

    if(rc)//Test de l'ouverture de la base de données
    {
        std::cout << "Can't open database\n";
    } else {
        std::cout << "Open database successfully\n";
    }

    const char *pSQL[5];

    pSQL[0] = "CREATE TABLE User(email varchar(50) primary key , username varchar(30) unique not null, passwordH varchar(57) not null, ispremium tinyint not null, balance real not null, signupdate varchar(30) not null)";
    pSQL[1] = "CREATE TABLE Usertemp(email varchar(50) primary key , username varchar(30) unique not null, passwordH varchar(57) not null, code varchar(6))";
    pSQL[2] = "CREATE TABLE Bet(id integer primary key autoincrement,  betAmount float not null, choice varchar(30) not null, username varchar(30) not null, isover tinyint not null)";
    pSQL[3] = "CREATE TABLE Event(id integer primary key, gender varchar(1) not null, sport varchar(20) not null, countries varchar(300) not null, isteamsport tinyint not null, start varchar(30) not null, end varchar(30) not null,stade varchar(40))";//"YYYY-MM-DD HH:MM:SS.SSS"
    pSQL[4] = "CREATE TABLE Player(id integer primary key, first_name varchar(30) not null, last_name varchar(30) not null, gender varchar(1) not null, country varchar(30) not null, sport varchar(30) not null)";
    //pSQL[2] = "INSERT INTO User(email, username, passwordH) VALUES ('Woodycd@email.a', 'CrazyBetluf', 'knenobuecviefgzhube')";

    // execute sql
    for(int i = 0; i < 5; i++)
    {
        rc = sqlite3_exec(db, pSQL[i], callback, 0, &ErrMsg);
        if(rc != SQLITE_OK)
        {
            std::cout << "SQL Error: " << ErrMsg << std::endl;
            sqlite3_free(ErrMsg);
            //break;
        }
    }

    //Creation des equipes de Football
    spawnPlayer(11,"F","Espagne","Football",db);
    spawnPlayer(11,"F","Chine","Football",db);
    spawnPlayer(11,"F","Belgique","Football",db);
    spawnPlayer(11,"F","Tunisie","Football",db);
    spawnPlayer(11,"H","France","Football",db);
    spawnPlayer(11,"H","Japon","Football",db);
    spawnPlayer(11,"H","Russie","Football",db);
    spawnPlayer(11,"H","Maroc","Football",db);
    //addUser("aof@fnef.c","firstMan","ucebiivbeibvoe",db);

    // close database
    if(db)
    {
        sqlite3_close(db);
    }
    return 0;
}
