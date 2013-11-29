#include "classmonth.h"

ClassMonth::ClassMonth(const QDate & date):
    datum(date)
{

}

ClassMonth::~ClassMonth()
{
    QList<ClassDay*> dny = days.values();

    foreach(ClassDay * den, dny)
    {
        delete den;
    }
}

ClassDay * ClassMonth::GetDay(int num)
{
    QDate date;
    date.setDate(datum.year(),datum.month(),num);
    return GetDay(date);
}

ClassDay * ClassMonth::GetDay(QDate date)
{
    ClassDay * day=days.value(date,NULL);
    if (day == NULL)
    {
        day = new ClassDay(date);
        days.insert(date,day);
    }

    return day;
}

QString ClassMonth::Serialize()
{
    QString data;
    //hodit datum a pod něho den
    QMapIterator<QDate, ClassDay*> it(days);

    //počet dní v měsicu

    int i = 0;

    while(it.hasNext())
    {
        it.next();
        ClassDay * day = it.value();
        QDate date = it.key();
        QString daySer = day->GetTextLine();

        if (!daySer.isEmpty())
        {
            data += date.toString(DATEFORMAT);
            data += '\n';
            data += daySer;
            data += '\n';
            i++;
        }
    }

    if (i)
        data.prepend(QString("%1 \n").arg(i));

    return data;
}

void ClassMonth::Unserialize(QFile * file)
{
    //QString str(data);
    //obráceně vyčorovat data a vyplnit mapu
    QByteArray line = file->readLine().trimmed();
    int DayCount = line.toInt();

    for (int i = 0; i < DayCount; i++)
    {
        line = file->readLine().trimmed();
        ClassDay * day = GetDay(QDate::fromString(line,DATEFORMAT));
        line = file->readLine();
        day->ReadTextLine(QString::fromUtf8(line));
    }
}

void ClassMonth::SaveXml(QDomElement * year)
{
    QDomDocument doc = year->ownerDocument();
    QDomElement el = doc.createElement("month");
    el.setAttribute("number",datum.month());

    int i = 0;
    QMapIterator<QDate, ClassDay*> it(days);
    while(it.hasNext())
    {
        QDomElement den = doc.createElement("day");
        it.next();
        ClassDay * day = it.value();
        QDate date = it.key();
        den.setAttribute("number", date.day());
        if (day->SaveXml(&den))
            el.appendChild(den);
    }

    year->appendChild(el);
}

void ClassMonth::LoadXml(QDomElement *month)
{
    QDomElement day = month->firstChildElement("day");

    while (day.isElement())
    {
        //do something
        QString je =  day.attribute("number");
        ClassDay * den = GetDay(je.toInt());
        den->LoadXml(&day);

        day = day.nextSiblingElement("day");
    }

}

float ClassMonth::GetEstimatedHours() const
{
    //projit celej měsic a zjistit počet pracovnich dnu a vynasobit 7.5
    int mont = datum.month();
    int year = datum.year();

    int dny = 0;
    for (int i = 1 ; i <32 ;i++)
    {
        QDate temp(year,mont,i);
        if (temp.isValid() && !(temp.dayOfWeek() % 6 == 0 ||
                                temp.dayOfWeek() % 7 == 0))
            dny++;
    }
    int svatky;
    dny -= GetVybranaDovolena(&svatky);
    dny -= svatky;

    return dny * 7.5;
}

float ClassMonth::GetHoursInWork() const
{
    QMapIterator<QDate,ClassDay*> it(days);
    float hodiny = 0;

    while(it.hasNext())
    {
        it.next();
        hodiny += it.value()->GetHodinyPrace();
    }

    return hodiny;
}

float ClassMonth::GetVykazanoHours() const
{
    QMapIterator<QDate,ClassDay*> it(days);
    float hodiny = 0;

    while(it.hasNext())
    {
        it.next();
        hodiny += it.value()->GetHodinyVykazano();
        //hodiny += it.value()->English;
    }

    return hodiny;
}

float ClassMonth::GetVykazanoHoursEng() const
{
    QMapIterator<QDate,ClassDay*> it(days);
    float hodiny = 0;

    while(it.hasNext())
    {
        it.next();
        hodiny += it.value()->GetHodinyVykazano();
        hodiny += it.value()->English;
    }

    return hodiny;
}

float ClassMonth::GetVykazanoPrescasEng() const
{
    QMapIterator<QDate,ClassDay*> it(days);
    float hodiny = 0;

    while(it.hasNext())
    {
        it.next();
        hodiny += it.value()->GetHodinyPrescasVykazano();
        hodiny -= it.value()->English;
    }

    return hodiny;
}

float ClassMonth::GetVykazanoPrescas() const
{
    QMapIterator<QDate,ClassDay*> it(days);
    float hodiny = 0;

    while(it.hasNext())
    {
        it.next();
        hodiny += it.value()->GetHodinyPrescasVykazano();
        //hodiny -= it.value()->English;
    }

    return hodiny;
}

float ClassMonth::GetEnglish() const
{
    QMapIterator<QDate,ClassDay*> it(days);
    float hodiny = 0;

    while(it.hasNext())
    {
        it.next();
        //hodiny += it.value()->GetHodinyPrescasVykazano();
        hodiny += it.value()->English;
    }

    return hodiny;
}

int ClassMonth::GetVybranaDovolena(int * svatek) const
{
    QMapIterator<QDate,ClassDay*> it(days);
    int hodiny = 0;

    if (svatek != NULL)
        *svatek = 0;

    while(it.hasNext())
    {
        it.next();
        if (it.value()->dovolena)
            hodiny++;
        if (svatek && it.value()->svatek)
            (*svatek)++;
    }

    return hodiny;
}

int ClassMonth::GetWorkCount() const
{
    QMapIterator<QDate,ClassDay*> it(days);
    int hodiny = 0;

    while(it.hasNext())
    {
        it.next();
        hodiny += it.value()->GetPrace().count();
    }

    return hodiny;
}

bool ClassMonth::IsVykazanoVPraci() const
{
    return (GetHoursInWork() == GetVykazanoHours() + GetVykazanoPrescas());
}

bool ClassMonth::IsVykazanoRegulerne() const
{
    return (GetEstimatedHours() == GetVykazanoHoursEng());
}
