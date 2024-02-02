#include <iostream>
#include "/usr/include/sqlite3.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "jsoncpp/json/json.h"
#include "sendmail.h"
#include "hashcode.h"
#include "bdd.h"
#include "datetime.h"
#include <thread>
#include <chrono>
#include <algorithm>
#include <random>
#include "servermanager.h"

void generateEvents(const int numEvents, std::string gender, std::string sport, std::vector<std::string> CountriesArray, std::vector<std::string> stadiums, Spawner spawn)//permet de generer des evenements sportifs et de creer les paris qui vont avec
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> stadiumdis(0, stadiums.size() - 1);
    std::uniform_int_distribution<> timedis(5, 30);//distribution pour generer la fin d'un match entre 5 et 30 min après la création de la base de donnée
    auto randeng = std::default_random_engine {rd()};
    std::shuffle(std::begin(CountriesArray), std::end(CountriesArray), randeng);

    std::string country1,country2;
    std::vector<float> cotes;
    std::string starttime,endtime;
    int numevents=numEvents;
    for(size_t i=0;i<CountriesArray.size()-1;i+=2)//On genere un evenement et les paris qui lui sont associés
    {
        numevents+=1;
        country1=CountriesArray[i];
        country2=CountriesArray[i+1];
        std::string stadium = stadiums[stadiumdis(gen)];
        starttime=getnowtime();
        endtime=getnowtime(timedis(gen));
        spawn.spawnEvent(gender.c_str(),sport.c_str(),country2.c_str(), country1.c_str(),starttime.c_str(),endtime.c_str(),stadium.c_str());
        cotes=generateCote(2);
        spawn.addBetDispo(std::to_string(numevents).c_str(),CountriesArray[i].c_str(),std::to_string(cotes[0]).c_str(),endtime.c_str());
        spawn.addBetDispo(std::to_string(numevents).c_str(),CountriesArray[i+1].c_str(),std::to_string(cotes[1]).c_str(),endtime.c_str());
    }
}

int main()
{
    Spawner spawn;

    const char *pSQL[5];

    pSQL[0] = "CREATE TABLE User(email varchar(50) primary key , username varchar(30) unique not null, passwordH varchar(57) not null, ispremium tinyint not null, balance real not null, signupdate varchar(30) not null)";
    pSQL[1] = "CREATE TABLE Usertemp(email varchar(50) primary key , username varchar(30) unique not null, passwordH varchar(57) not null, code varchar(6))";
    pSQL[2] = "CREATE TABLE BetDispo(id integer primary key autoincrement, eventid int not null, choice varchar(30) not null, cote float not null, closetime varchar(30) not null)";
    pSQL[3] = "CREATE TABLE UserBet(id integer primary key autoincrement, username varchar(30) not null, betAmount float not null, betid int not null)";
    pSQL[4] = "CREATE TABLE Event(id integer primary key, gender varchar(1) not null, sport varchar(20) not null, country1 varchar(30) not null, country2 varchar(30) not null, start varchar(30) not null, end varchar(30) not null,stadium varchar(40))";//"YYYY-MM-DD HH:MM:SS.SSS"
    pSQL[5] = "CREATE TABLE Player(id integer primary key, first_name varchar(30) not null, last_name varchar(30) not null, gender varchar(1) not null, country varchar(30) not null, sport varchar(30) not null)";

    //On fait l'initialisation de la base de donnée
    int rc;
    char *ErrMsg = 0;
    for(int i = 0; i < 6; i++)
    {
        rc = sqlite3_exec(spawn.getdb(), pSQL[i], callback, 0, &ErrMsg);
        if(rc != SQLITE_OK)
        {
            std::cout << "SQL Error: " << ErrMsg << std::endl;
            sqlite3_free(ErrMsg);
        }
    }

    std::vector<std::string> sports ={"Football","Basketball"};
    std::vector<std::string> countries ={"Algeria","Argentina","Brazil","Canada","France","Germany","Italy","Japan","Spain","USA"};
    std::vector<std::string> stadiums ={"Stade de France","Orange Velodrome","Groupama Stadium","Parc des Princes","Matmut Atlantique","Allianz Riviera","Stade de Gerland","Roazhon Park","Stade Océane","Stade de la Mosson","Stade des Alpes","Stadium de Toulouse","Paris Defense Arena"};
    

    //Creation des equipes de Football
    for(auto country : countries)
    {
        spawn.spawnPlayer(11,"F",country.c_str(),"Football");
        spawn.spawnPlayer(5,"F",country.c_str(),"Basketball");
    }
    for(auto country : countries)
    {
        spawn.spawnPlayer(11,"H",country.c_str(),"Football");
        spawn.spawnPlayer(5,"H",country.c_str(),"Basketball");
    }

    //Creation des evenenements
    int numEvents=0;
    int added=countries.size()/2;
    for(auto sport : sports)
    {
        generateEvents(numEvents,"F",sport,countries,stadiums,spawn);
        numEvents+=added;
        // generateEvents(numEvents,"H",sport,countries,stadiums,spawn);
        // numEvents+=added;
    }

    // spawn.addUserBet("superuser","1020.91","2");
    // spawn.addUserBet("superuser","463.91","7");

    /*std::vector<Json::Value> tab=DBM.getUser("superuser");
    for(size_t i=0; i<tab.size();i++)
    {
        std::cout<<tab[i]<<std::endl;
    }*/

    sqlite3_close(spawn.getdb());

    return 0;
}

// g++ -o ib initBDDS.cpp bdd.cpp datetime.cpp hashcode.cpp -lsqlite3 -lpthread -ljsoncpp -lcrypt
//Penser à supprimer la base de donnée existante avant d'éxecuter
