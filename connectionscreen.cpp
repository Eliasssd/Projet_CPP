#include "connectionscreen.h"
//#include "mainwindow.h"

ConnectionScreen::ConnectionScreen(QWidget *parent)
    : QWidget{parent}
{
    QLabel *waveLabel = new QLabel(this);
    QPixmap wavePixmap(":/ressources/olympicstake.svg"); // Remplacez par le chemin d'accès à votre image de vague
    waveLabel->setPixmap(wavePixmap);
    waveLabel->setScaledContents(true); // Pour que l'image s'adapte à la taille du QLabel

    // // Définissez l'image de vague comme fond pour le widget principal ou un autre widget
    // this->setStyleSheet("QWidget {"
    //                     "background-image: url(:/ressources/waves.svg);"
    //                     "background-repeat: no-repeat;"
    //                     "background-position: top;"
    //                     "}");

    // Créez un layout vertical pour organiser les widgets
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    // verticalLayout->addLayout(topLayout);
    verticalLayout->addWidget(waveLabel); // Ajoutez-le en premier pour qu'il soit en haut
    // Ajoutez le reste de vos widgets au layout
    verticalLayout->addStretch();

    // Créer un QLabel pour le logo central
    // QLabel *centerLogoLabel = new QLabel(this);
    // QPixmap centerLogoPixmap(":/ressources/olympicstake.svg"); // Remplacez par le chemin vers votre logo
    // centerLogoLabel->setPixmap(centerLogoPixmap.scaled(400, 400, Qt::KeepAspectRatio)); // Ajustez la taille selon vos besoins
    // centerLogoLabel->setAlignment(Qt::AlignCenter); // Centrez le logo dans le label

    // Ajoutez le logo central au layout vertical avant les boutons
    // verticalLayout->addWidget(centerLogoLabel, 0, Qt::AlignCenter);


    // Ajoutez un espace élastique en haut pour pousser le bouton vers le bas
    // verticalLayout->addStretch();

    SignUp = new QPushButton("Sign Up", this);
    SignUp->setStyleSheet(
        "QPushButton {"
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 15px 100px;" /* Espace intérieur */
        "border: none;" /* Pas de bordure */
        "text-align: center;"
        "margin-bottom: 5px;"
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
    connect(SignUp, SIGNAL(clicked()), this, SIGNAL(switchToSignUp()));

    SignIn = new QPushButton("Sign In", this);
    SignIn->setStyleSheet("color: rgb(0, 0, 0);");
    SignIn->setStyleSheet(
        "QPushButton {"
        "margin-top: 5px;"
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 15px 105px;" /* Espace intérieur */
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
    connect(SignIn, SIGNAL(clicked()), this, SIGNAL(switchToSignIn()));

    // Placez le bouton au centre en bas
    verticalLayout->addWidget(SignUp, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(SignIn, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addStretch();
    // Obtenez les dimensions de l'image
    // QPixmap backgroundImage(imagePath);
    // int imageWidth = backgroundImage.width();
    // int imageHeight = backgroundImage.height();

    // Définissez la taille de la fenêtre en fonction des dimensions de l'image
    //resize(imageWidth, imageHeight);

    // Ajoutez le bouton à un layout pour une meilleure disposition
    //QVBoxLayout *layout = new QVBoxLayout(firstscreen);
    //layout->addWidget(SignUp);
    //firstscreen->setLayout(layout);

    //setFixedSize(imageWidth, imageHeight+100);

    // Ajoutez l'écran d'accueil au QStackedWidget
    //stackedWidget->addWidget(this);

    // Définissez le QStackedWidget comme widget central de la QMainWindow
    //setCentralWidget(stackedWidget);
}
