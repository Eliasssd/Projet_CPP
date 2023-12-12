#pragma once
#include <iostream>
#include <string>

using namespace std;
/**
 * User
 */
class User
{
    protected:
        string _mail;          // address mail
        string _Hashpassword;  // password
        string _name;          // name
        float _money;          // money

    public:
        User();
        User(float money, string mail, string Hashpassword, string name);
        User(float money);

        void Sign_up();

        void Log_in();
        //verify si le mot de passe lié à mail est correcte 
        bool Check_password(string password , string mail);
        //vérifie si le mot de passe lié à name est correcte
        bool Check_password(string password , string name);
        //vérifie si le mail est déjà utilisé
        bool Check_mail(string mail);
        //vérifie si le nom est déjà utilisé
        bool Check_name(string name);

        void setMail(string mail) {_mail = mail;}
        void getMail() const {cout << _mail << endl;}

        void setHashpassword(string Hashpassword) {_Hashpassword = Hashpassword;}

        void setName(string name) {_name = name;}
        string getName() const {return _name;}

        void setMoney(float money) {_money = money;}
        float getMoney() const {return _money;}



        friend ostream& operator<<(ostream& os, const User& u);
};
