#pragma once
#include <iostream>
#include "/usr/include/sqlite3.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "jsoncpp/json/json.h"
#include <vector>
#include "servermanager.h"

int callback(void *NotUsed, int argc, char **argv, char **ColName);

//int addUser(const char* timestamp, const char* email, const char* username, const char* passwordH, sqlite3* db, const std::string table);

//const int updateCode(const char* email, const char* code, sqlite3* db);

//const int checkUser(const char* email, const char* username, sqlite3* db);

//const int compareCode(const char* email, const char* code, sqlite3* db, const bool adduser);

//std::string getSalt(const char* email,sqlite3* db);

//const int setPremium(const char* email, const bool isPremium, sqlite3* db);

//const int setBalance(const char* email, const int balance, sqlite3* db);

//const int addUserBet(const char* username, const char* amount, const char* betid, sqlite3* db);

//const int addBetDispo(const char* eventid, const char* choice, const char* cote, const char* closetime, sqlite3* db);

// std::vector<Json::Value> getBetDispo(sqlite3* db);

// std::vector<Json::Value> getEvent(sqlite3* db);

// std::vector<Json::Value> getUserBet(const char* username, sqlite3* db);

// std::vector<Json::Value> getUser(const char* username, sqlite3* db);

class DatabaseManager {
public:

    DatabaseManager();

    ~DatabaseManager(){
        sqlite3_close(this->db);
    }

    const int checkUser(const char* email, const char* username);

    int addUser(const char* timestamp, const char* email, const char* username, const char* passwordH, const std::string table);

    const int updateCode(const char* email, const char* code);

    const int compareCode(const char* email, const char* code, const bool adduser);

    const int addBetDispo(const char* eventid, const char* choice, const char* cote, const char* closetime);

    const int setBalance(const char* username, const float balance);

    const int setPremium(const char* username, const bool isPremium);

    const int addUserBet(const char* username, const char* amount, const char* betid);
    
    const std::string getSalt(const char* email);

    std::vector<Json::Value> getBetDispo();

    std::vector<Json::Value> getEvent();

    std::vector<Json::Value> getUserBet(const char* username);

    Json::Value getUser(const char* email);

    sqlite3* getdb();

protected:
    sqlite3* db;
};

class Spawner : public DatabaseManager {
public:
    
    const int spawnEvent(const char* gender, const char* sport, const char* country1, const char* country2, const char* start, const char* end, const char* stadium);
    const int spawnPlayer(const int number, const std::string gender, const char* country, const char* sport);
    
};
