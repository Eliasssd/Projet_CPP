#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

class NetworkManager
{
    public:
        NetworkManager();

        static QString jsonTreatement(QNetworkReply *reply,const QString &param);
        static QMap<QString, QString> jsonTreatement(QNetworkReply *reply, const QVector<QString> &params);
        static QMap<QString, QJsonValue> jsonArrayTreatement(QNetworkReply *reply, const QVector<QString> &params);
        static QString SignUpRequest(const QString &mail ,const QString &username ,const QString &passwordH);
        static QString FA2_Request(const QString &FA2 ,const QString &email);
        static QString SaltRequest(const QString &Email);
        static QMap<QString, QString> SignInRequest(const QString &Email , const QString &PasswordH);
        static QMap<QString, QJsonValue> PublicInfoRequest();
};

#endif // NETWORKMANAGER_H
