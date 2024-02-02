#include "securitymanager.h"

#include <crypt.h>
#include <random>
#include <QRegularExpression>


SecurityManager::SecurityManager() {}

QString SecurityManager::generateSaltString(const size_t len)
{
    const QString charlist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, charlist.size() - 1);//instancie une distribution uniforme

    QString saltString;
    //on fait len tirages aleatoires dans la liste de caractères
    for (size_t i = 0; i < len; ++i)
    {
        saltString += charlist[distribution(generator)];
    }

    return saltString;
}

QString SecurityManager::hash(const QString &password)
{
    //genSalt=true pour generer le premier hash avec le sel (utile pour sign up)
    QString saltString = SecurityManager::generateSaltString(10);
    const QString hashAlg="$5$";

    saltString = hashAlg+saltString;

    const QString hash = crypt(password.toStdString().c_str(), saltString.toStdString().c_str());

    return hash;
}

QString SecurityManager::hash(const QString &password , const QString &salt)
{
    //genSalt=true pour generer le premier hash avec le sel (utile pour sign up)
    //gensalt=false, on ne genere pas le sel, il faut donc le préciser dans le paramètre salt
    QString saltString = salt;
    const QString hashAlg="$5$";

    saltString = hashAlg+saltString;

    const QString hash = crypt(password.toStdString().c_str(), saltString.toStdString().c_str());

    return hash;
}

bool SecurityManager::validEmail(const QString& email)
{
    static QRegularExpression regex("^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\\.[a-zA-Z0-9-.]+$");
    return regex.match(email).hasMatch();
}

bool SecurityManager::validPassword(const QString& password ,  QLabel* errorLabel)
{
    if (password.length() >= 10)
    {
        if (SecurityManager::AtLeastUpperCar(password , errorLabel))
        {
            if (SecurityManager::AtLeastLowerCar(password , errorLabel))
            {
                if (SecurityManager::AtLeastNumber(password , errorLabel))
                {
                    if (SecurityManager::AtLeastSpecialcar(password , errorLabel))
                    {
                        errorLabel->hide();
                        return true;
                    }
                    return false;
                }
                return false;
            }
            return false;
        }
        return false;
    }
    errorLabel->setText("Password should be at least 10");
    errorLabel->show();
    return false;
}

bool SecurityManager::AtLeastUpperCar(const QString& password ,  QLabel* errorLabel)
{
    foreach (QChar car, password)
    {
        if (car.isUpper())
        {
            errorLabel->hide();
            return true; // Dès qu'une majuscule est trouvée, retourner true
        }
    }
    errorLabel->setText("Password should has at least 1 maj");
    errorLabel->show();
    return false; // Aucune majuscule n'a été trouvée
}

bool SecurityManager::AtLeastLowerCar(const QString& password ,  QLabel* errorLabel)
{
    foreach (QChar car, password)
    {
        if (car.isLower())
        {
            errorLabel->hide();
            return true; // Dès qu'une miniscule est trouvée, retourner true
        }
    }
    errorLabel->setText("Password should has at least 1 min");
    errorLabel->show();
    return false; // Aucune miniscule n'a été trouvée
}

bool SecurityManager::AtLeastNumber(const QString& password ,  QLabel* errorLabel)
{
    foreach (QChar car, password)
    {
        if (car.isDigit())
        {
            errorLabel->hide();
            return true; // Dès qu'un chiffre est trouvé, retourner true
        }
    }
    errorLabel->setText("Password should has at least 1 number");
    errorLabel->show();
    return false; // Aucun chiffre n'a été trouvé
}

bool SecurityManager::AtLeastSpecialcar(const QString& password ,  QLabel* errorLabel)
{
    // Liste des caractères spéciaux autorisés
    QString autorised = "_$&!?.#";

    foreach (QChar car, password)
    {
        if (autorised.contains(car))
        {
            errorLabel->hide();
            return true; // Si un caractère non autorisé est trouvé, retourner false
        }
    }
    errorLabel->setText("Password should has at least 1 special car (_$&!?.#)");
    errorLabel->show();
    return false;
}
