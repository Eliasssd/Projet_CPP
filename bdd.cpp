#include <iostream>
#include "/usr/include/sqlite3.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "jsoncpp/json/json.h"
#include "bdd.h"
#include "sendmail.h"
#include <string.h>
#include <chrono>
#include <thread>
#include "datetime.h"
#include "hashcode.h"
#include "servermanager.h"

//==============================A REFAIRE=====================
//1 checkUser est inutile car quand la clause UNIQUE est violée, la colonne problématique est renvoyée
//2 Réecrire plus proprement les codes en mettant des if imbriqués testant les SQLITE_XXX plutôt que les valeurs des rc
//3 Essayer d'écrire que du c++ pour les strings au lieu d'utiliser des char* ceci fonctionne (std::string cppString(cString);)
//4 Essayer de faire des fonctions plus générales pour les queries
//5 DATABASE MANAGER
//6 Implementer des logs
//============================================================

int callback(void *NotUsed, int argc, char **argv, char **ColName)//Fonction classique de callback SQL
{
  for(int i = 0; i < argc; i++)
  {
    std::cout << ColName[i] << " = " << argv[i] << std::endl;
  }

  std::cout << "\n";

  return 0;

}

DatabaseManager::DatabaseManager()//Constructeur d'instance de DatabseManager
{
    int rc=sqlite3_open("Database.db", &(this->db));
    if(rc)
    {
        //std::cout << "Can't open database\n";
        ServerManager::errorlog("Can't open database");
        exit(1);
    } else {
        //std::cout << "Open database successfully\n";
        ServerManager::log("Open database successfully");
    }
}

const int DatabaseManager::checkUser(const char* email, const char* username)//Vérifie qu'un utilisateur n'est pas déjà dans la base de donnée, 0 si pas de doublon, 1 si erreur, 2 si email en double, 3 si username en double
{
    const char* request[2];
    request[0] = "SELECT email FROM User WHERE email == (?)";//On fait les deux requetes séparément pour voir laquelle pose problème
    request[1] = "SELECT username FROM User WHERE email == (?)";
    
    sqlite3_stmt *stmt;//objet requete
    for(int i=0;i<2;i++)
    {
        int rc = sqlite3_prepare_v2(this->db, request[i], -1, &stmt, NULL);//code de retour
        if (rc != SQLITE_OK) {
            //std::cerr << "Failed to prepare statement: checkUser " << sqlite3_errmsg(this->db) << std::endl;
            ServerManager::errorlog("Failed to prepare statement: checkUser "+std::string(sqlite3_errmsg(this->db)));
            //sqlite3_close(this->db);
            return 1;
        }

        sqlite3_bind_text(stmt, 1, email, -1, SQLITE_TRANSIENT);
    

        rc = sqlite3_step(stmt);//code de retour
        if (rc != SQLITE_DONE) {
            //std::cerr << "Execution failed: checkUser " << sqlite3_errmsg(this->db) << std::endl;
            ServerManager::errorlog("Execution failed: checkUser "+std::string(sqlite3_errmsg(this->db)));
            //sqlite3_close(this->db);
            return 1;
        }
        else if(rc==SQLITE_ROW)
        {//il y a au moins une ligne dans le resultat dans l'utilisateur existe deja
            return 2+i;//2 si email en double, 3 si username en double
        }
    }
    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

int DatabaseManager::addUser(const char* timestamp,const char* email, const char* username, const char* passwordH, const std::string table)//Ajoute un utilisateur dans la table User
{
    std::string request;
    if(table=="User")
    {
        request = "INSERT INTO user (email, username, passwordH, ispremium, balance, signupdate) VALUES (?, ?, ?, ?, ?, ?)";
    }
    else if(table=="Usertemp")
    {
        request = "INSERT INTO usertemp (email, username, passwordH) VALUES (?, ?, ?)";
    }
    
    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request.c_str(), -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        //std::cerr << "Failed to prepare statement: addUser " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Failed to prepare statement: addUser "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }

    if(table=="User")
    {
        sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, passwordH, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, "0", -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, "0.0", -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 6, timestamp, -1, SQLITE_STATIC);
    }
    else if(table=="Usertemp")
    {
        sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, passwordH, -1, SQLITE_STATIC);
    }
   

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        //std::cerr << "Execution failed: addUser " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Execution failed: addUser "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }

    //std::cout << "Data inserted successfully." << std::endl;
    ServerManager::log(std::string(email)+" : Data inserted successfully.");

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

const int DatabaseManager::updateCode(const char* email, const char* code)//Ajoute le code 2FA à la base de donnée temporaire
{
    const char* request = "UPDATE Usertemp SET code = (?) WHERE email=(?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        //std::cerr << "Failed to prepare statement: updateCode " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Failed to prepare statement: updateCode "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }

    sqlite3_bind_text(stmt, 1, code, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        //std::cerr << "Execution failed: updateCode " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Execution failed: updateCode "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }

    std::cout << "Data updated successfully." << std::endl;

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

const int DatabaseManager::compareCode(const char* email, const char* code,const bool adduser)//Si les codes 2FA de la base de donnée et de la requete matchent on retourne 0, sinon 1
{
    const char* request = "SELECT * FROM Usertemp WHERE email = (?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        //std::cerr << "Failed to prepare statement: compareCode " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Failed to prepare statement: compareCode "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);//code de retour

    const char* codeDB;
    codeDB=(const char*)malloc(10*sizeof(codeDB));
    //const char codeDB[6] //priviliegier ca
    
    if (rc == SQLITE_ROW) {


        // const unsigned char* text = sqlite3_column_text(stmt, 0);
        // std::cout << "email"<<(text ? reinterpret_cast<const char*>(text) : "NULL1") << " ";
        codeDB = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)); //code est dans la colonne 4
        //std::cout <<"codeDB : "<< (codeDB ? reinterpret_cast<const char*>(codeDB) : "NULLDB") << " ";

    } else if (rc == SQLITE_DONE) {
        //std::cerr << "Execution failed: compareCode " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Execution failed: compareCode "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }
    if(strcmp(code,codeDB))
    {
        //std::cout<<"Code comparison is false"<<std::endl;
        ServerManager::errorlog("Code comparison is false: compareCode "+std::string(sqlite3_errmsg(this->db)));
        return 1;
    }
    if(adduser)
    {   
        if(this->addUser(getnowtime().c_str(),reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),"User")!=0)
        {
            //std::cout << "Failed to insert data in user" << std::endl;
            ServerManager::errorlog("Failed to insert data in user: compareCode "+std::string(sqlite3_errmsg(this->db)));
            return 1;
        }
        //std::cout << "Data succesfully inserted in user" << std::endl;
        ServerManager::log(std::string(email)+" : Data succesfully inserted in user: compareCode");
    }

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

const std::string DatabaseManager::getSalt(const char* email)//Récupère le salt d'un hash pour l'envoyer afin de faire la vérification 2FA
{
    const char* request = "SELECT * FROM User WHERE email = (?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->getdb(), request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        //std::cerr << "Failed to prepare statement: getSalt " << sqlite3_errmsg(this->getdb()) << std::endl;
        ServerManager::errorlog("Failed to prepare statement: getSalt "+std::string(sqlite3_errmsg(this->getdb())));
            //sqlite3_close(DatabaseManager::db);
        //sqlite3_close(this->getdb());
        return NULL;
    }
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);//code de retour

    const char* hash;
    hash=(const char*)malloc(20*sizeof(hash));
    //const char hash[6] //priviliegier ca
    //char salt[14];
    if (rc == SQLITE_ROW) {


        // const unsigned char* text = sqlite3_column_text(stmt, 0);
        // std::cout << "email"<<(text ? reinterpret_cast<const char*>(text) : "NULL1") << " ";
        hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)); //code est dans la colonne 4
        //std::cout <<"hash : "<< (hash ? reinterpret_cast<const char*>(hash) : "NULLHash") << " "<<std::endl;


    } else if (rc == SQLITE_DONE) {
        //std::cerr << "Execution failed: getSalt " << sqlite3_errmsg(this->getdb()) << std::endl;
        ServerManager::errorlog("Execution failed: getSalt "+std::string(sqlite3_errmsg(this->getdb())));
            //sqlite3_close(DatabaseManager::db);
        //sqlite3_close(this->getdb());
        return NULL;
    }
    std::string H(hash);
    std::string S;
    S=H.substr(3,10); //On enleve $5$ et le $ d'apres
    // strncpy(salt, hash, 14);
    // salt[14] = '\0'; // Ensuring the string is null-terminated
    //std::cout <<"salt : "<< (S.empty() ? "NULLSalt" : S) << " "<<std::endl;
    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return S;
}

const int Spawner::spawnPlayer(const int number, const std::string gender, const char* country, const char* sport)//Génère number joueur(s) avec un nom prenom aléatoire, un genre, un pays, un sport
{
    std::vector<std::string> F = { "Jade", "Louise", "Emma","Alice", "Ambre", "Lina", "Rose","Chloe", "Mia","Lea", "Anna", "Mila", "Julia","Romy", "Lou","Ines", "Lena", "Agathe", "Juliette","Inaya", "Nina", "Zoe", "Jeanne","Leonie","Iris" };
    std::vector<std::string> H = { "Leo", "Gabriel", "Raphael","Arthur","Louis", "Jules", "Mael","Lucas","Hugo", "Noah", "Liam", "Gabin","Sacha","Paul", "Nathan", "Aaron","Mohamed","Ethan","Eden", "Tom","Leon","Noe","Tiago","Theo","Isaac"};

    std::string request = "INSERT INTO Player (first_name, last_name, gender, country, sport) VALUES (?, ?, ?, ?, ?)";
    
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<> distribution(0, 24);//instancie une distribution uniforme
    std::string first_name,last_name;

    sqlite3_stmt *stmt;//objet requete
    for(int i=0;i<number;i++)
    {
        if(gender=="F")
        {
            first_name=F[distribution(generator)];
            last_name=F[distribution(generator)];
        }
        else
        {
            first_name=H[distribution(generator)];
            last_name=H[distribution(generator)];
        }
        int rc = sqlite3_prepare_v2(DatabaseManager::db, request.c_str(), -1, &stmt, NULL);//code de retour
        if (rc != SQLITE_OK) {
            //std::cerr << "Failed to prepare statement: spawnPlayer " << sqlite3_errmsg(DatabaseManager::db) << std::endl;
            ServerManager::errorlog("Failed to prepare statement: spawnPlayer "+std::string(sqlite3_errmsg(DatabaseManager::db)));
            //sqlite3_close(DatabaseManager::db);
            return 1;
        }
        sqlite3_bind_text(stmt, 1, first_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, last_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, gender.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, country, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, sport, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);//code de retour
        if (rc != SQLITE_DONE) {
            //std::cerr << "Execution failed: spawnPlayer " << sqlite3_errmsg(DatabaseManager::db) << std::endl;
            ServerManager::errorlog("Execution failed: spawnPlayer "+std::string(sqlite3_errmsg(DatabaseManager::db)));
            //sqlite3_close(DatabaseManager::db);
            return 1;
        }
        //std::cout << "Data inserted successfully." << std::endl;
    }
    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement
    return 0;
}

const int Spawner::spawnEvent(const char* gender, const char* sport, const char* country1, const char* country2, const char* start, const char* end, const char* stadium)//Génère number joueur(s) avec un nom prenom aléatoire, un genre, un pays, un sport
{
    std::string request = "INSERT INTO Event (gender, sport, country1, country2, start, end, stadium) VALUES (?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(DatabaseManager::db, request.c_str(), -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        //std::cerr << "Failed to prepare statement: spawnEvent " << sqlite3_errmsg(DatabaseManager::db) << std::endl;
        ServerManager::errorlog("Failed to prepare statement: spawnEvent "+std::string(sqlite3_errmsg(DatabaseManager::db)));
        //sqlite3_close(DatabaseManager::db);
        return 1;
    }
    sqlite3_bind_text(stmt, 1, gender, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, sport, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, country1, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, country2, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, start, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, end, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, stadium, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        //std::cerr << "Execution failed: spawnEvent " << sqlite3_errmsg(DatabaseManager::db) << std::endl;
        ServerManager::errorlog("Execution failed: spawnEvent "+std::string(sqlite3_errmsg(DatabaseManager::db)));
        //sqlite3_close(DatabaseManager::db);
        return 1;
    }
    //std::cout << "Data inserted successfully." << std::endl;
    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement
    return 0;
}

const int DatabaseManager::setPremium(const char* username, const bool isPremium)//Accorde ou enlève les droits premiums à un utilisateur
{
    const char* request = "UPDATE User SET ispremium = (?) WHERE username=(?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        //std::cerr << "Failed to prepare statement: setPremium " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Failed to prepare statement: setPremium "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }

    const char* premstr=isPremium?"Yes":"No";

    sqlite3_bind_text(stmt, 1, premstr, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        //std::cerr << "Execution failed: setPremium " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Execution failed : setPremium "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }

    //std::cout << "Data updated successfully." << std::endl;
    ServerManager::log(std::string(username)+" : Data updated successfully : setPremium");

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

const int DatabaseManager::setBalance(const char* username, const float balance)//Set la valeur du solde d'un utilisateur
{
    const char* request = "UPDATE User SET balance = (?) WHERE username=(?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        //std::cerr << "Failed to prepare statement: updateBalance " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Failed to prepare statement: updateBalance "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }

    const char* balancestr=std::to_string(balance).c_str();

    sqlite3_bind_text(stmt, 1, balancestr, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        //std::cerr << "Execution failed: updateBalance " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Execution failed : updateBalance "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }

    //std::cout << "Data updated successfully." << std::endl;
    ServerManager::log(std::string(username)+" : Data updated successfully : updateBalance");

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

const int DatabaseManager::addUserBet(const char* username, const char* amount, const char* betid)//Ajoute le pari d'un utilisateur dans la database
{
    std::string request = "INSERT INTO userbet (username, betamount, betid) VALUES (?, ?, ?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request.c_str(), -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        //std::cerr << "Failed to prepare statement: addUserBet " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Failed to prepare statement: addUserBet "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, amount, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, betid, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        //std::cerr << "Execution failed: addUserBet " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Execution failed: addUserBet " +std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }
    //std::cout << "Data inserted successfully." << std::endl;
    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement
    return 0;
}

const int DatabaseManager::addBetDispo(const char* eventid, const char* choice, const char* cote, const char* closetime)//Ajoute un pari disponible pour l'utilisateur dans la database des paris qu'il peut choisir
{
    std::string request = "INSERT INTO betdispo (eventid, choice, cote, closetime) VALUES (?, ?, ?, ?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request.c_str(), -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        //std::cerr << "Failed to prepare statement: addBetDispo " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Failed to prepare statement: addBetDispo "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }
    sqlite3_bind_text(stmt, 1, eventid, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, choice, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, cote, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, closetime, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        //std::cerr << "Execution failed: addBetDispo " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Execution failed: addBetDispo "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        return 1;
    }
    //std::cout << "Data inserted successfully." << std::endl;
    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement
    return 0;
}

std::vector<Json::Value> DatabaseManager::getBetDispo()//Récupère tous les paris disponibles
{
    const char* request = "SELECT * FROM betdispo";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: getBetDispo " << sqlite3_errmsg(this->db) << std::endl;
        //sqlite3_close(this->db);
        exit(1);
    }
    //rc = sqlite3_step(stmt);//code de retour
    std::vector<Json::Value> returnVec;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Json::Value obj;
        obj["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        obj["eventid"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        obj["choice"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        obj["cote"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        obj["closetime"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        //std::cout << "Le JSON complet : " << obj<< std::endl;
        returnVec.push_back(obj);
    }

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return returnVec;
}

std::vector<Json::Value> DatabaseManager::getEvent()//Récupère tous les évènements
{
    const char* request = "SELECT * FROM event";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        //std::cerr << "Failed to prepare statement: getEvent " << sqlite3_errmsg(this->db) << std::endl;
        ServerManager::errorlog("Failed to prepare statement: getEvent "+std::string(sqlite3_errmsg(this->db)));
        //sqlite3_close(this->db);
        exit(1);
    }
    //rc = sqlite3_step(stmt);//code de retour
    std::vector<Json::Value> returnVec;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Json::Value obj;
        obj["id"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        obj["gender"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        obj["sport"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        obj["country1"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        obj["country2"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        obj["start"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        obj["end"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        obj["stadium"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        //std::cout << "Le JSON complet : " << obj<< std::endl;
        returnVec.push_back(obj);
    }

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return returnVec;
}

std::vector<Json::Value> DatabaseManager::getUserBet(const char* username)//Récupère tous les paris d'un utilisateur
{
    const char* request = "SELECT * FROM userbet WHERE username=(?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: getUserBet " << sqlite3_errmsg(this->db) << std::endl;
        //sqlite3_close(this->db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    //rc = sqlite3_step(stmt);//code de retour
    std::vector<Json::Value> returnVec;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Json::Value obj;
        //obj["username"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));//pas vraiment utile mais bon si besoin...
        obj["betAmount"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        obj["betid"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        //std::cout << "Le JSON complet : " << obj<< std::endl;
        returnVec.push_back(obj);
    }

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return returnVec;
}

Json::Value DatabaseManager::getUser(const char* email)//Récupère les informations d'un utilisateur
{
    const char* request = "SELECT * FROM user WHERE email=(?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(this->db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: getUser " << sqlite3_errmsg(this->db) << std::endl;
        //sqlite3_close(this->db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);//code de retour
    Json::Value obj;
    // //obj["email"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));//pas vraiment utile mais bon si besoin...
    obj["username"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));//pas vraiment utile mais bon si besoin...
    // //obj["passwordH"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));//pas vraiment utile mais bon si besoin...
    obj["ispremium"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    obj["balance"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    //obj["signupdate"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
    //std::cout << "Le JSON complet : " << obj<< std::endl;

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return obj;
}

sqlite3* DatabaseManager::getdb()//Getter pour la base de donnée associée au DatabaseManager
{
    return this->db;
}
