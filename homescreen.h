#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QWidget>
#include <QToolBar>
#include <QPushButton>

class Homescreen : public QWidget
{
        Q_OBJECT
    public:
        explicit Homescreen(QWidget *parent = nullptr);

    private:
        QToolBar *toolbar;
        QPushButton *profile;


    signals:
};

#endif // HOMESCREEN_H
