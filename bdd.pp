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

//==============================A REFAIRE=====================
//1 checkUser est inutile car quand la clause UNIQUE est violée, la colonne problématique est renvoyée
//2 Réecrire plus proprement les codes en mettant des if imbriqués testant les SQLITE_XXX plutôt que les valeurs des rc
//3 Essayer d'écrire que du c++ pour les strings au lieu d'utiliser des char* ceci fonctionne (std::string cppString(cString);)
//4 Essayer de faire des fonctions plus générales pour les queries
//5 DATABASE MANAGER
//6 Implementer des logs
//============================================================

static int callback(void *NotUsed, int argc, char **argv, char **ColName)
{
  for(int i = 0; i < argc; i++)
  {
    std::cout << ColName[i] << " = " << argv[i] << std::endl;
  }

  std::cout << "\n";

  return 0;

}

const int checkUser(const char* email, const char* username, sqlite3* db)//0 si pas de doublon, 1 si erreur, 2 si email en double, 3 si username en double
{
    const char* request[2];
    request[0] = "SELECT email FROM User WHERE email == (?)";
    request[1] = "SELECT username FROM User WHERE email == (?)";
    
    sqlite3_stmt *stmt;//objet requete
    for(int i=0;i<2;i++)
    {
        int rc = sqlite3_prepare_v2(db, request[i], -1, &stmt, NULL);//code de retour
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: checkUser " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return 1;
        }

        sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
    

        rc = sqlite3_step(stmt);//code de retour
        if (rc != SQLITE_DONE) {
            std::cerr << "Execution failed: checkUser " << sqlite3_errmsg(db) << std::endl;
            //sqlite3_close(db);
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

int addUser(const char* email, const char* username, const char* passwordH, sqlite3* db, const std::string table)
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
    int rc = sqlite3_prepare_v2(db, request.c_str(), -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: addUser " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    if(table=="User")
    {
        sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, passwordH, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, "0", -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, "0.0", -1, SQLITE_STATIC);
        std::string date=getnowtime();
        //std::cout<<date.c_str()<<std::endl;
        sqlite3_bind_text(stmt, 6, date.c_str(), -1, SQLITE_STATIC);
    }
    else if(table=="Usertemp")
    {
        sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, passwordH, -1, SQLITE_STATIC);
    }
   

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: addUser " << sqlite3_errmsg(db) << std::endl;
        //sqlite3_close(db);
        return 1;
    }

    std::cout << "Data inserted successfully." << std::endl;

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

const int updateCode(const char* email, const char* code, sqlite3* db)
{
    const char* request = "UPDATE Usertemp SET code = (?) WHERE email=(?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: updateCode " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    sqlite3_bind_text(stmt, 1, code, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: updateCode " << sqlite3_errmsg(db) << std::endl;
        //sqlite3_close(db);
        return 1;
    }

    std::cout << "Data updated successfully." << std::endl;

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

const int compareCode(const char* email, const char* code, sqlite3* db,const bool adduser)//Si les codes 2FA de la base de donnée et de la requete matchent on retourne 0, sinon 1
{
    const char* request = "SELECT * FROM Usertemp WHERE email = (?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: compareCode " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
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
        std::cout <<"codeDB : "<< (codeDB ? reinterpret_cast<const char*>(codeDB) : "NULLDB") << " ";

    } else if (rc == SQLITE_DONE) {
        std::cerr << "Execution failed: compareCode " << sqlite3_errmsg(db) << std::endl;
        //sqlite3_close(db);
        return 1;
    }
    if(strcmp(code,codeDB))
    {
        std::cout<<"Code comparison is false"<<std::endl;
        return 1;
    }
    if(adduser)
    {
        if(addUser(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),db,"User")!=0)
        {
            std::cout << "Failed to insert data in user" << std::endl;
            return 1;
        }
        std::cout << "Data succesfully inserted in user" << std::endl;
    }

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

std::string getSalt(const char* email,sqlite3* db)//Récupère le salt d'un hash pour l'envoyer afin de faire la vérification 2FA
{
    const char* request = "SELECT * FROM User WHERE email = (?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: getSalt " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return NULL;
    }
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);//code de retour

    const char* hash;
    hash=(const char*)malloc(20*sizeof(hash));
    //const char hash[6] //priviliegier ca
    char salt[14];
    if (rc == SQLITE_ROW) {


        // const unsigned char* text = sqlite3_column_text(stmt, 0);
        // std::cout << "email"<<(text ? reinterpret_cast<const char*>(text) : "NULL1") << " ";
        hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)); //code est dans la colonne 4
        std::cout <<"hash : "<< (hash ? reinterpret_cast<const char*>(hash) : "NULLHash") << " "<<std::endl;


    } else if (rc == SQLITE_DONE) {
        std::cerr << "Execution failed: getSalt " << sqlite3_errmsg(db) << std::endl;
        //sqlite3_close(db);
        return NULL;
    }
    std::string H(hash);
    std::string S;
    S=H.substr(3,10); //On enleve $5$ et le $ d'apres
    // strncpy(salt, hash, 14);
    // salt[14] = '\0'; // Ensuring the string is null-terminated
    std::cout <<"salt : "<< (S.empty() ? "NULLSalt" : S) << " "<<std::endl;

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return S;
}

const int spawnPlayer(const int number, const std::string gender, const char* country, const char* sport, sqlite3* db)//Génère number joueur(s) avec un nom prenom aléatoire, un genre, un pays, un sport
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
        int rc = sqlite3_prepare_v2(db, request.c_str(), -1, &stmt, NULL);//code de retour
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: spawnPlayer " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return 1;
        }
        sqlite3_bind_text(stmt, 1, first_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, last_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, gender.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, country, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, sport, -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);//code de retour
        if (rc != SQLITE_DONE) {
            std::cerr << "Execution failed: spawnPlayer " << sqlite3_errmsg(db) << std::endl;
            //sqlite3_close(db);
            return 1;
        }
        //std::cout << "Data inserted successfully." << std::endl;
    }
    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement
    return 0;
}

const int spawnEvent(const char* gender, const char* sport, const char* countries, const char* isteamsport, const char* start, const char* end, const char* stadium, sqlite3* db)//Génère number joueur(s) avec un nom prenom aléatoire, un genre, un pays, un sport
{
    std::string request = "INSERT INTO Event (gender, sport, countries, isteamsport, start, end, stadium) VALUES (?, ?, ?, ?, ?, ?, ?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request.c_str(), -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: spawnEvent " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }
    sqlite3_bind_text(stmt, 1, gender, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, sport, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, countries, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, isteamsport, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, start, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, end, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, stadium, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: spawnEvent " << sqlite3_errmsg(db) << std::endl;
        //sqlite3_close(db);
        return 1;
    }
    //std::cout << "Data inserted successfully." << std::endl;
    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement
    return 0;
}

const int setPremium(const char* email, const bool isPremium, sqlite3* db)//Accorde ou enlève les droits premiums à un utilisateur
{
    const char* request = "UPDATE User SET ispremium = (?) WHERE email=(?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: updateCode " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    const char* premstr=isPremium?"Yes":"No";

    sqlite3_bind_text(stmt, 1, premstr, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: updateCode " << sqlite3_errmsg(db) << std::endl;
        //sqlite3_close(db);
        return 1;
    }

    std::cout << "Data updated successfully." << std::endl;

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

const int setBalance(const char* email, const int balance, sqlite3* db)//Set la valeur du solde d'un utilisateur
{
    const char* request = "UPDATE User SET balance = (?) WHERE email=(?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: updateBalance " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    std::string balancestr=std::to_string(balance);

    sqlite3_bind_text(stmt, 1, balancestr.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: updateBalance " << sqlite3_errmsg(db) << std::endl;
        //sqlite3_close(db);
        return 1;
    }

    std::cout << "Data updated successfully." << std::endl;

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}

const int addUserBet(const char* username, const char* amount, const char* betid, sqlite3* db)
{
    std::string request = "INSERT INTO userbet (username, betamount, betid) VALUES (?, ?, ?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request.c_str(), -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: addUserBet " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, amount, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, betid, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: addUserBet " << sqlite3_errmsg(db) << std::endl;
        //sqlite3_close(db);
        return 1;
    }
    //std::cout << "Data inserted successfully." << std::endl;
    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement
    return 0;
}

const int addBetDispo(const char* eventid, const char* choice, const char* cote, const char* closetime, sqlite3* db)
{
    std::string request = "INSERT INTO betdispo (eventid, choice, cote, closetime) VALUES (?, ?, ?, ?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request.c_str(), -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: addBetDispo " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }
    sqlite3_bind_text(stmt, 1, eventid, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, choice, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, cote, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, closetime, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: addBetDispo " << sqlite3_errmsg(db) << std::endl;
        //sqlite3_close(db);
        return 1;
    }
    //std::cout << "Data inserted successfully." << std::endl;
    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement
    return 0;
}

std::vector<Json::Value> getBetDispo(sqlite3* db)
{
    const char* request = "SELECT * FROM betdispo";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: compareCode " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
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
    // i{
    //     std::cerr << "Execution failed: compareCode " << sqlite3_errmsg(db) << std::endl;
    //     exit(1);
    // }

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return returnVec;
}

/*const int generateBetDispo(sqlite3* db)
{
    const char* request = "SELECT * FROM User WHERE email = (?)";

    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: getBetDispo " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return NULL;
    }
    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);//code de retour

    const char* hash;
    hash=(const char*)malloc(20*sizeof(hash));
    //const char hash[6] //priviliegier ca
    char salt[14];
    if (rc == SQLITE_ROW) {


        // const unsigned char* text = sqlite3_column_text(stmt, 0);
        // std::cout << "email"<<(text ? reinterpret_cast<const char*>(text) : "NULL1") << " ";
        hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)); //code est dans la colonne 4
        std::cout <<"hash : "<< (hash ? reinterpret_cast<const char*>(hash) : "NULLHash") << " "<<std::endl;


    } else if (rc == SQLITE_DONE) {
        std::cerr << "Execution failed: getSalt " << sqlite3_errmsg(db) << std::endl;
        //sqlite3_close(db);
        return NULL;
    }
    std::string H(hash);
    std::string S;
    S=H.substr(3,10); //On enleve $5$ et le $ d'apres
    // strncpy(salt, hash, 14);
    // salt[14] = '\0'; // Ensuring the string is null-terminated
    std::cout <<"salt : "<< (S.empty() ? "NULLSalt" : S) << " "<<std::endl;

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return S;
}*/

/*
int checklogin(const std::string mail)
{
    const char* request = "Select passwordH from User where email=";
    
    sqlite3_stmt *stmt;//objet requete
    int rc = sqlite3_prepare_v2(db, request, -1, &stmt, NULL);//code de retour
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    sqlite3_bind_text(stmt, 1, email, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);//code de retour
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    std::cout << "Data inserted successfully." << std::endl;

    sqlite3_finalize(stmt);//nettoie les ressources utilisées par la mise en place du statement

    return 0;
}*/

// g++ -o bdd bdd.cpp -lsqlite3 -lpthread -ljsoncpp
