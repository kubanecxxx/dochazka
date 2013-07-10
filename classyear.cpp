#include "classyear.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QDomDocument>
#include <QDomNode>

ClassYear::ClassYear():
    volneDovolene(10)   //načitat z konfiguráku
{
    //načist ze souboru
    QString cesta = QFileInfo( QCoreApplication::applicationFilePath() ).absolutePath();
    file = new QFile(cesta +  "/soubor.txt");
    xml = new QFile(cesta +  "/dochazka.xml");

    LoadXml();
    //LoadFile();
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

    if (month == NULL)
        month = AddMonth(mon);

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

void ClassYear::SaveXml() const
{
    QDomDocument doc;
    QDomElement el = doc.createElement("xml");
    QDomElement year = doc.createElement("rok");

    year.setAttribute("cislo",2013);
    el.appendChild(year);

    QMapIterator<int, ClassMonth *> it(mesice);
    while(it.hasNext())
    {
        it.next();
        //číslo měsice
        it.value()->SaveXml(&year);
        //year.appendChild(mon);
    }

    doc.appendChild(el);
    QString str = doc.toString();
    qDebug() << str;
    xml->open(QFile::WriteOnly);
    xml->write(doc.toString().toUtf8());
    xml->close();
}
void ClassYear::LoadXml()
{
    if (!xml->open(QFile::ReadOnly))
        return;

    mesice.clear();

    QDomDocument doc;
    QString str = QString::fromUtf8(xml->readAll());
    xml->close();
    doc.setContent(str);

    QDomElement element = doc.documentElement();
    QDomElement rok = element.firstChildElement("rok");
    QDomElement mesic =  rok.firstChildElement("month");

    while (mesic.isElement())
    {
        //do something
        QString je =  mesic.attribute("number");
        ClassMonth * mon = GetMonth(je.toInt());
        mon->LoadXml(&mesic);

        //next
        mesic = mesic.nextSiblingElement("month");
    }


}

void ClassYear::SaveFile() const
{
    file->copy("backup.txt");
    file->open(QFile::WriteOnly);

    //počet měsicu
    //QTextStream stream()
    file->write(QString("%1 \n").arg(mesice.count()).toLatin1());
    QMapIterator<int, ClassMonth *> it(mesice);
    while(it.hasNext())
    {
        it.next();
        QString line;;
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
    if (!file->open(QFile::ReadOnly))
        return;
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

int ClassYear::GetVolnaDovolena() const
{
    int dovolena = volneDovolene;
    QMapIterator<int, ClassMonth *> it(mesice);
    while(it.hasNext())
    {
        it.next();
        dovolena -= it.value()->GetVybranaDovolena();
    }

    return dovolena;
}
