#ifndef SIGNUPSCREEN_H
#define SIGNUPSCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>

class SignUpScreen : public QWidget
{
        Q_OBJECT
    public:
        explicit SignUpScreen(QWidget *parent = nullptr);

    private:
        QVBoxLayout *verticalLayout;

        QPushButton *Submit;
        QPushButton *Return;

        QLineEdit* EmailLineEdit;
        QLineEdit* UsernameLineEdit;
        QLineEdit* PasswordLineEdit;

        QLabel* EmailLabel;
        QLabel* UsernameLabel;
        QLabel* PasswordLabel;

        QLabel* EmailErrorLabel;
        QLabel* PasswordErrorLabel;

        void initButton(const QString &Title , QPushButton* Button);

    signals:
        void switchToFA2();
        void switchToHome();
};

#endif // SIGNUPSCREEN_H
