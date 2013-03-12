#ifndef DIALOGPREHLED_H
#define DIALOGPREHLED_H

#include <QDialog>
#include "classmonth.h"

namespace Ui {
class DialogPrehled;
}

class DialogPrehled : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogPrehled(const ClassMonth& Month, QWidget *parent = 0);
    ~DialogPrehled();

public slots:
    void Refresh();
    
private:
    Ui::DialogPrehled *ui;
    const ClassMonth & month;
};

#endif // DIALOGPREHLED_H
