#include "widget.h"
#include "ui_widget.h"
#include <QLibrary>
#include <qDebug>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    mfrc522 = new MFRC522(this);
}

Widget::~Widget() {
    delete ui;
}

void Widget::on_beforeClose_clicked() {
    rc522Thread->repeta = false;
}

void Widget::on_switcOn_clicked() {
     mfrc522->PCD_Init();
/*     qDebug()<< "BEFORE test ";
     if(mfrc522->PCD_PerformSelfTest()){
         qDebug()<< "test initial Merge";
     } else {
         qDebug()<< "test initial NUUUUUU Merge";
     } */
     rc522Thread = new atpRc522Thread(mfrc522, this);
     rc522Thread->repeta = true;
     rc522Thread->start();

}
