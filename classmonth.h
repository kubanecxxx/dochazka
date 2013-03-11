#ifndef CLASSMONTH_H
#define CLASSMONTH_H

#include <QMap>
#include <QDate>
#include <QFile>
#include "classday.h"

class ClassMonth
{
public:
    ClassMonth(const QDate & date);
    ~ClassMonth();
    //vyhodi počet dní ktery by si měl odpracovat
    //počitá aji dovolenou
    float GetEstimatedHours() const;
    float GetHoursInWork() const;
    float GetVykazanoHours() const;

    ClassDay * GetDay(QDate date);
    void RemoveDay(QDate date) {days.remove(date);}
    QString Serialize();
    void Unserialize(QFile * file);

private:
    QMap<QDate,ClassDay *> days;
    QList<QDate> dovolena;
    ClassMonth(ClassMonth & cpy);
    const QDate datum;
};

#endif // CLASSMONTH_H
