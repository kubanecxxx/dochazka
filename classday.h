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
    ClassDay();
    ~ClassDay();
    ClassDay(ClassDay & cpy);

    typedef struct
    {
        float hodiny;
        QString hlaseni;
        QString Poznamka;
        bool prescas;
    } prace_t;      //řádek v tabulce

    float GetHodinyPrace() const;
    float GetHodinyVykazano() const;
    QString GetTextLine() const;
    void ReadTextLine(const QString & line);

    QTime Prichod1, Prichod2;
    QTime Odchod1, Odchod2;

    prace_t * AddPrace();
    inline void DelPrace(int i) {prace.removeAt(i);}
    prace_t * GetPraceP(int i) const;
    inline QList <prace_t*> GetPrace() const {return prace;}

    bool IsOk() const;

private:
    QList<prace_t*> prace;
};

#endif // CLASSDAY_H
