#ifndef MOJETABLE_H
#define MOJETABLE_H

#include <QTableWidget>

#define T_HODINY 0
#define T_MX 1
#define T_HLASENI 2
#define T_POZN 3
#define T_PRESCAS 4
#define T_NUCENO 5

class MojeTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit MojeTable(QWidget *parent = 0);
    
signals:
    
public slots:

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    typedef struct line_t
    {
        float hodiny;
        bool mx;
        quint32 hlaseni;
        QString poznamka;
    } line_t;

    QList<line_t> clipboard;
    QList<line_t> clipmirror;
    QTimer * timer;
    int sel;

private slots:
    void timeout(void);
};

#endif // MOJETABLE_H
