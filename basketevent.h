#ifndef BASKETEVENT_H
#define BASKETEVENT_H

#include "event.h"

class BasketEvent : public Event
{
public:
    BasketEvent(const int& ID, const QString &gender, const QDateTime& start, const QDateTime& end,
              const QString &location, const QString& team1, const QString& team2)
        : Event(ID, gender, start, end, location), teamOne(team1), teamTwo(team2) {}

    int calculateEventDuration() const override {
        return startTime.secsTo(endTime) / 60; // Durée en minutes
    }

    QString getEventType() const override {
        return "Basketball";
    }

    QString getTeamone()
    {
        return teamOne;
    }

    QString getTeamTwo()
    {
        return teamTwo;
    }

protected:
    QString teamOne;
    QString teamTwo;
};
#endif // BASKETEVENT_H
