#ifndef CLASSDAY_H
#define CLASSDAY_H

#include <QString>
#include <QTime>
#include <QList>
#include <QDomElement>

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

    struct prace_t
    {
        float hodiny;
        int hlaseni;
        QString Poznamka;
        bool prescas;
        bool nuceno;
        bool mx;
        bool operator==(const prace_t & other)
        {
            bool same;
            same = ((hlaseni == other.hlaseni) && (mx == other.mx));
            return same;
        }
    };      //řádek v tabulce

    typedef prace_t prace_t;
    typedef enum {PUPIK, AUTO,KOLO, JINAK} dojizdeni_t;

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
    QTime Rucne;
    float Korekce;
    float English;
    dojizdeni_t Dojizdeni;

    prace_t * AddPrace();
    inline void DelPrace(int i) {prace.removeAt(i);}
    prace_t * GetPraceP(int i) const;
    inline QList <prace_t*> GetPrace() const {return prace;}

    bool IsOk() const;
    bool IsNotWeekend() const;
    const QDate datum;

    bool dovolena;
    bool SaveXml(QDomElement * el);
    void LoadXml(QDomElement * el);
    bool svatek;

    static bool praceValid(const prace_t * prac);

private:
    QList<prace_t*> prace;
};

#endif // CLASSDAY_H
