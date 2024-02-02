#ifndef SIGNINSCREEN_H
#define SIGNINSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>


class SignInScreen : public QWidget
{
        Q_OBJECT
    public:
        explicit SignInScreen(QWidget *parent = nullptr);

    private:
        QVBoxLayout *verticalLayout;

        QPushButton *Submit;
        QPushButton *Return;

        QLineEdit* EmailLineEdit;
        QLineEdit* PasswordLineEdit;

        QLabel* EmailLabel;
        QLabel* PasswordLabel;

        QLabel* EmailErrorLabel;
        QLabel* PasswordErrorLabel;

        void initButton(const QString &Title , QPushButton* Button);

    signals:
        void switchToHome();
        void switchToWelcome();
};

#endif // SIGNINSCREEN_H
