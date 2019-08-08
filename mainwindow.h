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
#include "employee.hpp"
#include "summarywindow.hpp"

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
        HALF = 2
    };

    QWidget *wid;
    QGridLayout *layout;

    CustomCalendar *calendar;
    QComboBox *employeeDropdown, *yearDropdown;
    QLabel *vacationDaysLabel;


    QVector<Employee*> employees;
    QMap<VacationStatus, QColor> colors = {
        std::pair<VacationStatus, QColor>(FULL, QColor(255, 0, 0)),
        std::pair<VacationStatus, QColor>(HALF, QColor(255, 255, 0))
    };
    Employee* companyEmployee = nullptr;
    Employee* currentEmployee = companyEmployee;
    int startingMonth;
    int currentYear;

    void loadCellData();
    void populateEmployeeDropdown();
    double getVacationCount(Employee* employee);
    void createSummaryWindow();
    void updateStatistics();
    void createEmployeeConfigWindow();

    bool read();
    void write();

private slots:
    void cellClicked();
    void cellUpdated(CalendarCell* cell, VacationStatus status);
};

#endif // MAINWINDOW_H
