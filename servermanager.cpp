#include <iostream>
#include <string>
#include <vector>
#include "jsoncpp/json/json.h"
#include "servermanager.h"
#include "datetime.h"
#include "bdd.h"

void ServerManager::log(const std::string logstr)//Presente les logs du serveur
{
    std::cout<<"("<<getnowtime()<<") "<<logstr<<std::endl;
}

void ServerManager::errorlog(const std::string errorstr)//Presente les logs d'erreurs du serveur
{
    std::cerr<<"/!\\("<<getnowtime()<<")/!\\ "<<errorstr<<std::endl;
}

Json::Value JSONhelper::prepareConnectionJSON(DatabaseManager DBM)
{
    Json::Value obj;

    std::vector<Json::Value> eventjsons = DBM.getEvent();
    std::vector<Json::Value> betdispojsons = DBM.getBetDispo();
    // Créer un tableau JSON
    Json::Value eventArray;
    for(auto eventjson:eventjsons)
    {
        eventArray.append(eventjson);
    }
    obj["events"]=eventArray;
    Json::Value betdispoArray;
    for(auto betdispojson:betdispojsons)
    {
        betdispoArray.append(betdispojson);
    }
    obj["betdispo"]=betdispoArray;
    return obj;
}

std::string JSONhelper::json_to_string(Json::Value obj)//Transforme un json en chaine de caractère (utile pour l'envoie de requetes)
{
    Json::StreamWriterBuilder builder;
    //builder["commentStyle"] = "None";
    builder["indentation"] = "";
    std::string output = Json::writeString(builder, obj);
    //std::cout << output << std::endl;
    return output;
}

Json::Value JSONhelper::string_to_json(std::string str) {//Transforme une chaine de caractère en json (utile pour la réception de requetes)
    Json::CharReaderBuilder builder;
    Json::Value obj;
    std::string errors;

    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(str.c_str(), str.c_str() + str.size(), &obj, &errors)) {
        std::cerr << errors << std::endl;
    }
    return obj;
}
