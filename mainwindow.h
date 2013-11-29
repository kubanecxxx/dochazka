#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "classday.h"
#include "classmonth.h"
#include "classyear.h"
#include "widgettoolbar.h"
#include "libxl.h"
#include "dojizdeni.h"

namespace Ui {
class MainWindow;
}

class QTableWidgetItem;
class QTextTable;
class QLineEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionSave_triggered();
    void on_actionPrehled_triggered();
    void on_table_itemChanged(QTableWidgetItem *item);
    void on_editOdchod2_textEdited(const QString &arg1);
    void on_editPrichod2_textEdited(const QString &arg1);
    void on_editOdchod1_textEdited(const QString &arg1);
    void on_editPrichod1_textEdited(const QString &arg1);
    void on_calendarWidget_clicked(const QDate &date);
    void on_checkDovolena_clicked(bool checked);
    void on_actionPrint_triggered();
    void on_dovolenaChanged(int value);
    void on_actionTiskJirasko_triggered();
    void on_editKorekce_textEdited(const QString &arg1);
    void on_editRucne_textEdited(const QString &arg1);
    void on_actionFrankova_triggered();
    void on_checkSvatek_clicked(bool checked);
    void on_spinKorekce_editingFinished();
    void on_spinKorekce_valueChanged(double arg1);
    void on_prehledDialogFinished(int);
    void on_spinAnglictina_valueChanged(double arg1);

private:
    Ui::MainWindow *ui;

    void fillFormDay(const ClassDay & day);
    void fillFormMonth(ClassMonth & month);
    void recolorCalendar(ClassMonth & month,QDate date);
    void setDovolena(bool enabled);
    void setSvatek(bool enabled);
    void AddRow();
    void printDay(const ClassDay & day, QTextTable * table);
    void printDayPrescas(const ClassDay & day, QTextTable * table);
    void printDayPrescasXls(const ClassDay & day, int & line, libxl::Sheet * sheet);

    void fillForm(void);

    ClassDay * PlonkDay;
    ClassMonth * PlonkMonth;
    ClassYear * year;
    QByteArray mByteArray;

    void SetTime(const QString & arg, QTime & time,QObject * widget);
    void LoadCombos(const ClassDay & day);

    WidgetToolBar * tool;
    void closeEvent(QCloseEvent *evt);
    QString Suffix(float hodiny);

    dojizdeni * doj;
};

#endif // MAINWINDOW_H
