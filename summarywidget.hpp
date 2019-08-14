#ifndef SUMMARYWINDOW_HPP
#define SUMMARYWINDOW_HPP

#include <QGridLayout>
#include <QLabel>
#include <QDialog>

#include "calendarview.hpp"

class SummaryWidget : public QFrame
{
    Q_OBJECT
private:
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *employeeName = new QLabel();
    QLabel *vacationCount = new QLabel();
    QLabel *sickDaysCount = new QLabel();

public:
    SummaryWidget(QString name, double vacations, double sickDays) {
        setFrameStyle(QFrame::Box);
        setMinimumHeight(100);
        setMinimumWidth(150);

        employeeName->setAlignment(Qt::AlignCenter);
        vacationCount->setAlignment(Qt::AlignCenter);
        sickDaysCount->setAlignment(Qt::AlignCenter);

        employeeName->setText(name);

        layout->addWidget(employeeName);
        if (vacations > 0) {
            vacationCount->setText("Vacations: " + QString::number(vacations));
            layout->addWidget(vacationCount);
        }
        if (sickDays > 0) {
            sickDaysCount->setText("Sick Days: " + QString::number(sickDays));
            layout->addWidget(sickDaysCount);
        }
    }
};

#endif // SUMMARYWINDOW_HPP
