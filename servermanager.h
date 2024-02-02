#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "jsoncpp/json/json.h"
#include "datetime.h"
#include "bdd.h"

class DatabaseManager;

class ServerManager {
public:

    static void log(const std::string logstr);

    static void errorlog(const std::string errorstr);

private:
    ServerManager() {}
};

class JSONhelper: public ServerManager{
public : 
    static Json::Value prepareConnectionJSON(DatabaseManager DBM);

    static std::string json_to_string(Json::Value json);

    static Json::Value string_to_json(std::string str);
};
