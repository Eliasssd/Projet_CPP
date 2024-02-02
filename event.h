#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include <QDateTime>

// Classe abstraite de base pour tout événement sportif
class Event {
public:
    Event(const int& ID,const QString &gender, const QDateTime& start, const QDateTime& end, const QString &location)
        : eventID(ID), gender(gender), startTime(start), endTime(end), stadium(location) {}

    virtual ~Event() {}

    int getEventID() const {
        return eventID;
    };

    virtual int calculateEventDuration() const = 0;

    virtual QString getEventType() const = 0;

protected:
    int eventID;
    QString gender;
    QDateTime startTime;
    QDateTime endTime;
    QString stadium;
};

#endif // EVENT_H
