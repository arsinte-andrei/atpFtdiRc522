#include "widget.h"
#include "ui_widget.h"
#include <QLibrary>
#include <qDebug>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);

    atpftdi = new atpFt232Spi();

}

Widget::~Widget() {
    delete ui;
}

void Widget::on_pushButton_clicked() {
    if(ui->pushButton->text()=="ON"){
        ui->pushButton->setText("OFF");
        atpftdi->writeGpioPin(0,0x11111111);
    } else {
        ui->pushButton->setText("ON");
        atpftdi->writeGpioPin(0,0x00000000);
    }

}
