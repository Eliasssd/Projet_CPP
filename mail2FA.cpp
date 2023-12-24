#include <cstdlib>
#include <iostream>
#include <string>
#include <random>
#include <fstream>

int sendmail(const std::string receiverMail)//fonction qui envoie un mail 2FA a l'utilisateur
{

    //generation du code pour la verification deux facteurs (2FA)
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0,9);//instancie une distribution uniforme
    std::string randomCode;
    for(int i=0;i<6;i++)
    {
        randomCode += std::to_string(distribution(generator));
    }

    //creation du fichier de texte du mail utile pour la requete curl
    std::string from = "From: OlympicStake\n";
    std::string object = "Subject: 2FA Verification code";
    std::string body = "Enter this code in the OlympicStake app to login : "+randomCode;
    std::string content = from+object+"\r\n\r\n"+body;

    std::string tempFileName = "email:"+randomCode+".txt";
    std::ofstream tempFile(tempFileName);
    //on met le contenu du fichier dedans
    tempFile << content;
    tempFile.close();

    //commade pour envoyer un mail avec curl
    const std::string cmd= "curl --ssl-reqd --url 'smtps://smtp.gmail.com:465' --user 'olympicstake@gmail.com:tonv ahnz qncn jcak' --mail-from 'olympicstake@gmail.com' --mail-rcpt '"+receiverMail+"' --upload-file "+tempFileName;
    
    //execution de la commande system
    int exec = system(cmd.c_str());
    std::cout<<exec<<std::endl;

    //on supprime le fichier
    std::remove(tempFileName.c_str());

    //retour de l'execution de la commande (voir si elle a fonctionné) (0 si pas de probleme)
    if (!exec) {
        std::cout << "done" << std::endl;
    } else {
        std::cout << "problem" << std::endl;
        return 1;
    }

    return 0;
}

//test
int main() {

    const std::string receiver="ap.awa.awo@gmail.com";

    sendmail(receiver);

    return 0;
}