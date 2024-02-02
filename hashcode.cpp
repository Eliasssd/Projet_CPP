#include <iostream>
#include <string>
#include <crypt.h>
#include <random>
#include "hashcode.h"
#include <vector>
#include <chrono>

std::string generateSaltString(const int len) {//genere un sel cryptographique utile pour créer un hash
    const std::string charlist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, charlist.size() - 1);//instancie une distribution uniforme sur des entiers

    std::string saltString;
    //on fait len tirages aleatoires dans la liste de caractères pour former un code
    for (int i = 0; i < len; ++i) {
        saltString += charlist[distribution(generator)];
    }

    return saltString;
}

std::string hash(const std::string password,const std::string salt,const bool genSalt=true ,const int saltLength=10,const std::string hashAlg="$5$")//genere un hash pour éviter de stocker le mot de passe en clair dans la base de donnée. On ajoute un sel pour éviter les attaques de dictionnaire
{
    //genSalt=true pour generer le premier hash avec le sel (utile pour sign up)
    //gensalt=false, on ne genere pas le sel, il faut donc le préciser dans le paramètre salt (utile pour login, le serveur nous envoie le salt a mettre)
    std::string saltString;
    if(genSalt)
    {
        saltString=generateSaltString(saltLength);
    }
    else//si on a deja le sel (utile pour sign in)
    {
        saltString=salt;
    }
    saltString=hashAlg+saltString;

    const std::string hash = crypt(password.c_str(), saltString.c_str());

    return hash;
}

const std::vector<float> generateCote(const int numberOfTeams)//Genere numberOfTeams cotes entre 1.1 et 2.1
{
    std::vector<float> cotes(numberOfTeams);
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<> distribution(1.1,2.1);//instancie une distribution uniforme sur des réels

    for(int i=0; i<numberOfTeams;i++)
    {
        cotes[i]=distribution(generator);
    }

    return cotes;
}

//g++ -o hc hashcode.cpp -lcrypt
