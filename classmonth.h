#ifndef CLASSMONTH_H
#define CLASSMONTH_H

#include <QMap>
#include <QDate>
#include <QFile>
#include "classday.h"
#include <QDomElement>

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
    float GetVykazanoPrescas() const;
    int GetVybranaDovolena(int * svatky = NULL) const;
    int GetWorkCount() const;

    bool IsVykazanoVPraci() const;
    bool IsVykazanoRegulerne() const;
    QList<ClassDay *> GetDays() const {return days.values();}

    ClassDay * GetDay(QDate date);
    ClassDay * GetDay(int num);
    void RemoveDay(QDate date) {days.remove(date);}
    QString Serialize();
    void Unserialize(QFile * file);
    void SaveXml(QDomElement * year);
    void LoadXml(QDomElement * month);

private:
    QMap<QDate,ClassDay *> days;
    QList<QDate> dovolena;
    ClassMonth(ClassMonth & cpy);
    const QDate datum;
};

#endif // CLASSMONTH_H
