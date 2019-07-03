#include "summarywindow.h"

SummaryWindow::SummaryWindow(int year, QWidget *parent) : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    auto layout = new QGridLayout(this);

    int i = 0;
    foreach(auto key, vacationData.keys()) {
        layout->addWidget(new Summary(key, getVacationCount(key, year)), i/4, i%4);
        i++;
    }
}
