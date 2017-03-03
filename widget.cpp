#include "widget.h"
#include "ui_widget.h"
#include <QLibrary>
#include <qDebug>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    atpftdi = new atpFt232Spi();
    mfrc522 = new MFRC522(this);

}

Widget::~Widget() {
    delete ui;
}

void Widget::on_pushButton_2_clicked(){
    atpftdi->initSpiComunication();
}

void Widget::on_pushButton_clicked() {
    if(ui->pushButton->text()=="ON"){
        ui->pushButton->setText("OFF");
        atpftdi->writeGpioPin(FT_PIN_C01, FT_PIN_HI);
    } else {
        ui->pushButton->setText("ON");
        atpftdi->writeGpioPin(FT_PIN_C01, FT_PIN_LOW);
    }

}

void Widget::on_pushButton_3_clicked() {
    if(ui->pushButton_3->text()=="ON"){
        ui->pushButton_3->setText("OFF");
        atpftdi->writeGpioPin(FT_PIN_C02, FT_PIN_HI);
    } else {
        ui->pushButton_3->setText("ON");
        atpftdi->writeGpioPin(FT_PIN_C02, FT_PIN_LOW);
    }
}


void Widget::on_pushButton_4_clicked() {
    mfrc522->PCD_Init();

    rc522Thread = new atpRc522Thread(mfrc522, this);
    rc522Thread->start();
}
