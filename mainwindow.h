#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include <QtNetwork/QNetworkReply>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    public slots:
        void changeScreen(int index);
        void showSignUpScreen();
        void showSignInScreen();
        void showFA2Screen();
        void showHome();
        void showWelcomeScreen();
        // void SignUp();
        // void LogIn();
        // void retourAccueil();

    //private slots:
        // QString sendMailRequest(QString mail);

        // void ecrantest();
        // void FA2(QVBoxLayout *verticalLayout , QLineEdit* passwordlineEdit , QLineEdit* usernamelineEdit , QLineEdit* maillineEdit , QLabel* mail , QLabel* username , QLabel* password ,  QPushButton* bouton , QString email);
        // QString FA2_Request(QString FA2 , QString email);



    private:
        // const std::string generateSaltString(const size_t len);
        // const std::string hash(const std::string passwordH,const std::string salt,const bool genSalt,const size_t saltLength,const std::string hashAlg);
        QStackedWidget *stackedWidget;
        Ui::MainWindow *ui;
        // QWidget *firstscreen;
        // QWidget *connexionscreen;
        // QLabel *backgroundLabel;
        // QPixmap background;
        // QString exist;
};
#endif // MAINWINDOW_H
