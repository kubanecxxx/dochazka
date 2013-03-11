#include "classyear.h"
#include <QDebug>

ClassYear::ClassYear()
{
    //načist ze souboru
    file = new QFile("soubor.txt");
    LoadFile();
}

ClassYear::~ClassYear()
{
    QList<ClassMonth*> list = mesice.values();

    foreach(ClassMonth * mont, list)
    {
        delete mont;
    }
}

ClassDay * ClassYear::GetDay(QDate date)
{
    ClassMonth * mont = GetMonth(date.month());
    if (mont == NULL)
        mont = AddMonth(date.month());

    return mont->GetDay(date);
}

ClassMonth * ClassYear::GetMonth(int mon)
{
    ClassMonth * month = mesice.value(mon,NULL);
    return month;
}

ClassMonth * ClassYear::AddMonth(int mont)
{
    QDate date;
    date.setDate(2013,mont,1);
    ClassMonth * mon = new ClassMonth(date);
    mesice.insert(mont,mon);

    return mon;
}

void ClassYear::SaveFile()
{
    file->open(QFile::WriteOnly);

    //počet měsicu
    file->write(QString("%1 \n").arg(mesice.count()).toLatin1());
    QMapIterator<int, ClassMonth *> it(mesice);
    while(it.hasNext())
    {
        it.next();
        QString line;
        //číslo měsice
        line += QString("%1\n").arg(it.key());
        line += it.value()->Serialize();
        file->write(line.toUtf8());

        qDebug() << line;
    }

    file->close();
}

void ClassYear::LoadFile()
{
    file->open(QFile::ReadOnly);
    mesice.clear();

    QByteArray line = file->readLine().trimmed();
    int monthcount = line.toInt();

    for (int i = 0 ; i < monthcount ; i++)
    {
        line = file->readLine().trimmed();
        int month = line.toInt();
        AddMonth(month)->Unserialize(file);
    }

    file->close();
}
