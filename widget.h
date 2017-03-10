#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMap>
#include <MFRC522.h>
#include <atprc522thread.h>

namespace Ui {
    class Widget;
}

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_beforeClose_clicked();
    void on_switcOn_clicked();

private:
    Ui::Widget *ui;
    MFRC522 *mfrc522;
    atpRc522Thread *rc522Thread;
};

#endif // WIDGET_H
