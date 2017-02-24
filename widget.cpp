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
