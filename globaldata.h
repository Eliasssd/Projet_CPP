#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QString>

class GlobalData
{
    public:
        static GlobalData& instance()
        {
            static GlobalData instance;
            return instance;
        }

        QString getEmail() const { return email; }
        QString getUsername() const { return username; }
        void setInfo(const QString &value1 , const QString &value2) { email = value1; username = value2; }

    private:
        QString email;
        QString username;
        GlobalData() {}  // Constructor is private
        GlobalData(GlobalData const&);  // Prevent copying
        void operator=(GlobalData const&);  // Prevent assignment
};

#endif // GLOBALDATA_H
