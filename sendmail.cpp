#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
#include <fstream>
#include "sendmail.h"
#include "servermanager.h"

const std::string genCode(const int len=6)//generation du code pour la verification deux facteurs (2FA)
{
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0,9);//instancie une distribution uniforme sur les chiffres
    std::string randomCode;
    for(int i=0;i<len;i++)
    {
        randomCode += std::to_string(distribution(generator));
    }
    return randomCode;
}

int sendmail(const std::string receiverMail,const std::string code)//fonction qui envoie un mail 2FA a l'utilisateur
{

    //creation du fichier de texte du mail utile pour la requete curl
    std::string from = "From: OlympicStake\n";
    std::string object = "Subject: 2FA Verification code";
    std::string body = "Enter this code in the OlympicStake app to login : "+code;
    std::string content = from+object+"\r\n\r\n"+body;

    std::string tempFileName = "email:"+code+".txt";
    std::ofstream tempFile(tempFileName);
    //on met le contenu du fichier dedans
    tempFile << content;
    tempFile.close();

    //commade pour envoyer un mail avec curl
    const std::string cmd= "curl --ssl-reqd --url 'smtps://smtp.gmail.com:465' --user 'olympicstake@gmail.com:tonv ahnz qncn jcak' --mail-from 'olympicstake@gmail.com' --mail-rcpt '"+receiverMail+"' --upload-file "+tempFileName;
    
    //execution de la commande system (appel de curl)
    int exec = system(cmd.c_str());
    //std::cout<<exec<<std::endl;

    std::remove(tempFileName.c_str());//supression du fichier temporaire pour ne pas encombrer le serveur

    //retour de l'execution de la commande (voir si elle a fonctionné) (0 si pas de probleme)
    if (!exec) {
        ServerManager::log(receiverMail+" : 2FA email sent");
    } else {
        ServerManager::errorlog(receiverMail+" : Failed to send 2FA email");
        return 1;
    }

    return 0;
}
