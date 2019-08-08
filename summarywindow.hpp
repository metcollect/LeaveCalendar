#ifndef SUMMARYWINDOW_HPP
#define SUMMARYWINDOW_HPP

#include <QGridLayout>
#include <QLabel>
#include <QDialog>

#include "calendarview.hpp"

class Summary : public QFrame
{
    Q_OBJECT
private:
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* employeeName = new QLabel(this);
    QLabel* vacationCount = new QLabel(this);

public:
    Summary(QString name, double count) {
        setFrameStyle(QFrame::Box);
        employeeName->setAlignment(Qt::AlignCenter);
        vacationCount->setAlignment(Qt::AlignCenter);

        employeeName->setText(name);
        vacationCount->setText("Vacations taken: " + QString::number(count));

        layout->addWidget(employeeName);
        layout->addWidget(vacationCount);
    }
};

class SummaryWindow : public QDialog
{
    Q_OBJECT
public:
    explicit SummaryWindow(QWidget *parent = nullptr) : QDialog(parent) {
        setAttribute(Qt::WA_DeleteOnClose);
        setModal(true);
    }
    QGridLayout* getLayout() {
        return layout;
    }

private:
    QGridLayout* layout = new QGridLayout(this);
};

#endif // SUMMARYWINDOW_HPP
