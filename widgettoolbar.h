#ifndef WIDGETTOOLBAR_H
#define WIDGETTOOLBAR_H

#include <QWidget>
#include "ui_widgettoolbar.h"

namespace Ui {
class WidgetToolBar;
}

class WidgetToolBar : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetToolBar(QWidget *parent = 0);
    ~WidgetToolBar();
    Ui::WidgetToolBar *ui;
    
public slots:
    void SaveDataToFile();

private:
    void LoadDataFromFile();

signals:
    void DovolenaChanged(int value);
};

#endif // WIDGETTOOLBAR_H
