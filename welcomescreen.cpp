#include "welcomescreen.h"
#include "networkmanager.h"
#include "footevent.h"
#include "basketevent.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

#include <QDebug>
#include <QString>


#include <QVBoxLayout>
#include <QScrollArea>
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>





#include <QVBoxLayout>
#include <QPushButton>

WelcomeScreen::WelcomeScreen(QWidget *parent)
    : QWidget{parent}
{
    QMap<QString,QString> footlogos = chargefootlogos();
    QMap<QString,QString> basketlogos = chargebasketlogos();

    QList<Event*> events = chargepublicinfo();
    QVBoxLayout *layout = new QVBoxLayout(this);

    QScrollArea *scrollArea = new QScrollArea(this);
    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);

    qDebug() << "avant for";
    int j = 1;
    for (int i = 1; i <= 10; i++)
    {
        qDebug() << "for i" << i;
        QFrame *rect = new QFrame();
        rect->setMinimumHeight(i % 4 == 0 ? 70 : 200); // hauteur plus petite pour chaque 4ème rectangle
        setrecStyle(rect);


        if (i % 4 != 0)
        { // Pour les rectangles qui ne sont pas multiples de 3
            Event* event;
            event = findEventById(events , j);

            QHBoxLayout *hLayout = new QHBoxLayout(rect);

            // Diviser le rectangle horizontalement en deux parties pour les images
            QVBoxLayout *leftLayout = new QVBoxLayout();
            QVBoxLayout *rightLayout = new QVBoxLayout();

            // Ajouter des espaces pour des images (remplacer par des QLabel avec des images)
            QLabel *leftImage = new QLabel("Image Gauche");
            leftImage->setStyleSheet("background-color: white; border: 1px solid black;");
            QLabel *rightImage = new QLabel("Image Droite");
            rightImage->setStyleSheet("background-color: white; border: 1px solid black;");

            // Ajouter l'image à chaque layout
            leftLayout->addWidget(leftImage);
            rightLayout->addWidget(rightImage);

            // Créer des boutons et les ajouter sous les images correspondantes
            QPushButton *leftButton = new QPushButton("Bet");
            QPushButton *rightButton = new QPushButton("Bet");

            setButtonStyle(leftButton);
            setButtonStyle(rightButton);

            leftLayout->addWidget(leftButton);
            rightLayout->addWidget(rightButton);

            // Ajouter les layouts verticaux à l'horizontal layout
            hLayout->addLayout(leftLayout, 1); // 1 représente la proportion de l'espace
            hLayout->addLayout(rightLayout, 1);
            qDebug() << "for i une" << i << "une deuxieme fois";
            // Appliquer le layout horizontal au QFrame
            rect->setLayout(hLayout);
            QString sport;
            if (event) {
                sport = event->getEventType();
                qDebug() << "for i une" << i << "une troisieme fois ca a marché";
                // Utilisez eventType ici
            } else {
                // Gérez le cas où event est null
            }
            qDebug() << "for i une" << i << "une troisieme fois";
            qDebug() << sport;
            QString team1;
            QString team2;
            QString path1;
            QString path2;
            if (sport == "Football")
            {
                FootEvent* footEvent = dynamic_cast<FootEvent*>(event);
                if (footEvent)
                {
                    team1 = footEvent->getTeamone();
                    path1 = ":/ressources/football/" + footlogos[team1] ;
                    team2 = footEvent->getTeamTwo();
                    path2 = ":/ressources/football/" + footlogos[team2] ;
                }
            }
            else if (sport == "BasketBall")
            {
                BasketEvent* basketEvent = dynamic_cast<BasketEvent*>(event);
                if (basketEvent)
                {
                    team1 = basketEvent->getTeamone();
                    path1 = ":/ressources/basketball/" + basketlogos[team1] ;
                    team2 = basketEvent->getTeamTwo();
                    path2 = ":/ressources/basketball/" + basketlogos[team2] ;
                }
            }
            // Charger l'image à partir du fichier de ressources
            QPixmap pixmap1(path1);
            QPixmap pixmap2(path2);
            leftImage->resize(100, 100);
            rightImage->resize(200,100);
            // Redimensionner l'image pour qu'elle corresponde à la taille du label
            QPixmap scaledPixmap1 = pixmap1.scaled(leftImage->size(), Qt::KeepAspectRatio);
            QPixmap scaledPixmap2 = pixmap2.scaled(rightImage->size(), Qt::KeepAspectRatio);

            leftImage->setAlignment(Qt::AlignCenter);
            rightImage->setAlignment(Qt::AlignCenter);
            leftImage->setPixmap(scaledPixmap1);
            rightImage->setPixmap(scaledPixmap2);
            //label->resize(pixmap.width(), pixmap.height());
            j++;
        }
        else
        {
            // Ceci est un rectangle "solo" où nous voulons afficher la publicité
            QVBoxLayout *adLayout = new QVBoxLayout(rect); // Créer un nouveau QVBoxLayout pour le rect de l'annonce

            QLabel *adLabel = new QLabel("Deposit 100€ and win another 100€ to Bet!");
            adLabel->setAlignment(Qt::AlignCenter);
            adLabel->setStyleSheet("QLabel {"
                                   "  color: black;"
                                   "  font-weight: bold;"
                                   "  font-size: 18px;"
                                   "  background-color: #FFA500;"
                                   "  padding: 15px;"
                                   "}");

            adLayout->addWidget(adLabel); // Ajouter l'annonce au layout de l'annonce
            rect->setLayout(adLayout); // Définir le layout de l'annonce comme le layout pour le rect de l'annonce
        }



        containerLayout->addWidget(rect);
        containerLayout->setSpacing(10); // espace entre les rectangles
    }

    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidgetResizable(true); // Permet au widget conteneur de redimensionner avec la scrollArea

    scrollArea->setWidget(container);
    layout->addWidget(scrollArea);
}

QList<Event*> WelcomeScreen::chargepublicinfo()
{
    QMap<QString, QJsonValue> result = NetworkManager::PublicInfoRequest();
    QVector<QString> params = {"events"};
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
    return events;
}

const QMap<QString , QString> WelcomeScreen::chargefootlogos()
{
    QMap<QString , QString> map;
    map.insert("Algeria", "Algeria.png");
    map.insert("Argentina", "Argentina.svg.png");
    map.insert("Brazil", "Brazil.svg.png");
    map.insert("Canada", "Canada.svg.png");
    map.insert("France", "France.svg.png");
    map.insert("Germany", "Germany.svg.png");
    map.insert("Italy", "Italy.svg.png");
    map.insert("Japan", "Japan.svg.png");
    map.insert("Spain", "Spain.svg.png");
    map.insert("USA", "USA.svg.png");
    return map;
}

const QMap<QString , QString> WelcomeScreen::chargebasketlogos()
{
    QMap<QString , QString> map;
    map.insert("Algeria", "Algeria.png");
    map.insert("Argentina", "Argentina.jpeg");
    map.insert("Brazil", "Brazil.png");
    map.insert("Canada", "Canada.svg.png");
    map.insert("France", "France.svg.png");
    map.insert("Germany", "Germany.jpg");
    map.insert("Italy", "Italy.png");
    map.insert("Japan", "Japan.png");
    map.insert("Spain", "Spain.png");
    map.insert("USA", "USA.png");
    return map;
}

void WelcomeScreen::setButtonStyle(QPushButton *button)
{
    button->setStyleSheet(
        "QPushButton {"
        "border-radius: 20px;"
        "font-size: 16px;"
        "color: white;"
        "padding: 5px 15px;"
        "border: none;"
        "text-align: center;"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));"
        "}"

        "QPushButton:hover {"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 100, 100, 255), stop:1 rgba(255, 255, 100, 255));"
        "}"

        "QPushButton:pressed {"

        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(200, 70, 70, 255), stop:1 rgba(200, 200, 0, 255));"
        "}");
}

void WelcomeScreen::setrecStyle(QFrame *rect)
{
    rect->setFrameShape(QFrame::Box);
    rect->setMaximumWidth(448);
    rect->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    rect->setStyleSheet("QFrame { background-color: #A9A9A9; border: 1px solid black; }");
}

// La fonction de recherche
Event* WelcomeScreen::findEventById(const QList<Event*>& events, int searchId)
{
    for (Event* event : events)
    {
        if (event && event->getEventID() == searchId)
        {
            qDebug() << "trouvé";
            qDebug() << "id = " << event->getEventID();
            return event;
        }
    }
    qDebug() << "trouvé";
    return nullptr; // Retourner nullptr si aucun événement correspondant n'est trouvé
}
