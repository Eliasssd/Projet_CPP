#include "signupscreen.h"
#include "securitymanager.h"
#include "networkmanager.h"
#include "globaldata.h"
#include <QSettings>

SignUpScreen::SignUpScreen(QWidget *parent)
    : QWidget{parent}
{

    QLabel *waveLabel = new QLabel(this);
    QPixmap wavePixmap(":/ressources/olympicstake.svg"); // Remplacez par le chemin d'accès à votre image de vague
    waveLabel->setPixmap(wavePixmap);
    waveLabel->setScaledContents(true); // Pour que l'image s'adapte à la taille du QLabel

    const QString buttonStyle = R"(
    border-radius: 20px; /* Ajustez pour l'arrondi des coins */
    font-size: 20px; /* Ajustez selon vos besoins */
    font-weight: bold;
    color: black; /* Couleur du texte */
    padding: 10px 50px; /* Espace intérieur */
    border: none; /* Pas de bordure */
    text-align: center;
    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));
)";

    // Créez un layout vertical pour organiser les widgets
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    // Ajoutez un espace élastique en haut pour pousser le bouton vers le bas
    verticalLayout->addWidget(waveLabel);
    // verticalLayout->addStretch();

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
    EmailLabel->setStyleSheet(buttonStyle);

    // Créez un QLabel pour le message d'erreur
    EmailErrorLabel = new QLabel(this);
    EmailErrorLabel->setStyleSheet("color: rgb(255, 0, 0);");
    EmailErrorLabel->hide();

    // Créez des zones d'entrée de texte et des libellés
    UsernameLineEdit = new QLineEdit(this);
    UsernameLineEdit->setStyleSheet(
        "border-radius: 20px;" /* Ajustez pour l'arrondi des coins */
        "font-size: 20px;" /* Ajustez selon vos besoins */
        "font-weight: bold;"
        "color: black;" /* Couleur du texte */
        "padding: 10px 15px;" /* Espace intérieur */
        "border: none;" /* Pas de bordure */
        "text-align: center;"
        // "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 80, 80, 255), stop:1 rgba(255, 255, 0, 255));"
        "}");
    UsernameLabel = new QLabel("User name", this);
    UsernameLabel->setStyleSheet(buttonStyle+
                                 "padding: 10px 30px;");

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
    PasswordLabel->setStyleSheet(buttonStyle+
                                 "padding: 10px 30px;");

    // Créez un QLabel pour le message d'erreur
    PasswordErrorLabel = new QLabel(this);
    PasswordErrorLabel->setStyleSheet("color: rgb(255, 0, 0);");
    PasswordErrorLabel->hide();

    Submit = new QPushButton("Submit", this);
    Submit->setStyleSheet("color: rgb(0, 0, 0);");
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
        /* Changez légèrement les couleurs pour l'effet hover, si vous le souhaitez */
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 100, 100, 255), stop:1 rgba(255, 255, 100, 255));"
        "}"

        "QPushButton:pressed {"
        /* Effet pour le bouton pressé (facultatif) */
        "background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(200, 70, 70, 255), stop:1 rgba(200, 200, 0, 255));"
        "}");
    //connect(SignUp, &QPushButton::clicked, this, &SignUpScreen::SignUp);

    Return = new QPushButton ("Return" , this);
    Return->setStyleSheet("color: rgb(0, 0, 0);");
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

    // Connecter le signal clicked du bouton Submit au slot sendMailRequest
    connect(Submit, &QPushButton::clicked,this, [=]()
            {
                // Appeler sendMailRequest avec la chaîne nécessaire en tant que paramètre
                // Connectez le clic du bouton OK à une fonction qui récupère les entrées
                //SLOT(accept());

                // MAJ 12/01
                // vérifier que le mail est valide
                QString mail = EmailLineEdit->text();
                QString username = UsernameLineEdit->text();
                if (SecurityManager::validEmail(mail))
                {

                    // Set the email
                    GlobalData::instance().setInfo(mail , username);


                    EmailErrorLabel->hide();
                    // vérifier que le mot de passe est valide

                    QString password = PasswordLineEdit->text();
                    if (SecurityManager::validPassword(password , PasswordErrorLabel))
                    {
                        QString username = UsernameLineEdit->text();
                        QString hash = SecurityManager::hash(password);
                        if (NetworkManager::SignUpRequest(mail , username , hash) == "true")
                        {
                            emit switchToFA2();
                        }
                    }
                }
                else
                {
                    EmailErrorLabel->setText("E-mail address not valid.");
                    EmailErrorLabel->show();
                }

            });

    connect(Return, &QPushButton::clicked,this, [=]()
        {
            emit switchToHome();
        });
    // Placez le bouton au centre en bas
    verticalLayout->addWidget(EmailLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(EmailLineEdit, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(EmailErrorLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(UsernameLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(UsernameLineEdit, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(PasswordLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(PasswordLineEdit, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(PasswordErrorLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(Submit, 0, Qt::AlignHCenter | Qt::AlignBottom);
    verticalLayout->addWidget(Return, 0, Qt::AlignHCenter | Qt::AlignBottom);

}
