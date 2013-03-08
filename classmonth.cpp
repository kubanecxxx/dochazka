#include "classmonth.h"

ClassMonth::ClassMonth()
{
}

ClassDay * ClassMonth::AddDay(QDate date)
{
    ClassDay * day = new ClassDay;
    days.insert(date,day);
    return day;
}
