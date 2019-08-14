#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QInputDialog>
#include <QJsonDocument>
#include <QMainWindow>
#include <QMessageBox>
#include <QLayout>
#include <QCalendarWidget>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QStandardPaths>

#include "calendarview.hpp"
#include "editemployeewindow.hpp"
#include "employee.hpp"
#include "summarywidget.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    enum VacationStatus {
        NONE = 0,
        FULL = 1,
        HALF = 2,
        SICK = 3
    };

    CalendarView *calendar;
    QComboBox *employeeDropdown, *yearDropdown;
    QLabel *vacationDaysLabel, *sickDaysLabel;


    QVector<Employee*> employees;
    QMap<VacationStatus, QColor> colors = {
        std::pair<VacationStatus, QColor>(FULL, QColor(255, 0, 0)),
        std::pair<VacationStatus, QColor>(HALF, QColor(255, 255, 0)),
        std::pair<VacationStatus, QColor>(SICK, QColor(0, 255, 0))
    };
    Employee* companyEmployee = nullptr;
    Employee* currentEmployee = companyEmployee;
    int startingMonth;
    int currentYear;

    void loadCellData();
    void populateEmployeeDropdown();
    void getVacationCount(Employee *employee, double *vacations, double *sickDays);
    void updateStatistics();
    void createSummaryWindow();
    void createColorConfigWindow();

    bool read();
    void write();

private slots:
    void cellClicked();
    void cellUpdated(CalendarCell *cell, VacationStatus status);
};

#endif // MAINWINDOW_H
