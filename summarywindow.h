#ifndef SUMMARYWINDOW_H
#define SUMMARYWINDOW_H

#include <QCloseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "customcalendar.h"
#include "data.h"

class Summary : public QFrame
{
    Q_OBJECT
private:
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* employeeName = new QLabel(this);
    QLabel* vacationCount = new QLabel(this);

public:
    Summary(QString employee, double count) {
        setFrameStyle(QFrame::Box);
        employeeName->setAlignment(Qt::AlignCenter);
        vacationCount->setAlignment(Qt::AlignCenter);

        employeeName->setText(employee);
        vacationCount->setText("Vacations taken: " + QString::number(count));

        layout->addWidget(employeeName);
        layout->addWidget(vacationCount);
    }
};

class SummaryWindow : public QDialog
{
    Q_OBJECT
public:
    explicit SummaryWindow(int year, QWidget *parent = nullptr);

signals:

public slots:
};

#endif // SUMMARYWINDOW_H
