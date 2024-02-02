#ifndef FOOTBET_H
#define FOOTBET_H

#include "footevent.h"
#include <QMap>

class FootBet : public FootEvent
{
public:
    FootBet(const int& ID, const QString &gender, const QDateTime& start, const QDateTime& end,
            const QString &location, const QString& team1, const QString& team2, const QMap<QString, double>& bettingOdds)
        : FootEvent(ID, gender, start, end, location, team1, team2), odds(bettingOdds) {}

    int calculateEventDuration() const override {
        // Ici, vous pouvez garder la même logique ou la personnaliser si nécessaire
        return FootEvent::calculateEventDuration();
    }

    // Redéfinition facultative si vous voulez un comportement différent de celui de FootEvent
    QString getEventType() const override {
        return "Football Betting";
    }

private:
    QMap<QString, double> odds; // Les cotes pour les paris
};

#endif // FOOTBET_H
