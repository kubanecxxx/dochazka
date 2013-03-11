#ifndef CLASSDAY_H
#define CLASSDAY_H

#include <QString>
#include <QTime>
#include <QList>

#define TIMEFORMAT "h:mm"
#define DATEFORMAT "dd.MM.yyyy"
#define ESCAPE ";;; "
#define ESC_LEN 4

class ClassDay
{
public:
    ClassDay(const QDate & datum);
    ~ClassDay();
    ClassDay(ClassDay & cpy);

    typedef struct
    {
        float hodiny;
        QString hlaseni;
        QString Poznamka;
        bool prescas;
    } prace_t;      //řádek v tabulce

    //počet hodiny v práci is přesčasem
    float GetHodinyPrace() const;
    //počet vykázanéch hodin, bez přesčasu
    float GetHodinyVykazano() const;
    //počet hodin v práci přesčas
    float GetHodinyPrescas() const ;
    //počet vykazanéch přesčasu
    float GetHodinyPrescasVykazano() const;

    QString GetTextLine() const;
    void ReadTextLine(const QString & line);

    QTime Prichod1, Prichod2;
    QTime Odchod1, Odchod2;

    prace_t * AddPrace();
    inline void DelPrace(int i) {prace.removeAt(i);}
    prace_t * GetPraceP(int i) const;
    inline QList <prace_t*> GetPrace() const {return prace;}

    bool IsOk() const;
    bool IsNotWeekend() const;
    const QDate datum;

    bool dovolena;

private:
    QList<prace_t*> prace;
};

#endif // CLASSDAY_H
