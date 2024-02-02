#include "networkmanager.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <QCoreApplication>


NetworkManager::NetworkManager() {}

QString NetworkManager::jsonTreatement(QNetworkReply *reply,const QString &param)
{
    // Vérifier d'abord si la réponse est valide
    if (!reply)
    {
        qDebug() << "La réponse du réseau est nulle.";
        return "error";
    }

    // Lire la réponse et vérifier si elle est non vide
    QByteArray responseData = reply->readAll();
    if (responseData.isEmpty())
    {
        qDebug() << "La réponse du réseau est vide.";
        return "error";
    }

    // Essayer de convertir la réponse en document JSON
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    if (jsonResponse.isNull() || !jsonResponse.isObject())
    {
        qDebug() << "Erreur de parsing JSON ou la réponse n'est pas un objet JSON.";
        qDebug() << "Réponse brute:" << responseData;
        return "no";
    }

    // Accéder à l'objet JSON
    QJsonObject jsonObject = jsonResponse.object();

    // Vérifier si le paramètre spécifié est présent
    if (!jsonObject.contains(param))
    {
        qDebug() << "Le paramètre spécifié n'est pas trouvé:" << param;
        return "no";
    }

    // Vérifier si le paramètre est un booléen et le traiter
    if (jsonObject[param].isBool())
    {
        bool value = jsonObject[param].toBool();
        qDebug() << "Valeur de" << param << "(bool):" << value;
        return value ? "true" : "false";
    }
    else
    {
        return jsonObject[param].toString();
    }

    // Obtenir la valeur et la retourner
    QString value = jsonObject[param].toString();
    qDebug() << "Valeur de" << param << ":" << value;
    return value;
}

QMap<QString, QString> NetworkManager::jsonTreatement(QNetworkReply *reply, const QVector<QString> &params)
{
    QMap<QString, QString> results;

    if (!reply)
    {
        qDebug() << "La réponse du réseau est nulle.";
        results.insert("error", "La réponse du réseau est nulle.");
        return results;
    }

    QByteArray responseData = reply->readAll();
    if (responseData.isEmpty())
    {
        qDebug() << "La réponse du réseau est vide.";
        results.insert("error", "La réponse du réseau est vide.");
        return results;
    }

    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    if (jsonResponse.isNull() || !jsonResponse.isObject())
    {
        qDebug() << "Erreur de parsing JSON ou la réponse n'est pas un objet JSON.";
        qDebug() << "Réponse brute:" << responseData;
        results.insert("error", "Erreur de parsing JSON ou la réponse n'est pas un objet JSON.");
        return results;
    }

    QJsonObject jsonObject = jsonResponse.object();

    for (const QString &param : params)
    {
        if (!jsonObject.contains(param))
        {
            qDebug() << "Le paramètre spécifié n'est pas trouvé:" << param;
            results.insert(param, "no");
            continue;
        }

        if (jsonObject[param].isBool())
        {
            results.insert(param, jsonObject[param].toBool() ? "true" : "false");
        }
        else if (jsonObject[param].isString())
        {
            results.insert(param, jsonObject[param].toString());
        }
        else
        {
            // Vous pouvez étendre cette logique pour gérer d'autres types si nécessaire.
            results.insert(param, jsonObject[param].toVariant().toString());
        }
    }

    return results;
}

QMap<QString, QJsonValue> NetworkManager::jsonArrayTreatement(QNetworkReply *reply, const QVector<QString> &params)
{
    QMap<QString, QJsonValue> results;

    if (!reply)
    {
        qDebug() << "La réponse du réseau est nulle.";
        // Vous pouvez choisir de représenter une erreur avec un QJsonValue approprié
        return results;
    }

    QByteArray responseData = reply->readAll();
    if (responseData.isEmpty())
    {
        qDebug() << "La réponse du réseau est vide.";
        // Vous pouvez choisir de représenter une erreur avec un QJsonValue approprié
        return results;
    }

    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    if (jsonResponse.isNull() || !jsonResponse.isObject())
    {
        qDebug() << "Erreur de parsing JSON ou la réponse n'est pas un objet JSON.";
        qDebug() << "Réponse brute:" << responseData;
        // Vous pouvez choisir de représenter une erreur avec un QJsonValue approprié
        return results;
    }

    QJsonObject jsonObject = jsonResponse.object();

    for (const QString &param : params)
    {
        QJsonValue value = jsonObject.value(param);
        if (value.isUndefined())
        {
            qDebug() << "Le paramètre spécifié n'est pas trouvé:" << param;
            // Vous pouvez choisir de représenter une absence de valeur avec un QJsonValue approprié
            results.insert(param, QJsonValue::Undefined);
        }
        else
        {
            // Insérez la QJsonValue directement dans les résultats
            results.insert(param, value);
        }
    }

    return results;
}


QString NetworkManager::SignUpRequest(const QString &mail ,const QString &username ,const QString &passwordH)
{
    // Créer un gestionnaire d'accès réseau
    QNetworkAccessManager manager;

    // Créer une requête
    QNetworkRequest request;
    request.setUrl(QUrl("http://132.227.122.89:18080/api/signup"));

    // Créer des données JSON
    QJsonObject jsonData;
    jsonData["mail"] = mail;
    jsonData["username"] = username;
    jsonData["passwordH"] = passwordH;

    // Convertir les données JSON en QByteArray
    QJsonDocument jsonDoc(jsonData);
    QByteArray postData = jsonDoc.toJson();

    // Spécifier le type de contenu dans l'en-tête de la requête
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Envoyer la requête POST
    QNetworkReply *reply = manager.post(request, postData);

    // Attendre la réponse (synchronisation)
    while (!reply->isFinished()) {
        qApp->processEvents();
    }

    // Vérifier les erreurs
    if (reply->error() == QNetworkReply::NoError)
    {
        // Lire la réponse
        QString response = NetworkManager::jsonTreatement(reply , "code");
        // Nettoyer
        reply->deleteLater();

        return response;
    } else
    {
        // Gérer les erreurs
        qDebug() << "Erreur:" << reply->errorString();
        // Nettoyer
        reply->deleteLater();
        return "erreur";
    }
}

QString NetworkManager::FA2_Request(const QString &FA2 , const QString& email)
{
    // Créer un gestionnaire d'accès réseau
    QNetworkAccessManager manager;

    // Créer une requête
    QNetworkRequest request;
    request.setUrl(QUrl("http://132.227.122.89:18080/api/2fa"));

    // Créer des données JSON
    QJsonObject jsonData;
    jsonData["mail"] = email;
    jsonData["code"] = FA2;

    // Convertir les données JSON en QByteArray
    QJsonDocument jsonDoc(jsonData);
    QByteArray postData = jsonDoc.toJson();

    // Spécifier le type de contenu dans l'en-tête de la requête
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Envoyer la requête POST
    QNetworkReply *reply = manager.post(request, postData);

    // Attendre la réponse (synchronisation)
    while (!reply->isFinished()) {
        qApp->processEvents();
    }

    // Vérifier les erreurs
    if (reply->error() == QNetworkReply::NoError)
    {
        // Lire la réponse
        QString response = NetworkManager::jsonTreatement(reply , "code");
        // Nettoyer
        reply->deleteLater();

        return response;
    } else
    {
        // Gérer les erreurs
        qDebug() << "Erreur:" << reply->errorString();
        // Nettoyer
        reply->deleteLater();
        return "erreur";
    }
}

QString NetworkManager::SaltRequest(const QString &Email)
{
    // Créer un gestionnaire d'accès réseau
    QNetworkAccessManager manager;

    // Créer une requête
    QNetworkRequest request;
    request.setUrl(QUrl("http://132.227.122.89:18080/api/signin/salt"));

    // Créer des données JSON
    QJsonObject jsonData;
    jsonData["mail"] = Email;

    // Convertir les données JSON en QByteArray
    QJsonDocument jsonDoc(jsonData);
    QByteArray postData = jsonDoc.toJson();

    // Spécifier le type de contenu dans l'en-tête de la requête
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Envoyer la requête POST
    QNetworkReply *reply = manager.post(request, postData);

    // Attendre la réponse (synchronisation)
    while (!reply->isFinished())
    {
        qApp->processEvents();
    }

    // Vérifier les erreurs
    if (reply->error() == QNetworkReply::NoError)
    {
        // Lire la réponse
        QVector<QString> params = {"code", "salt"};
        QMap<QString, QString> results = NetworkManager::jsonTreatement(reply , params);
        QString response;
        if (results["code"] == "true")
        {
            response = results["salt"];
        }
        else
        {
            response = "false";
        }
        // Nettoyer
        reply->deleteLater();

        return response;
    } else
    {
        // Gérer les erreurs
        qDebug() << "Erreur:" << reply->errorString();
        // Nettoyer
        reply->deleteLater();
        return "false";
    }
}

QMap<QString, QString> NetworkManager::SignInRequest(const QString &Email , const QString &PasswordH)
{
    // Créer un gestionnaire d'accès réseau
    QNetworkAccessManager manager;

    // Créer une requête
    QNetworkRequest request;
    request.setUrl(QUrl("http://132.227.122.89:18080/api/signin"));

    // Créer des données JSON
    QJsonObject jsonData;
    jsonData["mail"] = Email;
    jsonData["passwordH"] = PasswordH;

    // Convertir les données JSON en QByteArray
    QJsonDocument jsonDoc(jsonData);
    QByteArray postData = jsonDoc.toJson();

    // Spécifier le type de contenu dans l'en-tête de la requête
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Envoyer la requête POST
    QNetworkReply *reply = manager.post(request, postData);

    // Attendre la réponse (synchronisation)
    while (!reply->isFinished())
    {
        qApp->processEvents();
    }

    // Vérifier les erreurs
    if (reply->error() == QNetworkReply::NoError)
    {
        // Lire la réponse
        QVector<QString> params = {"code" , "username" , "ispremium" , "balance"};
        QMap<QString, QString> response = NetworkManager::jsonTreatement(reply , params);
        // Nettoyer
        reply->deleteLater();

        return response;
    } else
    {
        // Gérer les erreurs
        qDebug() << "Erreur dans else:" << reply->errorString();
        // Nettoyer
        reply->deleteLater();
        QMap<QString, QString> response;
        response.insert("code", "false");
        return response;
    }
}

QMap<QString, QJsonValue> NetworkManager::PublicInfoRequest()
{
    // Créer un gestionnaire d'accès réseau
    QNetworkAccessManager manager;

    // Créer une requête
    QNetworkRequest request;
    request.setUrl(QUrl("http://132.227.122.89:18080/api/publicinfo"));

    // Créer des données JSON
    QJsonObject jsonData;
    jsonData["mail"] = "salut";

    // Convertir les données JSON en QByteArray
    QJsonDocument jsonDoc(jsonData);
    QByteArray postData = jsonDoc.toJson();

    // Spécifier le type de contenu dans l'en-tête de la requête
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Envoyer la requête POST
    QNetworkReply *reply = manager.post(request, postData);
    // Attendre la réponse (synchronisation)
    while (!reply->isFinished())
    {
        qApp->processEvents();
    }

    // Vérifier les erreurs
    if (reply->error() == QNetworkReply::NoError)
    {
        // Lire la réponse
        QVector<QString> params = {"betdispo" , "events"};
        QMap<QString, QJsonValue> response = NetworkManager::jsonArrayTreatement(reply , params);
        // Nettoyer
        reply->deleteLater();

        return response;
    } else
    {
        // Gérer les erreurs
        qDebug() << "Erreur dans else:" << reply->errorString();
        // Nettoyer
        reply->deleteLater();
        QMap<QString, QJsonValue> response;
        response.insert("code", "false");
        return response;
    }
}

// QMap<QString, QString> NetworkManager::eventsinfo(QString events)
// {

// }

// QMap<QString, QString> NetworkManager::betsinfo()
// {

// }



