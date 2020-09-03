#include "dialogfind.h"
#include "ui_dialogfind.h"

DialogFind::DialogFind(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFind)
{
    ui->setupUi(this);
    setFixedSize(400, 150);
}

DialogFind::~DialogFind()
{
    delete ui;
}