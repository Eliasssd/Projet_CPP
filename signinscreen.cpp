#include "signinscreen.h"
#include "securitymanager.h"
#include "networkmanager.h"
#include "event.h"
#include "footevent.h"
#include "basketevent.h"
#include "globaldata.h"


#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

SignInScreen::SignInScreen(QWidget *parent)
    : QWidget{parent}
{

    QLabel *waveLabel = new QLabel(this);
    QPixmap wavePixmap(":/ressources/olympicstake.svg"); // Remplacez par le chemin d'accès à votre image de vague
    waveLabel->setPixmap(wavePixmap);
    waveLabel->setScaledContents(true); // Pour que l'image s'adapte à la taille du QLabel

    // Créez un layout vertical pour organiser les widgets
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    // Ajoutez un espace élastique en haut pour pousser le bouton vers le bas
    // verticalLayout->addStretch();
    verticalLayout->addWidget(waveLabel);

    // Créez des zones d'entrée de texte et des libellés
    EmailLineEdit = new QLineEdit(this);
    EmailLineEdit->setStyleSheet(
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 10px 15px;" /* Espace intérieur */
        "border: none;" /* Pas de bordure */
        "text-align: center;"
        // "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));"
        "}");

    EmailLabel = new QLabel("Mail", this);
    EmailLabel->setStyleSheet(
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 10px 50px;" /* Espace intérieur */
        "border: none;" /* Pas de bordure */
        "text-align: center;"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));"
        "}");

    // Créez un QLabel pour le message d'erreur
    EmailErrorLabel = new QLabel(this);
    EmailErrorLabel->setStyleSheet("color: rgb(255, 0, 0);");
    EmailErrorLabel->hide();

    // Ajoutez des éléments à nouvelEcran selon vos besoins
    PasswordLineEdit = new QLineEdit(this);
    PasswordLineEdit->setStyleSheet(
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 10px 15px;" /* Espace intérieur */
        "border: none;" /* Pas de bordure */
        "text-align: center;"
        // "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));"
        "}");
    PasswordLabel = new QLabel("Password", this);
    PasswordLabel->setStyleSheet(
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 10px 25px;" /* Espace intérieur */
        "border: none;" /* Pas de bordure */
        "text-align: center;"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));"
        "}");

    // Créez un QLabel pour le message d'erreur
    PasswordErrorLabel = new QLabel(this);
    PasswordErrorLabel->setStyleSheet("color: rgb(255, 0, 0);");
    PasswordErrorLabel->hide();

    Submit = new QPushButton("Submit", this);
    Submit->setStyleSheet(
        "QPushButton {"
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 15px 100px;" /* Espace intérieur */
        "border: none;" /* Pas de bordure */
        "text-align: center;"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));"
        "}"

        "QPushButton:hover {"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 100, 100, 255), stop:1 rgba(255, 255, 100, 255));"
        "}"

        "QPushButton:pressed {"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(200, 70, 70, 255), stop:1 rgba(200, 200, 0, 255));"
        "}");

    connect(Submit, &QPushButton::clicked,this, [=]()
            {
                // Appeler sendMailRequest avec la chaîne nécessaire en tant que paramètre
                // Connectez le clic du bouton OK à une fonction qui récupère les entrées
                //SLOT(accept());

                // MAJ 12/01
                // vérifier que le mail est valide
                QString mail = EmailLineEdit->text();
                if (SecurityManager::validEmail(mail))
                {
                    EmailErrorLabel->hide();
                    // vérifier que le mot de passe est valide
                    QString salt = NetworkManager::SaltRequest(mail);
                    if (salt != "false")
                    {
                        QString password = PasswordLineEdit->text();
                        QString hash = SecurityManager::hash(password , salt);
                        QMap<QString, QString> response = NetworkManager::SignInRequest(mail , hash);
                        qDebug() << "code" <<response["code"];
                        if (response["code"] == "true")
                        {
                            QMap<QString, QJsonValue> result = NetworkManager::PublicInfoRequest();
                            // qDebug() << "code" <<response["code"];
                            qDebug() << "username" <<response["username"];
                            QString username = response["username"];
                            qDebug() << "balance" <<response["balance"];
                            qDebug() << "ispremium" <<response["ispremium"];
                            // Set the email
                            GlobalData::instance().setInfo(mail , username);

                            QString test1 = GlobalData::instance().getEmail();
                            QString test2 = GlobalData::instance().getUsername();
                            qDebug() << "mail" << test1;
                            qDebug() << "username" << test2;
                            // QVector<QString> params = {"country1" , "country2" , "end" , "gender" ,"id" , "sport" , "stadium" , "start"};
                            QVector<QString> params = {"events"};// , "betdispo"};
                            QList<Event*> events;
                            for (const QString &param : params) {
                                QJsonValue value = result.value(param);
                                if (value.isArray()) {
                                    QJsonArray array = value.toArray();
                                    for (const QJsonValue &element : array)
                                    {
                                        if (element.isObject())
                                        {
                                            QJsonObject obj = element.toObject();
                                            int id = obj["id"].toInt();
                                            QString location = obj["location"].toString();
                                            QDateTime start = QDateTime::fromString(obj["start"].toString(), Qt::ISODate);
                                            QDateTime end = QDateTime::fromString(obj["end"].toString(), Qt::ISODate);
                                            QString sport = obj["sport"].toString();
                                            QString gender = obj["gender"].toString(); // Non utilisé dans cet exemple

                                            if (sport == "Football") {
                                                QString team1 = obj["team1"].toString();
                                                QString team2 = obj["team2"].toString();
                                                events.append(new FootEvent(id,gender, start, end,location, team1, team2));
                                            } else if (sport == "Basketball") {
                                                QString team1 = obj["team1"].toString();
                                                QString team2 = obj["team2"].toString();
                                                events.append(new BasketEvent(id,gender, start, end,location, team1, team2));
                                            }
                                        }
                                    }
                                }
                            }

                            // qDebug() << "events" <<result["events"];
                            // qDebug() << "betdispo" <<result["betdispo"];
                            emit switchToWelcome();
                            // Utilisez QTimer::singleShot pour introduire un retard
                            // QTimer::singleShot(5000, this, SIGNAL(switchToWelcome()));
                        }
                    }
                }
                else
                {
                    EmailErrorLabel->setText("E-mail address not valid.");
                    EmailErrorLabel->show();
                }

            });

    Return = new QPushButton("Return",this);
    Return->setStyleSheet(
        "QPushButton {"
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 15px 100px;" /* Espace intérieur */
        "border: none;" /* Pas de bordure */
        "text-align: center;"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));"
        "}"

        "QPushButton:hover {"
        /* Changez légèrement les couleurs pour l'effet hover, si vous le souhaitez */
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 100, 100, 255), stop:1 rgba(255, 255, 100, 255));"
        "}"

        "QPushButton:pressed {"
        /* Effet pour le bouton pressé (facultatif) */
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(200, 70, 70, 255), stop:1 rgba(200, 200, 0, 255));"
        "}");

    connect(Return, &QPushButton::clicked,this, [=]()
            {
                emit switchToHome();
            });

    // Placez le bouton au centre en bas
    verticalLayout->addWidget(EmailLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(EmailLineEdit, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(EmailErrorLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(PasswordLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(PasswordLineEdit, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(PasswordErrorLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(Submit, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(Return, 0, Qt::AlignHCenter | Qt::AlignBottom);

}
