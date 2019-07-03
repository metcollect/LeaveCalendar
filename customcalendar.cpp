#include "customcalendar.h"

CustomCalendar::CustomCalendar(QWidget *parent) : QWidget(parent)
{
    // Set up main layout
    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);

    // Top row layout
    auto topRow = new QHBoxLayout;

    // Employee selection widget
    employeeDropdown = new QComboBox(this);
    populateEmployeeDropdown();
    connect(employeeDropdown, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged), this, [this](){
        loadCellData();
    });

    // Add employee button
    auto addButton = new QPushButton(QIcon("res/add.png"), "", this);
    connect(addButton, &QPushButton::clicked, this, [this](){
        auto ret = QInputDialog::getText(this, "New Employee", "New employee name:");
        if (!ret.isEmpty()) {
            newEmployee(ret);
        }
    });

    // Remove employee button
    auto remButton = new QPushButton(QIcon("res/rem.png"), "", this);
    connect(remButton, &QPushButton::clicked, this, [this](){
        auto employee = employeeDropdown->currentText();
        if(employee != "Company Holidays") { // Dont allow removal of company holidays
            int ret = QMessageBox::question(this, "Remove Employee?", "Remove \"" + employee + "\"?", QMessageBox::No | QMessageBox::Yes);
            if (ret == QMessageBox::Yes) {
                remEmployee();
            }
        }
    });

    // Year selection widget
    yearDropdown = new QComboBox(this);
    for (int i = 2019; i < 2119; i++) {
        yearDropdown->addItem(QString::number(i));
    }
    connect(yearDropdown, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged), this, [this](){
        changeYear(yearDropdown->currentText().toInt());
    });

    // Add widgets to top bar
    topRow->addWidget(employeeDropdown);
    topRow->addWidget(addButton);
    topRow->addWidget(remButton);
    topRow->addStretch(1);
    topRow->addWidget(yearDropdown);

    // Set up calendar layout
    calendar = new QGridLayout;
    calendar->setMargin(0);
    calendar->setRowStretch(0, 0);
    calendar->setSpacing(0);

    // Day and Month names
    auto weekdays = QVector<QString>{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    for (int i = 0; i < weekdays.length(); i++) {
        weekdays[i].insert(1, '\n').insert(3, '\n');
    }
    auto months = QVector<QString>{"April", "May", "June", "July", "August", "September", "October", "November", "December", "January", "February", "March"};

    // Corner Label
    auto cornerlabel = new CalendarCell(this, "Month\\Weekday", QColor(0, 0, 100));
    cornerlabel->setColor(QColor(255, 255, 255));
    calendar->addWidget(cornerlabel, 0, 0);

    // Add weekday cells
    for(int i = 0; i < 7*6; i++) {
        auto weekdayCell = new CalendarCell(this, weekdays[i%7], QColor(0, 0, 100));
        weekdayCell->setColor(QColor(255, 255, 255));
        calendar->setColumnMinimumWidth(i+1, 20);
        calendar->addWidget(weekdayCell, 0, i + 1);
    }

    // Months
    for (auto i = 0; i < 12; i++) {
        // Alternate background color
        auto bgColor = QColor(255, 255, 255);
        if (i % 2 == 0) {
            bgColor = QColor(225, 225, 225);
        }

        auto row = QVector<CalendarCell*>();
        // Days
        for (int j = 0; j < 7*6; j++) {
            auto cell = new CalendarCell(this, "", bgColor);
            connect(cell, SIGNAL(updated(CalendarCell*)), this, SLOT(updateTotalVacationDays(CalendarCell*)));
            row.push_back(cell);
            calendar->addWidget(cell, i + 1, j + 1);
        }

        calendarCells.push_back(row); // Add month to vector
    }

    // Add month cells
    calendar->setColumnMinimumWidth(0, cornerlabel->width());
    for(size_t i = 0; i < 12; i++) {
        QColor bgColor;

        // Alternate backgorund color
        if (i % 2 == 0) {
            bgColor = QColor(225, 225, 225);
        } else {
            bgColor = QColor(255, 255, 255);
        }

        CalendarCell* month = new CalendarCell(this, months[int(i)], bgColor);

        calendar->addWidget(month, int(i + 1), 0);
    }

    // Bottom statistics
    auto statistics = new QHBoxLayout;
    vacationDaysLabel = new QLabel("Total Vacation Days: 0", this);
    statistics->addWidget(vacationDaysLabel);

    // Add sub layouts to main layout
    layout->addLayout(topRow);
    layout->addLayout(calendar, 1);
    layout->addLayout(statistics);

    // If successfully loaded data update employee dropdown
    if (load()) {
        populateEmployeeDropdown();
    }

    changeYear(getYear()); // Populate cells with dates
}

int CustomCalendar::getYear() {
    return yearDropdown->currentText().toInt();
}

void CustomCalendar::updateTotalVacationDays(CalendarCell* cell) {
    auto employee = employeeDropdown->currentText();
    auto date = cell->getDate();
    auto vacationStatus = cell->getVacationStatus();

    // Remove date from employee if it exists
    vacationData[employee].removeAll(QPair<QDate, int>(date, NONE));
    vacationData[employee].removeAll(QPair<QDate, int>(date, FULL));
    vacationData[employee].removeAll(QPair<QDate, int>(date, HALF));

    // Apply changes to vacationData
    if (employee == "Company Holidays") { // If company holidays is selected
        if (vacationStatus != NONE) {
            vacationData[employee].push_back(QPair<QDate, int>(date, vacationStatus));
        } else {
            // Remove redundant overwrite dates
            foreach (auto key, vacationData.keys()) {
                vacationData[key].removeAll(QPair<QDate, int>(date, vacationStatus));
            }
        }
    } else if (!vacationData["Company Holidays"].contains(QPair<QDate, int>(date, vacationStatus))) { // If company holidays is not selected and date is not a company holiday
        vacationData[employee].push_back(QPair<QDate, int>(date, vacationStatus));
    }

    vacationDaysLabel->setText("Total Vacation Days: " + QString::number(getVacationCount(employee, yearDropdown->currentText().toInt()))); // Update vacation count
}

void CustomCalendar::populateEmployeeDropdown() {

    employeeDropdown->clear(); // Empty dropdown contents

    // Add company holidays if it does not already exist
    if (!vacationData.keys().contains("Company Holidays")) {
        employeeDropdown->addItem("Company Holidays");
    }

    // Add employees to dropdown
    foreach (auto i, vacationData.keys()) {
        if (i != "") {
            employeeDropdown->addItem(i);
        }
        else {
            vacationData.remove(i);
        }
    }
}

void CustomCalendar::newEmployee(QString name) {
    vacationData[name] = QVector<QPair<QDate, int>>(); // Create employee's vacation data

    populateEmployeeDropdown(); // Update dropdown contents
    employeeDropdown->setCurrentText(name); // Change dropdown selection to new employee
}

void CustomCalendar::remEmployee() {
    vacationData.remove(employeeDropdown->currentText()); // Remove employee's vacation data

    populateEmployeeDropdown(); // Update dropdown contents
}

void CustomCalendar::loadCellData() {
    auto employee = employeeDropdown->currentText();

    // Months
    for (auto row : calendarCells) {
        // Days
        for (auto cell : row) {
            auto date = cell->getDate();

            // If day is an employee or company holiday set isVacation to true
            auto vacationStatus = NONE;
            auto companyStatus = NONE;
            auto employeeStatus = NONE;

            if (vacationData["Company Holidays"].contains(QPair<QDate, int>(date, FULL))) companyStatus = FULL;
            if (vacationData["Company Holidays"].contains(QPair<QDate, int>(date, HALF))) companyStatus = HALF;

            if (vacationData[employee].contains(QPair<QDate, int>(date, FULL))) employeeStatus = FULL;
            if (vacationData[employee].contains(QPair<QDate, int>(date, HALF))) employeeStatus = HALF;

            auto overrideNone = vacationData[employee].contains(QPair<QDate, int>(date, NONE));

            if (overrideNone) {
                vacationStatus = NONE;
            } else {
                vacationStatus = companyStatus;
                if (employeeStatus != NONE) {
                    vacationStatus = employeeStatus;
                }
            }

            // Update cell vacation status if needed
            if (cell->getVacationStatus() != vacationStatus) {
                if (vacationStatus == FULL)
                    cell->newFullVacation();
                else if (vacationStatus == HALF)
                    cell->newHalfVacation();
                else
                    cell->remVacation();
            }
        }
    }
}

/* Change cell values on year change */
void CustomCalendar::changeYear(int selectedYear) { // Selected year

    // Months
    int i = 0;
    foreach (auto row, calendarCells) {
        auto year = selectedYear;
        auto month = startingMonth + i;

        // Change year when month > 12 and offset month by 12
        if (month > 12) {
            year = selectedYear + 1;
            month -= 12;
        }

        auto day1 = QDate(year, month, 1).dayOfWeek();        // The day the first day of the month falls on
        day1 %= 7; // If day1 is sunday set to 0 to avoid an empty week
        auto monthDays = QDate(year, month, 1).daysInMonth(); // Days in the month

        // Days
        int j = 1;
        foreach (auto cell, row) {
            auto day = j - day1;     // Day is offset by day1

            auto text = QString();   // Default cell text
            auto date = QDate();     // Default cell date

            // If day is between 0 and days in the month, set text, date and clickable status
            if (day > 0 && day <= monthDays) {
                text = QString::number(day);
                date = QDate(year, month, day);
            }

            cell->setText(text);           // Set text
            cell->setDate(date);           // Set date

            j++;
        }

        i++;
    }

    // Load vacation data
    loadCellData();
}

CustomCalendar::~CustomCalendar() {
    save(); // Save on close
}
