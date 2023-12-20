#include <iostream>
#include <string>
#include <crypt.h>
#include <random>

std::string generateSaltString(const int len) {
    const std::string charlist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, charlist.size() - 1);//instancie une distribution uniforme

    std::string saltString;
    //on fait len tirages aleatoires dans la liste de caractères
    for (size_t i = 0; i < len; ++i) {
        saltString += charlist[distribution(generator)];
    }

    return saltString;
}

std::string hash(const std::string password,const std::string salt,const bool genSalt=true ,const int saltLength=10,const std::string hashAlg="$5$")
{
    //genSalt=true pour generer le premier hash avec le sel (utile pour sign up)
    //gensalt=false, on ne genere pas le sel, il faut donc le préciser dans le paramètre salt
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


int main() {
    // std::string password = "myPassword";
    // const int saltLength=10;//utile pour la protection contre les attaques de dictionnaire
    // const std::string hashAlg="$5$"; // le préfixe $5$ indique un hash sha-256
    //std::string salt = "$5$SomeSalt$";
    //std::string saltString=generateSaltString(saltLength);
    // saltString=hashAlg+saltString;
    // std::cout<<saltString<<std::endl;

    // const std::string hash = crypt(password.c_str(), saltString.c_str());

    // std::cout << "Hash : " << hash << std::endl;
    const std::string alreadyCalculatedSalt="p3rDwR6Zz8";
    std::cout<<"Hash : "<<hash("password",alreadyCalculatedSalt,false)<<std::endl;

    return 0;
}

//g++ -o hc hashcode.cpp -lcrypt
