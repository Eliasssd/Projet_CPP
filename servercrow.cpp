#include <iostream>
#include "crow.h"
#include "/usr/include/sqlite3.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "jsoncpp/json/json.h"
#include "hashcode.h"
#include "bdd.h"
#include "sendmail.h"
#include "datetime.h"

static int callback(void *NotUsed, int argc, char **argv, char **ColName)
{
  for(int i = 0; i < argc; i++)
  {
    std::cout << ColName[i] << " = " << argv[i] << std::endl;
  }

  std::cout << "\n";

  return 0;

}

/*std::string hexToStr(const std::string& hex)//Transforme une chaine hexadecimale en chaine lisible 
{
    std::string str;
    str.reserve(hex.length() / 2);

    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        char byte = static_cast<char>(std::stoul(byteString, nullptr, 16));
        str.push_back(byte);
    }

    return str;
}*/

int main() {

    crow::SimpleApp app;

    const int port=18080;

    sqlite3 *db;
    char *ErrMsg = 0;

    //open database
    int rc = sqlite3_open("Database.db", &db);

    if(rc)
    {
        std::cout << "Can't open database\n";
    } else {
        std::cout << "Open database successfully\n";
    }

    CROW_ROUTE(app, "/api/post").methods(crow::HTTPMethod::Post)([](const crow::request& req){
        crow::json::rvalue x = crow::json::load(req.body);//charge le corps de la requete post et on teste si elle est convenable
        if (!x)
            return crow::response(400, "Invalid JSON");//sinon on envoie un code d'erreur HTTP
        
        //std::cout<<req.body<<std::endl;//afficher la body (debogage)

        Json::Value obj;
        std::stringstream(req.body) >> obj;//on met req.body en objet JSON lisible par C++


        std::string value1 = obj["param1"].asString();//Lire une valeur et la mettre dans une variable pour l'utiliser
        std::cout<<"Value : "<<value1<<std::endl;
        
        // Create a response
        std::ostringstream os;
        os << "message de reponse" ;
        return crow::response(200,os.str());//on envoie un code de retour positif
    });

    CROW_ROUTE(app, "/api/signup").methods(crow::HTTPMethod::Post)([db](const crow::request& req){//On recoit les informations de creation de compte, on enregistre et on envoie un mail de 2FA
        crow::json::rvalue x = crow::json::load(req.body);//charge le corps de la requete post et on teste si elle est convenable
        if (!x)
            return crow::response(400, "Invalid JSON");//sinon on envoie un code d'erreur HTTP
        
        //std::cout<<req.body<<std::endl;//afficher la body (debogage)

        Json::Value obj;
        std::stringstream(req.body) >> obj;//on met req.body en objet JSON lisible par C++


        std::string email = obj["mail"].asString();//Lire une valeur et la mettre dans une variable pour l'utiliser
        std::string username = obj["username"].asString();
        std::string passwordH = obj["passwordH"].asString();
        std::cout<<"mail : "<<email<<" username : "<<username<<" passwordH : "<<passwordH<<std::endl;
        
        
        //int rco=addUser(email.c_str(),username.c_str(),passwordH.c_str(),db,"User");//return 1 si erreur, 0 si ok
        int rco=checkUser(email.c_str(), username.c_str(),db);
        //rco est le return code mettre un place un systeme pour savoir si le mail ou l'username existe deja
        if(rco==0)
        {
            int rct = addUser(email.c_str(),username.c_str(),passwordH.c_str(),db,"Usertemp");//ajoute les infos dans la base User temporaire en attendant que le code 2FA soit vérifié
            if(rct!=0){return crow::response(400,"{\"code\":false}");}
            const std::string randomCode=genCode(6);//On envoie le mail de 2FA
            int rcmail=sendmail(email,randomCode);//0 si mail envoyé avec succès, 1 sinon
            if(rcmail==0)
            {
                //ajouter le random code a la base de donnée en attente de vérification
                int rco2=updateCode(email.c_str(),randomCode.c_str(),db);//0 si base de donnée bien modifiée, 1 sinon
                if(rco2!=0){return crow::response(400,"{\"code\":false}");}
            }
            else
            {
                return crow::response(400,"{\"code\":false}");
            }
        }
        else if(rco==1)
        {
            return crow::response(400,"{\"code\":false}");//erreur de doublon 
        }
        else if(rco==2)
        {
            return crow::response(400,"{\"code\":false}");//erreur de doublon EMAIL
        }
        else if(rco==3)
        {
            return crow::response(400,"{\"code\":username}");//erreur de doublon USERNAME
        }
        return crow::response(200,"{\"code\":true}");//on envoie un code de retour positif
    });

    CROW_ROUTE(app, "/api/2fa").methods(crow::HTTPMethod::Post)([db](const crow::request& req){
        crow::json::rvalue x = crow::json::load(req.body);//charge le corps de la requete post et on teste si elle est convenable
        if (!x)
            return crow::response(400, "Invalid JSON");//sinon on envoie un code d'erreur HTTP
        
        //std::cout<<req.body<<std::endl;//afficher la body (debogage)

        Json::Value obj;
        std::stringstream(req.body) >> obj;//on met req.body en objet JSON lisible par C++


        std::string email = obj["mail"].asString();//Lire une valeur et la mettre dans une variable pour l'utiliser
        std::string code = obj["code"].asString();
        std::cout<<"mail : "<<email<<" code : "<<code<<std::endl;
        
        int rc=compareCode(email.c_str(),code.c_str(),db,true);
        std::cout<<rc<<std::endl;
        if(rc!=0){
            std::cout<<"False 2FA"<<std::endl;
            return crow::response(200,"{\"code\":false}");
        }
        std::cout<<"true 2FA"<<std::endl;
        //rc==0 donc il faut ajouter les infos de l'user a la vraie database
        return crow::response(200,"{\"code\":true}");//on retourne la validation
    });
    
    CROW_ROUTE(app, "/api/signin/salt").methods(crow::HTTPMethod::Post)([db](const crow::request& req){//on recoit email et on envoie le salt du mot de passe si le mail est trouvé
        crow::json::rvalue x = crow::json::load(req.body);//charge le corps de la requete post et on teste si elle est convenable
        if (!x)
            return crow::response(400, "Invalid JSON");//sinon on envoie un code d'erreur HTTP
        
        //std::cout<<req.body<<std::endl;//afficher la body (debogage)

        Json::Value obj;
        std::stringstream(req.body) >> obj;//on met req.body en objet JSON lisible par C++


        std::string email = obj["mail"].asString();//Lire une valeur et la mettre dans une variable pour l'utiliser
        std::cout<<"mail : "<<email<<std::endl;
        
        std::string salt=getSalt(email.c_str(),db);
        if(salt.empty())
        {
            std::cout<<"No salt"<<std::endl;
            return crow::response(200,"{\"code\":true,\"salt\":\"nosalt\"");
        }
        //return crow::response(200,"{\"code\":true,\"salt\":"+salt+"}");//on envoie le code vrai ou faux et le salt
        std::cout<<"Salt okay"<<std::endl;
        return crow::response(200,"{\"code\":true,\"salt\":\""+salt+"\"}");
    });

    CROW_ROUTE(app, "/api/signin").methods(crow::HTTPMethod::Post)([db](const crow::request& req){//on recoit email et password H et on compare le passwordH et on renvoie une autorisation ou non de connexion
        crow::json::rvalue x = crow::json::load(req.body);//charge le corps de la requete post et on teste si elle est convenable
        if (!x)
            return crow::response(400, "Invalid JSON");//sinon on envoie un code d'erreur HTTP
        
        //std::cout<<req.body<<std::endl;//afficher la body (debogage)

        Json::Value obj;
        std::stringstream(req.body) >> obj;//on met req.body en objet JSON lisible par C++


        std::string email = obj["mail"].asString();//Lire une valeur et la mettre dans une variable pour l'utiliser
        std::cout<<"mail : "<<email<<std::endl;
        

        
        //return crow::response(200,"a");//on envoie le code (pour tester)
        return crow::response(200,"{\"code\":true,\"salt\":\"wwwwwwww\"}");
    });











    CROW_ROUTE(app, "/api/mail").methods(crow::HTTPMethod::Post)([](const crow::request& req){
        crow::json::rvalue x = crow::json::load(req.body);//charge le corps de la requete post et on teste si elle est convenable
        if (!x)
            return crow::response(400, "Invalid JSON");//sinon on envoie un code d'erreur HTTP
        
        //std::cout<<req.body<<std::endl;//afficher la body (debogage)

        Json::Value obj;
        std::stringstream(req.body) >> obj;//on met req.body en objet JSON lisible par C++


        std::string email = obj["mail"].asString();//Lire une valeur et la mettre dans une variable pour l'utiliser
        std::string username = obj["username"].asString();
        std::cout<<"mail : "<<email<<" username : "<<username<<std::endl;
        
        const std::string randomCode=genCode(6);
        sendmail(email,randomCode);
        
        return crow::response(200,"{code:"+randomCode+"}");//on envoie le code (pour tester)
    });

    std::cout<<"Listening on port : "<<port<<std::endl;
    app.port(port).multithreaded().run();//serveur à l'écoute en gestion de requete multithreading

     if(db)
    {
            sqlite3_close(db);
    }

    return 0;
}

//g++ -o cs crowserver.cpp bdd.cpp sendmail.cpp -I/home/user18/ProjetCPP/crow/include -lsqlite3 -lboost_system -lpthread -ljsoncpp  //Commande de compilation
