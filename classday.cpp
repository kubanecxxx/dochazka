#include "classday.h"
#include <QStringList>
#include <math.h>

ClassDay::ClassDay(const QDate & date):
    datum(date),
    dovolena(false),
    Korekce(0),
    svatek(false)
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
    prac->nuceno = true;
    prac->prescas = false;
    prac->mx = true;
    prace.push_back(prac);

    //sobota neděle
    if (!IsNotWeekend())
    {
        prac->prescas = true;
    }

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
/*
    if (!Prichod1.isValid())
        return vystup;
*/
    //vystup
    vystup += Prichod1.toString(TIMEFORMAT);
    vystup += ESCAPE;
    vystup += Odchod1.toString(TIMEFORMAT);
    vystup += ESCAPE;
    vystup += Prichod2.toString(TIMEFORMAT);
    vystup += ESCAPE;
    vystup += Odchod2.toString(TIMEFORMAT);
    vystup += ESCAPE;

    vystup += QString("%1").arg(dovolena);
    vystup += ESCAPE;

    vystup += QString("%1").arg(Korekce);
    vystup += ESCAPE;
    vystup += Rucne.toString(TIMEFORMAT);
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
        vystup += QString("%1").arg(prac->nuceno);
        vystup += ESCAPE;
    }

    return vystup.trimmed();
}

bool  ClassDay::SaveXml(QDomElement *el)
{
    QDomDocument doc = el->ownerDocument();

    if (!Prichod1.isValid() && !dovolena  && !svatek)
    {
        if (!prace.count())
            return false;

        prace_t * prac = prace.at(0);
        if (!praceValid(prac))
            return false;
    }

    el->setAttribute("prichod1",Prichod1.toString(TIMEFORMAT));
    el->setAttribute("prichod2",Prichod2.toString(TIMEFORMAT));
    el->setAttribute("odchod1", Odchod1.toString(TIMEFORMAT));
    el->setAttribute("odchod2",Odchod2.toString(TIMEFORMAT));
    el->setAttribute("dovolena",dovolena);
    el->setAttribute("korekce" , Korekce);
    el->setAttribute("svatek",svatek);
    el->setAttribute("rucne",Rucne.toString(TIMEFORMAT));
    //el->setAttribute("pocetPraci",prace.count());

    foreach(prace_t * prac, prace)
    {
        if (!praceValid(prac))
            continue;

        QDomElement prace = doc.createElement("prace");
        QDomNode node = doc.createTextNode(prac->Poznamka);
        prace.setAttribute("hodiny",prac->hodiny);
        prace.setAttribute("hlaseni",prac->hlaseni);
        prace.setAttribute("prescas",prac->prescas);
        prace.setAttribute("nuceno",prac->nuceno);
        prace.setAttribute("mx",prac->mx);
        prace.appendChild(node);

        el->appendChild(prace);
    }

    return true;
}

bool ClassDay::praceValid(const prace_t *prac)
{
    bool ok = true;

    if (prac->hodiny == 0 && prac->hlaseni == 0 && prac->Poznamka.isEmpty())
        ok = false;


    return ok;
}

void ClassDay::LoadXml(QDomElement *el)
{
    Prichod1 = QTime::fromString(el->attribute("prichod1"),TIMEFORMAT);
    Prichod2 = QTime::fromString(el->attribute("prichod2"),TIMEFORMAT);
    Odchod1 = QTime::fromString(el->attribute("odchod1"),TIMEFORMAT);
    Odchod2 = QTime::fromString(el->attribute("odchod2"),TIMEFORMAT);
    Rucne =   QTime::fromString(el->attribute("rucne"),TIMEFORMAT);

    dovolena = el->attribute("dovolena").toInt();
    Korekce = el->attribute("korekce").toDouble();
    svatek = el->attribute("svatek").toInt();

    QDomElement prace = el->firstChildElement("prace");

    while (prace.isElement())
    {
        //do something
        prace_t * prac = AddPrace();
        prac->hodiny = prace.attribute("hodiny").toDouble();
        prac->hlaseni = prace.attribute("hlaseni").toInt();
        prac->prescas = prace.attribute("prescas").toInt();
        prac->nuceno = prace.attribute("nuceno").toInt();
        prac->Poznamka = (prace.firstChild().toText().data());
        prac->mx = prace.attribute("mx").toInt();

        prace = prace.nextSiblingElement("prace");
    }

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
    dovolena = list.at(4).toInt();
    Korekce = list.at(5).toFloat();
    Rucne = QTime::fromString(list.at(6),TIMEFORMAT);

    int count = list.at(7).toInt();
#define OFFSET 8
    for (int j = 0; j < count; j++)
    {
        int i = j * 5;
        prace_t * prac = new prace_t;
        prace.push_back(prac);
        prac->hodiny = list.at(OFFSET + i).toFloat();
        prac->hlaseni = list.at(OFFSET + 1 + i).toInt();
        prac->Poznamka = list.at(OFFSET + 2 + i);
        prac->prescas = list.at(OFFSET + 3 + i).toInt();
        prac->nuceno = list.at(OFFSET + 4 + i).toInt();
    }

    asm("nop");
}

float ClassDay::GetHodinyPrace() const
{
    QTime sest = QTime::fromString("6:00",TIMEFORMAT);
    const QTime obed1 = QTime::fromString("12:30",TIMEFORMAT);
    const QTime obed2 = QTime::fromString("13:00",TIMEFORMAT);
    const QTime vecere1 = QTime::fromString("18:00",TIMEFORMAT);
    const QTime vecere2 = QTime::fromString("18:30",TIMEFORMAT);
    const QTime nocni1 = QTime::fromString("2:00",TIMEFORMAT);
    const QTime nocni2 = QTime::fromString("2:30",TIMEFORMAT);
    int sec1;
    int sec2;

    if (!Prichod1.isValid())
        return 0;

    if (dovolena)
        return 0;

    //saturovat od 6 rána
    if (Prichod1 < sest)
        sec1 = sest.secsTo(Odchod1);
    else
        sec1 = Prichod1.secsTo(Odchod1);

    //nepočitat přestávku od 11:30 do 12:00
    if (Prichod1 <= obed1 && Odchod1 >= obed2)
        sec1 -= 1800;

    if (Prichod1 <= vecere1 && Odchod1 >= vecere2)
        sec1 -= 1800;

    if (Prichod1 <= nocni1 && Odchod1 >= nocni2)
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

    float cas = (sec1 + sec2);
    cas /= 3600.0;
    cas += (Korekce);

    return cas;
}

float ClassDay::GetHodinyVykazano() const
{
    if (dovolena)
        return 0;

    float ho = 0;
    foreach(prace_t * prac, prace)
    {
        if (!prac->prescas)
        ho += prac->hodiny;
    }

    return ho;
}

bool ClassDay::IsOk() const
{
    bool rucne = true;

    if (Rucne.isValid())
    {
        float time = GetHodinyPrace();
        float time2 = (float) (Rucne.hour() + Rucne.minute() / 60.0);
        rucne = (time  == time2);
    }

    if(IsNotWeekend())
        return ((GetHodinyPrace() == (GetHodinyVykazano() + GetHodinyPrescasVykazano())
                && Prichod1.isValid() && rucne)  || dovolena || svatek);
    else
        return ((GetHodinyPrace() == (GetHodinyVykazano() + GetHodinyPrescasVykazano())) && rucne);
}

float ClassDay::GetHodinyPrescasVykazano() const
{
    if (dovolena)
        return 0;

    float ho = 0;
    foreach(prace_t * prac, prace)
    {
        if (prac->prescas)
        ho += prac->hodiny;
    }

    return ho;

}

float ClassDay::GetHodinyPrescas() const
{
    if (dovolena)
        return 0;

    if (IsNotWeekend())
        return (7.5 - GetHodinyPrace());
    else
        return GetHodinyPrace();
}

bool ClassDay::IsNotWeekend() const
{
    bool sob = datum.dayOfWeek() % 6 == 0;
    bool ned = datum.dayOfWeek() % 7 == 0;

    bool can = !sob * !ned;

    return can;
}
