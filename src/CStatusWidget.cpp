#include "CStatusWidget.h"

CStatusWidget::CStatusWidget(QWidget *parent) :
    QWidget(parent)
{
    layout = new QHBoxLayout(this);
    layout->addWidget(&status);
}

void CStatusWidget::slotUpdate(QString value)
{
    status.setText(value);
}

