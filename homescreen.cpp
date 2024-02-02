#include "homescreen.h"
#include "globaldata.h"
#include "qdebug.h"

Homescreen::Homescreen(QWidget *parent)
    : QWidget{parent}
{

    toolbar = new QToolBar("TopToolBar" , this);
    // toolbar->addWidget(new QPushButton("Bouton"));
    // toolbar->setMovable(false);
    QString username = GlobalData::instance().getUsername();
    QString Email = GlobalData::instance().getEmail();
    qDebug() << username;
    qDebug() << Email;
    qDebug() << "code";
    QPushButton *profile = new QPushButton(username , this);
    profile->setStyleSheet(
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
    toolbar->addWidget(profile);

    QWidget *separator = new QWidget(this);
    separator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(separator);

    toolbar->addAction("Balance");
}
