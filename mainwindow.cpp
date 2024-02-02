#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "signupscreen.h"
#include "signinscreen.h"
#include "fa2screen.h"
#include "welcomescreen.h"
#include "globaldata.h"


#include <connectionscreen.h>
#include <connectionscreen.h>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

#include <QCoreApplication>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Olympic Stake"); // Remplacez par le titre que vous souhaitez
    this->setFixedSize(500, 900);
    this->setContentsMargins(0, 0, 0, 0);
    // Créez un QStackedWidget pour gérer les différents écrans
    stackedWidget = new QStackedWidget(this);

    ConnectionScreen* screen = new ConnectionScreen(this);
    SignUpScreen* signupscreen = new SignUpScreen(this);
    SignInScreen* signinscreen = new SignInScreen(this);
    FA2Screen* fa2screen = new FA2Screen(this);
    // WelcomeScreen* homescreen = new WelcomeScreen(this);
    // stackedWidget->addWidget(homescreen);

    stackedWidget->addWidget(screen);
    stackedWidget->addWidget(signupscreen);
    stackedWidget->addWidget(signinscreen);
    stackedWidget->addWidget(fa2screen);


    // Définissez le QStackedWidget comme widget central de la QMainWindow
    setCentralWidget(stackedWidget);

    connect(screen , SIGNAL(switchToSignUp()), this, SLOT(showSignUpScreen()));
    connect(screen , SIGNAL(switchToSignIn()), this, SLOT(showSignInScreen()));
    connect(signupscreen , SIGNAL(switchToHome()), this , SLOT(showHome()));
    connect(signupscreen , SIGNAL(switchToFA2()), this, SLOT(showFA2Screen()));
    connect(signinscreen , SIGNAL(switchToHome()), this , SLOT(showHome()));
    connect(fa2screen , SIGNAL(switchToSignUp()), this , SLOT(showSignUpScreen()));
    connect(fa2screen , SIGNAL(switchToWelcome()), this , SLOT(showWelcomeScreen()));
    connect(signinscreen , SIGNAL(switchToWelcome()), this , SLOT(showWelcomeScreen()));

}

void MainWindow::showSignUpScreen() {
    stackedWidget->setCurrentIndex(1); // Index de l'écran SignUp dans le QStackedWidget
}

void MainWindow::showSignInScreen() {
    stackedWidget->setCurrentIndex(2); // Index de l'écran SignUp dans le QStackedWidget
}

void MainWindow::showFA2Screen() {
    stackedWidget->setCurrentIndex(3); // Index de l'écran SignUp dans le QStackedWidget
}

void MainWindow::showWelcomeScreen() {
    WelcomeScreen* homescreen = new WelcomeScreen(this);
    stackedWidget->addWidget(homescreen);
    QToolBar *toolbar = addToolBar("TopBar");
    QString username = GlobalData::instance().getUsername();
    QPushButton *profile = new QPushButton(username, this);
    profile->setStyleSheet(
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
    toolbar->addWidget(profile);

    QWidget *separatorbas = new QWidget();
    separatorbas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(separatorbas);

    toolbar->addAction("balance");
    stackedWidget->setCurrentIndex(4); // Index de l'écran SignUp dans le QStackedWidget
}

void MainWindow::showHome()
{
    stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeScreen(int index)
{
    stackedWidget->setCurrentIndex(index);
}



