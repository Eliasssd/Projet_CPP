#ifndef CONNECTIONSCREEN_H
#define CONNECTIONSCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class ConnectionScreen : public QWidget
{
        Q_OBJECT
    public:
        explicit ConnectionScreen(QWidget *parent = nullptr);

    signals:
        void switchToSignUp();
        void switchToSignIn();

    private:
        QVBoxLayout *verticalLayout;
        QPushButton *SignUp;
        QPushButton *SignIn;
};

#endif // CONNECTIONSCREEN_H
