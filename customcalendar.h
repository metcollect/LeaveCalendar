#ifndef CUSTOMCALENDAR_H
#define CUSTOMCALENDAR_H

#include "calendarcell.h"
#include "data.h"

#include <algorithm>
#include <QComboBox>
#include <QDate>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

class CustomCalendar : public QWidget
{
    Q_OBJECT
public:
    explicit CustomCalendar(QWidget *parent = nullptr);
    QVector<QString> employees;
    ~CustomCalendar();
    int getYear();

private:
    QGridLayout* calendar;
    QComboBox* employeeDropdown, *yearDropdown;
    void populateEmployeeDropdown();
    QLabel* vacationDaysLabel;
    QVector<QVector<CalendarCell*>> calendarCells;
    void loadCellData();
    void newEmployee(QString);
    void remEmployee();
    void changeYear(int year);

public slots:
    void updateTotalVacationDays(CalendarCell* cell);
};

#endif // CUSTOMCALENDAR_H
