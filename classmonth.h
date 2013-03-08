#ifndef CLASSMONTH_H
#define CLASSMONTH_H

#include <QMap>
#include <QDate>
#include "classday.h"

class ClassMonth
{
public:
    ClassMonth();
    //vyhodi počet dní ktery by si měl odpracovat
    //počitá aji dovolenou
    float GetEstimatedHours() const;
    float GetHoursInWork() const;
    float GetVykazanoHours() const;

    ClassDay * AddDay(QDate date);
    ClassDay * GetDay(QDate date) const {return days.value(date);}
    void RemoveDay(QDate date) {days.remove(date);}
    QString Serialize();
    void Unserialize(QString & data);

private:
    QMap<QDate,ClassDay *> days;
    QList<QDate> dovolena;
    int volneDovolene;
};

#endif // CLASSMONTH_H
