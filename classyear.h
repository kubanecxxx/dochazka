#ifndef CLASSYEAR_H
#define CLASSYEAR_H

#include "classmonth.h"
#include <QMap>
#include <QFile>

class ClassYear
{
public:
    ClassYear();
    ~ClassYear();
    ClassDay * GetDay(QDate date);
    ClassMonth * GetMonth(QDate date) {return GetMonth(date.month());}
    void SaveFile() const;
    void LoadFile();
    int GetVolnaDovolena() const;

private:

    QMap<int,ClassMonth*> mesice;
    ClassMonth * AddMonth(int mont);
    ClassMonth * GetMonth(int mon);
    ClassYear(ClassYear & cpy);
    int volneDovolene;

    QFile * file;
};

#endif // CLASSYEAR_H
