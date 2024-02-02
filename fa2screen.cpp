#include "fa2screen.h"
#include "networkmanager.h"
#include "globaldata.h"

FA2Screen::FA2Screen(QWidget *parent)
    : QWidget{parent}
{
    QLabel *waveLabel = new QLabel(this);
    QPixmap wavePixmap(":/ressources/olympicstake.svg"); // Remplacez par le chemin d'accès à votre image de vague
    waveLabel->setPixmap(wavePixmap);
    waveLabel->setScaledContents(true); // Pour que l'image s'adapte à la taille du QLabel

    verticalLayout = new QVBoxLayout(this);

    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(waveLabel);

    FA2LineEdit = new QLineEdit(this);
    FA2LineEdit->setStyleSheet(
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 10px 15px;" /* Espace intérieur */
        "border: none;" /* Pas de bordure */
        "text-align: center;"
        // "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));"
        "}");
    FA2Label = new QLabel("Please enter the number you received", this);
    FA2Label->setStyleSheet(
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 10px 50px;" /* Espace intérieur */
        "border: none;" /* Pas de bordure */
        "text-align: center;"
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));"
        "}");

    Submit = new QPushButton("Submit", this);
    Submit->setStyleSheet(
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

    Return = new QPushButton("Return", this);
    Return->setStyleSheet(
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

    verticalLayout->addWidget(FA2Label, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(FA2LineEdit, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(Submit, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(Return, 0, Qt::AlignHCenter | Qt::AlignBottom);

    connect(Submit, &QPushButton::clicked,this, [=]()
    {
        QString FA2 = FA2LineEdit->text();
        // Get the email
        QString Email = GlobalData::instance().getEmail();
        qDebug() << Email;
        QString test = NetworkManager::FA2_Request(FA2 , Email);
        if (test == "true")
        {
            qDebug() << "Connecté";
            emit switchToWelcome();
        }
    });

    connect(Return, SIGNAL(clicked()), this, SIGNAL(switchToSignUp()));
}
