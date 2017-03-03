#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMap>
#include <MFRC522.h>
#include <atprc522thread.h>

#include <atpft232spi.h>



namespace Ui {
    class Widget;
}

class Widget : public QWidget {
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();


private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Widget *ui;
    atpFt232Spi *atpftdi;
    MFRC522 *mfrc522;
    atpRc522Thread *rc522Thread;
};

#endif // WIDGET_H
