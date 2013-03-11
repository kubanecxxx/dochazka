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
    }

    return hodiny;
}
