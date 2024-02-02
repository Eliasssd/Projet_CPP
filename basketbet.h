#ifndef BASKETBET_H
#define BASKETBET_H

#include "basketevent.h"
#include <QMap>

class BasketBet : public BasketEvent
{
public:
    BasketBet(const int& ID, const QString &gender, const QDateTime& start, const QDateTime& end,
            const QString &location, const QString& team1, const QString& team2, const QMap<QString, double>& bettingOdds)
        : BasketEvent(ID, gender, start, end, location, team1, team2), odds(bettingOdds) {}

    int calculateEventDuration() const override {
        // Ici, vous pouvez garder la même logique ou la personnaliser si nécessaire
        return BasketEvent::calculateEventDuration();
    }

    // Redéfinition facultative si vous voulez un comportement différent de celui de FootEvent
    QString getEventType() const override {
        return "Football Betting";
    }

private:
    QMap<QString, double> odds; // Les cotes pour les paris
};

#endif // BASKETBET_H
