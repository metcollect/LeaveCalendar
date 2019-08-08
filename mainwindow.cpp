#include "employeeconfigwindow.hpp"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    qDebug() << "Starting main window";

    setWindowTitle("Leave Calendar");

    auto viewMenu = new QMenu("View", this);
    auto viewSummaryAction = new QAction("View Summary", viewMenu);
    connect(viewSummaryAction, &QAction::triggered, this, &MainWindow::createSummaryWindow);
    viewMenu->addAction(viewSummaryAction);
    menuBar()->addMenu(viewMenu);

    wid = new QWidget(this);
    setCentralWidget(wid);

    // Add sub layouts to main layout
    auto layout = new QVBoxLayout(wid);

    // Top row layout
    auto topRow = new QHBoxLayout;

    // Employee selection widget
    employeeDropdown = new QComboBox(this);
    connect(employeeDropdown, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged), this, [this](QString text){
        for (auto employee : employees) {
            if (employee->getName() == text) {
                currentEmployee = employee;
                break;
            }
        }

        loadCellData();
    });

    // Year selection widget
    yearDropdown = new QComboBox(this);
    for (int i = 2019; i < 2119; i++) {
        yearDropdown->addItem(QString::number(i));
    }
    connect(yearDropdown, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged), this, [this](){
        currentYear = yearDropdown->currentText().toInt();
        calendar->setYear(currentYear);
        loadCellData();
    });

    layout->addLayout(topRow);

    // Add widgets to top bar
    topRow->addWidget(employeeDropdown);
    topRow->addStretch(1);
    topRow->addWidget(yearDropdown);

    qDebug() << "Creating Calendar";
    // Calendar Widget
    startingMonth = 4;

    calendar = new CustomCalendar(startingMonth);
    for (auto month : calendar->calendarCells) {
        for (auto cell : month) {
            connect(cell, &CalendarCell::clicked, this, &MainWindow::cellClicked);
        }
    }

    layout->addWidget(calendar, 1);

    // Bottom statistics
    auto statistics = new QHBoxLayout;
    vacationDaysLabel = new QLabel("Total Vacation Days: 0");
    statistics->addWidget(vacationDaysLabel);

    layout->addLayout(statistics);

    qDebug("Reading");
    read();

    qDebug() << "Setting company employee";

    for (auto employee : employees) {
        if (employee->getName() == "Company Holidays") {
            companyEmployee = employee;
            break;
        }
    }

    // Create company employee if it does not exist
    if (companyEmployee == nullptr) {
        companyEmployee = new Employee();
        companyEmployee->setName("Company Holidays");
        employees.push_back(companyEmployee);
    }

    currentEmployee = companyEmployee;

    qDebug() << "Setting year";
    currentYear = 2019;
    calendar->setYear(currentYear); // Populate cells with dates

    qDebug() << "Populating employee dropdown";
    populateEmployeeDropdown();

    qDebug() << "Finished constructor";
    createEmployeeConfigWindow();
}

void MainWindow::loadCellData() {
    qDebug() << "Loading cell data" << currentEmployee->getName();
    // Months
    for (auto row : calendar->calendarCells) {
        // Days
        for (auto cell : row) {
            auto date = cell->getDate();
            auto vacationStatus = NONE;

            if (companyEmployee->getVacationData().contains(date)) {
                vacationStatus = VacationStatus(companyEmployee->getVacationData()[date]);
            }


            if (currentEmployee->getVacationData().contains(date)) {
                vacationStatus = VacationStatus(currentEmployee->getVacationData()[date]);
                qDebug() << "Employee Holiday!" << date;
            }

            cell->setBackgroundColor(vacationStatus != NONE ? colors[vacationStatus] : cell->originalBackgroundColor);
        }
    }

    updateStatistics();
}

void MainWindow::populateEmployeeDropdown() {
    employeeDropdown->clear(); // Empty dropdown contents

    foreach (auto employee, employees) {
        qDebug() << "  " << employee->getName();
        employeeDropdown->addItem(employee->getName());
    }
}

double MainWindow::getVacationCount(Employee* employee) {
    qDebug() << "getVacationCount()";
    // Merge unique employee and company holidays
    auto dates = employee->getVacationData();
    for (auto date : dates.keys()) {
        dates[date] = dates[date];
    }

    // Remove dates for other years
    auto start = QDate(currentYear, startingMonth, 1);
    foreach (auto date, dates.keys()) {
        if (date < start || date >= start.addYears(1)) {
            dates.remove(date);
        }
    }

    // Separate Full days from half days
    auto fullDates = QVector<QDate>{};
    auto halfDates = QVector<QDate>{};
    foreach (auto date, dates.keys()) {
        if (dates[date] == FULL) {
            fullDates.push_back(date);
        } else {
            halfDates.push_back(date);
        }
    }

    return fullDates.count() + double(halfDates.count())/2; // Total holidays taken in current year
}

void MainWindow::cellClicked() {
    auto cell = qobject_cast<CalendarCell*>(sender());

    if (cell->getText().toInt() != 0) {
        auto menu = new QMenu(this);
        menu->setStyleSheet("color: rgb(0, 0, 0);"
                            "background-color: rgb(255, 255, 255);");
        menu->setAttribute(Qt::WA_DeleteOnClose);

        if (currentEmployee->getVacationData()[cell->getDate()] == NONE) {
            auto addVacationAction = new QAction(menu);
            addVacationAction->setText("Add Vacation");
            connect(addVacationAction, &QAction::triggered, this, [&](){
                cellUpdated(cell, FULL);
            });

            auto addHalfVacationAction = new QAction(menu);
            addHalfVacationAction->setText("Add Half Vacation");
            connect(addHalfVacationAction, &QAction::triggered, this, [&](){
                cellUpdated(cell, HALF);
            });

            menu->addAction(addVacationAction);
            menu->addAction(addHalfVacationAction);
        } else {
            auto remVacationAction = new QAction(menu);
            remVacationAction->setText("Remove Vacation");
            connect(remVacationAction, &QAction::triggered, this, [&](){
                cellUpdated(cell, NONE);
            });

            menu->addAction(remVacationAction);
        }

        menu->exec(QCursor::pos());
    }
}

void MainWindow::cellUpdated(CalendarCell* cell, VacationStatus status) {
    auto vd = currentEmployee->getVacationData();
    auto cvd = companyEmployee->getVacationData();
    auto date = cell->getDate();

    switch(status) {
    case FULL:
        cell->setBackgroundColor(QColor(255, 0, 0));
        if (status == cvd[date]) {
            vd.remove(date);
        } else {
            vd[date] = status;
        }
        break;
    case HALF:
        cell->setBackgroundColor(QColor(255, 255, 0));
        if (status == cvd[date]) {
            vd.remove(date);
        } else {
            vd[date] = status;
        }
        break;
    case NONE:
        cell->setBackgroundColor(cell->originalBackgroundColor);
        if (currentEmployee != companyEmployee && cvd.contains(date)) {
            vd[date] = status;
        } else {
            vd.remove(date);
        }
        break;
    }

    if (status == companyEmployee->getVacationData()[date]) {
        vd.remove(date);
    } else if(status != NONE){
        vd[date] = status;
    }

    currentEmployee->setVacationData(vd);

    updateStatistics();
}

void MainWindow::createSummaryWindow() {
    auto summaryWindow = new SummaryWindow(this);
    auto summaryLayout = summaryWindow->getLayout();

    int i = 0;
    foreach(auto employee, employees) {
        summaryLayout->addWidget(new Summary(employee->getName(), getVacationCount(employee)), i/4, i%4);
        i++;
    }

    summaryWindow->show();
}

void MainWindow::updateStatistics() {
    vacationDaysLabel->setText("Total Vacation Days: " + QString::number(getVacationCount(currentEmployee)));
}

void MainWindow::createEmployeeConfigWindow() {
    auto ecw = new EmployeeConfigWindow(this);
    auto ecwLayout = ecw->getLayout();

    for (auto employee : employees) {
        auto name = new QLabel(employee->getName());
        ecwLayout->addWidget(name);
    }

    auto newButton = new QPushButton("New");
    connect(newButton, &QPushButton::clicked, this, [&](){
        qDebug() << "New Employee!";
    });
    ecwLayout->addWidget(newButton);
    //ecwLayout->insertWidget(1000, newButton);

    ecw->show();
}

bool MainWindow::read() {
    auto appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile file(appDataLocation + "/vacations.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to load data!";
        return false;
    }

    employees.clear();

    auto doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray employeeArray = doc.object()["employees"].toArray();
    for (auto employee : employeeArray) {
        auto obj = employee.toObject();
        auto newEmployee = new Employee();
        newEmployee->read(obj);
        employees.push_back(newEmployee);
    }

    file.close();

    return true;
}

void MainWindow::write() {
    auto appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (!QDir(appDataLocation).exists()) {
        QDir().mkdir(appDataLocation);
    }

    QFile file(appDataLocation + "/vacations.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to save data!";
       return;
    }

    QJsonArray employeeData;
    for (auto employee : employees) {
        QJsonObject employeeObject;
        employee->write(employeeObject);
        employeeData.push_back(employeeObject);
    }

    QJsonObject em;
    em["employees"] = employeeData;
    QJsonDocument doc(em);

    file.write(doc.toJson());

    file.close();
}

MainWindow::~MainWindow()
{
    write();
}
