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

static int callback(void *NotUsed, int argc, char **argv, char **ColName)
{
  for(int i = 0; i < argc; i++)
  {
    std::cout << ColName[i] << " = " << argv[i] << std::endl;
  }

  std::cout << "\n";

  return 0;

}

void generateEvents(const int numEvents, std::string gender, std::string sport, std::string isteamsport, std::vector<std::string> CountriesArray, std::vector<std::string> stadiums, sqlite3* db)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> stadiumdis(0, stadiums.size() - 1);
    std::uniform_int_distribution<> timedis(5, 30);//distribution pour generer la fin d'un match entre 5 et 30 min après la création de la base de donnée
    auto randeng = std::default_random_engine {rd()};
    std::shuffle(std::begin(CountriesArray), std::end(CountriesArray), randeng);

    std::string countries;
    std::vector<float> cotes;
    int index1=0;
    int index2=0;
    std::string starttime,endtime;
    int numevents=numEvents;
    for(int i=0;i<CountriesArray.size()-1;i+=2)
    {
        numevents+=1;
        countries=CountriesArray[i]+" | "+CountriesArray[i+1];
        //std::cout<<countries<<std::endl;
        std::string stadium = stadiums[stadiumdis(gen)];
        starttime=getnowtime();
        endtime=getnowtime(timedis(gen));
        spawnEvent(gender.c_str(),sport.c_str(),countries.c_str(),isteamsport.c_str(),starttime.c_str(),endtime.c_str(),stadium.c_str(),db);
        cotes=generateCote(2);
        //std::cout<<strnumevents<<std::endl;
        addBetDispo(std::to_string(numevents).c_str(),CountriesArray[i].c_str(),std::to_string(cotes[0]).c_str(),endtime.c_str(),db);
        addBetDispo(std::to_string(numevents).c_str(),CountriesArray[i+1].c_str(),std::to_string(cotes[1]).c_str(),endtime.c_str(),db);
    }
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
    pSQL[2] = "CREATE TABLE BetDispo(id integer primary key autoincrement, eventid int not null, choice varchar(30) not null, cote float not null, closetime varchar(30) not null)";
    pSQL[3] = "CREATE TABLE UserBet(id integer primary key autoincrement, username varchar(30) not null, betAmount float not null, betid int not null)";
    pSQL[4] = "CREATE TABLE Event(id integer primary key, gender varchar(1) not null, sport varchar(20) not null, countries varchar(300) not null, isteamsport tinyint not null, start varchar(30) not null, end varchar(30) not null,stadium varchar(40))";//"YYYY-MM-DD HH:MM:SS.SSS"
    pSQL[5] = "CREATE TABLE Player(id integer primary key, first_name varchar(30) not null, last_name varchar(30) not null, gender varchar(1) not null, country varchar(30) not null, sport varchar(30) not null)";

    // execute sql
    for(int i = 0; i < 6; i++)
    {
        rc = sqlite3_exec(db, pSQL[i], callback, 0, &ErrMsg);
        if(rc != SQLITE_OK)
        {
            std::cout << "SQL Error: " << ErrMsg << std::endl;
            sqlite3_free(ErrMsg);
            //break;
        }
    }
    
    //Creation du superuser (avec premium et très grand solde)
    addUser("olympicstake@gmail.com","superuser","$5$94HjxpMwUw$87nX0arkD.9lw5Jyl0gdDXoPenJQcrMGg5gdCgOD6k2",db,"User");//hash du mdp : Superuser123_
    setPremium("olympicstake@gmail.com",true,db);
    setBalance("olympicstake@gmail.com",100000000,db);

    std::vector<std::string> HFootCountries ={"France","Japon","Russie","Maroc","Italie","Finlande","Angleterre","Jamaique"};
    std::vector<std::string> FFootCountries ={"Espagne","Chine","Belgique","Tunisie","Portugal","Argentine","Colombie","Pays-Bas"};
    std::vector<std::string> stadiums ={"Stade de France","Orange Velodrome","Groupama Stadium","Parc des Princes","Matmut Atlantique","Allianz Riviera","Stade de Gerland","Roazhon Park","Stade Océane","Stade de la Mosson","Stade des Alpes","Stadium de Toulouse","Paris Defense Arena"};
    

    //Creation des equipes de Football
    for(auto country : FFootCountries)
    {
        spawnPlayer(11,"F",country.c_str(),"Football",db);
    }
    for(auto country : HFootCountries)
    {
        spawnPlayer(11,"H",country.c_str(),"Football",db);
    }

    //Creation des evenenements
    int numEvents=0;
    generateEvents(numEvents,"F","Football","Yes",FFootCountries,stadiums,db);
    numEvents+=FFootCountries.size()/2;
    generateEvents(numEvents,"H","Football","Yes",HFootCountries,stadiums,db);
    numEvents+=HFootCountries.size()/2;

    addUserBet("superuser","1020.91","1",db);

    std::vector<Json::Value> tab=getBetDispo(db);
    for(int i=0; i<tab.size();i++)
    {
        std::cout<<tab[i]<<std::endl;
    }

    //Close database
    if(db)
    {
        sqlite3_close(db);
    }
    return 0;
}




// g++ -o ib initBDDS.cpp bdd.cpp datetime.cpp hashcode.cpp -lsqlite3 -lpthread -ljsoncpp -lcrypt
//Penser à supprimer la base de donnée existante avant d'éxecuter
