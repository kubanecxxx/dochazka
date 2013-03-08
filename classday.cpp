#include "classday.h"
#include <QStringList>
#include <math.h>

ClassDay::ClassDay()
{
}


ClassDay::~ClassDay()
{
    foreach(prace_t * prac , prace)
    {
        delete prac;
    }
}

ClassDay::ClassDay(ClassDay &cpy):
    Prichod1(cpy.Prichod1),
    Prichod2(cpy.Prichod2),
    Odchod1(cpy.Odchod1),
    Odchod2(cpy.Odchod2)
{
    for (int i = 0; i < cpy.prace.count(); i++)
    {
        prace_t * prac = new prace_t;
        memcpy(prac,cpy.prace.at(i),sizeof(prace_t));
        prace.push_back(prac);
    }
}

ClassDay::prace_t * ClassDay::AddPrace()
{
    prace_t * prac = new prace_t;
    prace.push_back(prac);

    return prac;
}

ClassDay::prace_t * ClassDay::GetPraceP(int i) const
{
    if (i < prace.count())
        return (prace.at(i));
    else
        return NULL;
}

QString ClassDay::GetTextLine() const
{
    QString vystup;

    if (!Prichod1.isValid())
        return vystup;

    //vystup
    vystup += Prichod1.toString(TIMEFORMAT);
    vystup += ESCAPE;
    vystup += Odchod1.toString(TIMEFORMAT);
    vystup += ESCAPE;
    vystup += Prichod2.toString(TIMEFORMAT);
    vystup += ESCAPE;
    vystup += Odchod2.toString(TIMEFORMAT);
    vystup += ESCAPE;

    vystup += QString("%1").arg(prace.count());
    vystup += ESCAPE;

    foreach(prace_t * prac, prace)
    {
        vystup += QString("%1").arg(prac->hodiny);
        vystup += ESCAPE;
        vystup += prac->hlaseni;
        vystup += ESCAPE;
        vystup += prac->Poznamka;
        vystup += ESCAPE;
        vystup += QString("%1").arg(prac->prescas);
        vystup += ESCAPE;
    }

    return vystup;
}

void ClassDay::ReadTextLine(const QString &line)
{
    QString moje(line);
    QStringList list;


    while (moje.indexOf(ESCAPE) >= 0)
    {
        QString temp;
        int i = moje.indexOf(ESCAPE);;
        temp = moje.left(i);
        moje.remove(0, i + ESC_LEN);

        list << temp;
    }

    Prichod1 = QTime::fromString(list.at(0),TIMEFORMAT);
    Odchod1 = QTime::fromString(list.at(1),TIMEFORMAT);
    Prichod2 = QTime::fromString(list.at(2),TIMEFORMAT);
    Odchod2 = QTime::fromString(list.at(3),TIMEFORMAT);

    int count = list.at(4).toInt();
    for (int j = 0; j < count; j++)
    {
        int i = j * 4;
        prace_t * prac = new prace_t;
        prace.push_back(prac);
        prac->hodiny = list.at(5 + i).toFloat();
        prac->hlaseni = list.at(6+i);
        prac->Poznamka = list.at(7+i);
        prac->prescas = list.at(8+i).toInt();
    }

    asm("nop");
}

float ClassDay::GetHodinyPrace() const
{
    QTime sest = QTime::fromString("6:00",TIMEFORMAT);
    QTime obed1 = QTime::fromString("11:30",TIMEFORMAT);
    QTime obed2 = QTime::fromString("12:00",TIMEFORMAT);
    int sec1;
    int sec2;

    //saturovat od 6 rána
    if (Prichod1 < sest)
        sec1 = sest.secsTo(Odchod1);
    else
        sec1 = Prichod1.secsTo(Odchod1);

    //nepočitat přestávku od 11:30 do 12:00
    if (Prichod1 <= obed1 && Odchod1 >= obed2)
        sec1 -= 1800;

    sec1 = floor(sec1/1800.0) * 1800;

    if (Prichod2 > Odchod1)
    {
        sec2 = Prichod2.secsTo(Odchod2);
        sec2 = floor(sec2/1800.0) * 1800;
    }
    else
    {
        sec2 = 0;
    }

    float cas = (sec1 + sec2) / 3600.0;

    return cas;
}

float ClassDay::GetHodinyVykazano() const
{
    float ho = 0;
    foreach(prace_t * prac, prace)
    {
        ho += prac->hodiny;
    }

    return ho;
}

bool ClassDay::IsOk() const
{
    return (GetHodinyPrace() == GetHodinyVykazano());
}
