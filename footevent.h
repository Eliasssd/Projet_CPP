#ifndef FOOTEVENT_H
#define FOOTEVENT_H

#include "event.h"

class FootEvent : public Event
{
public:
    FootEvent(const int& ID, const QString &gender, const QDateTime& start, const QDateTime& end,
              const QString &location, const QString& team1, const QString& team2)
        : Event(ID, gender, start, end, location), teamOne(team1), teamTwo(team2) {}

    int calculateEventDuration() const override {
        return startTime.secsTo(endTime) / 60; // Durée en minutes
    }

    QString getEventType() const override {
        return "Football";
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

#endif // FOOTEVENT_H
