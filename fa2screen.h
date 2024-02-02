#ifndef FA2SCREEN_H
#define FA2SCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>

class FA2Screen : public QWidget
{
        Q_OBJECT
    public:
        explicit FA2Screen(QWidget *parent = nullptr);

    private:
        QVBoxLayout* verticalLayout;

        QLineEdit *FA2LineEdit;
        QLabel *FA2Label;

        QPushButton *Submit;
        QPushButton *Return;


    signals:
        void switchToSignUp();
        void switchToWelcome();

};

#endif // FA2SCREEN_H
