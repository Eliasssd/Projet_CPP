#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include "event.h"
#include <QWidget>

#include <QToolBar>
#include <QPushButton>
#include <QFrame>

class WelcomeScreen : public QWidget
{
        Q_OBJECT
    public:
        explicit WelcomeScreen(QWidget *parent = nullptr);


    private:
        QToolBar *toolbar;
        QPushButton *profile;



        QList<Event*> chargepublicinfo();
        const QMap<QString , QString> chargefootlogos();
        const QMap<QString , QString> chargebasketlogos();
        void setButtonStyle(QPushButton *button);
        void setrecStyle(QFrame *rect);
        Event* findEventById(const QList<Event*>& events, int searchId);

    signals:
};

#endif // WELCOMESCREEN_H
