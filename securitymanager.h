#ifndef SECURITYMANAGER_H
#define SECURITYMANAGER_H

#include <QString>
#include <QLabel>


class SecurityManager
{
    public:
        SecurityManager();

        static QString generateSaltString(const size_t len);
        static QString hash(const QString &password);
        static QString hash(const QString &password , const QString &salt);

        static bool validEmail(const QString& email);
        static bool validPassword(const QString& password ,  QLabel* errorLabel);
        static bool AtLeastUpperCar(const QString& password ,  QLabel* errorLabel);
        static bool AtLeastLowerCar(const QString& password ,  QLabel* errorLabel);
        static bool AtLeastNumber(const QString& password ,  QLabel* errorLabel);
        static bool AtLeastSpecialcar(const QString& password ,  QLabel* errorLabel);

};

#endif // SECURITYMANAGER_H
