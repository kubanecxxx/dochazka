#ifndef DOJIZDENI_H
#define DOJIZDENI_H

#include <QWidget>
#include <QRadioButton>

namespace Ui {
class dojizdeni;
}

class ClassDay;
class ClassMonth;

class dojizdeni : public QWidget
{
    Q_OBJECT

public:
    explicit dojizdeni(ClassMonth** month,ClassDay ** day, QWidget *parent = 0);
    ~dojizdeni();
    void refresh();

private:
    Ui::dojizdeni *ui;
    ClassDay ** plonkDay;
    ClassMonth **plonkMonth;
    const int count;
    QVector<QRadioButton*> radios;
    void recount();

private slots:
    void radioClicked(bool checked);

};

#endif // DOJIZDENI_H
