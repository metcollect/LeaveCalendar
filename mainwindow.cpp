#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    qDebug() << "Starting main window";

    setWindowTitle("Leave Calendar");

    const auto viewMenu = new QMenu("View", this);
    const auto viewSummaryAction = new QAction("View Summary", viewMenu);
    connect(viewSummaryAction, &QAction::triggered, this, &MainWindow::createSummaryWindow);
    viewMenu->addAction(viewSummaryAction);

    const auto editMenu = new QMenu("Edit", this);
    const auto editColorsAction = new QAction("Edit Colors", editMenu);
    connect(editColorsAction, &QAction::triggered, this, &MainWindow::createColorConfigWindow);
    editMenu->addAction(editColorsAction);

    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(editMenu);

    const auto wid = new QWidget(this);
    setCentralWidget(wid);

    // Add sub layouts to main layout
    const auto layout = new QVBoxLayout(wid);

    // Top row layout
    const auto topRow = new QHBoxLayout();

    // Employee selection widget
    employeeDropdown = new QComboBox(this);
    connect(employeeDropdown, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged), this, [this](QString text){
        qDebug() << "Selected Employee:" << text;
        for (const auto &employee : employees) {
            if (employee->getName() == text) {
                currentEmployee = employee;
                break;
            }
        }

        loadCellData();
    });

    const auto addButton = new QPushButton(QIcon("res/add.png"), "");
    connect(addButton, &QPushButton::clicked, this, [&](){
        const auto employee = new Employee();
        const auto eew = new EditEmployeeWindow(employee, this);
        const auto result = eew->exec();
        if (result == QDialog::Accepted) {
            employees.push_back(employee);
            populateEmployeeDropdown();
            employeeDropdown->setCurrentText(employee->getName());
        } else {
            delete employee;
        }
    });
    const auto editButton = new QPushButton(QIcon("res/edit.png"), "");
    connect(editButton, &QPushButton::clicked, this, [&](){
        if (currentEmployee != companyEmployee) {
            const auto &employee = currentEmployee;
            const auto eew = new EditEmployeeWindow(employee, this);
            const auto result = eew->exec();
            if (result == QDialog::Accepted) {
                populateEmployeeDropdown();
                employeeDropdown->setCurrentText(employee->getName());
            }
        }
    });
    const auto removeButton = new QPushButton(QIcon("res/rem.png"), "");
    connect(removeButton, &QPushButton::clicked, this, [&](){
        if (currentEmployee != companyEmployee) {
            const auto reply = QMessageBox::question(this, "Leave Calendar", "Delete " + currentEmployee->getName() + "?");
            if (reply == QMessageBox::Yes) {
                employees.removeAll(currentEmployee);
                populateEmployeeDropdown();
            }
        }
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
    topRow->addWidget(addButton);
    topRow->addWidget(editButton);
    topRow->addWidget(removeButton);
    topRow->addStretch(1);
    topRow->addWidget(yearDropdown);

    qDebug() << "Creating Calendar";
    // Calendar Widget
    startingMonth = 4;

    calendar = new CalendarView(startingMonth);
    for (const auto &month : calendar->calendarCells) {
        for (const auto &cell : month) {
            connect(cell, &CalendarCell::clicked, this, &MainWindow::cellClicked);
        }
    }

    layout->addWidget(calendar, 1);

    // Bottom statistics
    const auto statistics = new QHBoxLayout();
    vacationDaysLabel = new QLabel("Total Vacation Days: 0");
    sickDaysLabel = new QLabel("Total Sick Days: 0");
    statistics->addWidget(vacationDaysLabel);
    statistics->addStretch(1);
    statistics->addWidget(sickDaysLabel);

    layout->addLayout(statistics);

    read();

    qDebug() << "Setting company employee";

    for (const auto &employee : employees) {
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

    populateEmployeeDropdown();

    qDebug() << "Finished constructor";
}

void MainWindow::loadCellData() {
    qDebug() << "Loading cell data";
    // Months
    for (const auto &row : calendar->calendarCells) {
        // Days
        for (const auto &cell : row) {
            const auto date = cell->getDate();
            auto vacationStatus = NONE;

            if (companyEmployee->getVacationData().contains(date)) {
                vacationStatus = VacationStatus(companyEmployee->getVacationData()[date]);
            }

            if (currentEmployee->getVacationData().contains(date)) {
                vacationStatus = VacationStatus(currentEmployee->getVacationData()[date]);
            }

            if (cell->getDate() < currentEmployee->getStartDate()) {
                vacationStatus = NONE;
            }

            cell->setBackgroundColor(vacationStatus != NONE ? colors[vacationStatus] : cell->getOriginalBackgroundColor());
        }
    }

    updateStatistics();
}

void MainWindow::populateEmployeeDropdown() {
    qDebug() << "Populating employee dropdown";
    employeeDropdown->clear(); // Empty dropdown contents

    //QVector<QString> names;
    //for (const auto &employee : employees) {
    //    names.push_back(employee->getName());
    //}

    std::sort(employees.begin(), employees.end());

    for (const auto &employee : employees) {
        qDebug() << "\t" << employee->getName();
        employeeDropdown->addItem(employee->getName());
    }
}

void MainWindow::getVacationCount(Employee *employee, double *vacations, double *sickDays) {
    qDebug() << "Getting vacation count";
    // Merge unique employee and company holidays
    auto dates = companyEmployee->getVacationData();
    for (auto date : employee->getVacationData().keys()) {
        dates[date] = employee->getVacationData()[date];
    }

    // Remove dates for other years
    const auto start = QDate(currentYear, startingMonth, 1);
    foreach (const auto &date, dates.keys()) {
        if (date < start || date >= start.addYears(1) || date < currentEmployee->getStartDate()) {
            dates.remove(date);
        }
    }

    // Sort days
    auto fullDates = QVector<QDate>{};
    auto halfDates = QVector<QDate>{};
    auto sickDates = QVector<QDate>{};
    foreach (const auto &date, dates.keys()) {
        if (dates[date] == FULL) {
            fullDates.push_back(date);
        } else if (dates[date] == HALF) {
            halfDates.push_back(date);
        } else if (dates[date] == SICK) {
            sickDates.push_back(date);
        }
    }

    *vacations = fullDates.count() + double(halfDates.count())/2; // Total holidays taken in current year
    *sickDays = sickDates.count();
}

void MainWindow::cellClicked() {
    const auto cell = qobject_cast<CalendarCell*>(sender());
    const auto date = cell->getDate();

    const auto vd = currentEmployee->getVacationData();
    const auto cvd = companyEmployee->getVacationData();

    if (date < currentEmployee->getStartDate()) {
        return;
    }

    if (cell->getText().toInt() != 0) {
        const auto menu = new QMenu(this);
        menu->setAttribute(Qt::WA_DeleteOnClose);

        auto isEmployeeHoliday = cvd.contains(date) && cvd[date] != NONE;
        if (vd.contains(date)) {
            if (vd[date] == NONE) {
                isEmployeeHoliday = false;
            } else {
                isEmployeeHoliday = true;
            }
        }

        if (isEmployeeHoliday) {
            const auto remVacationAction = new QAction(menu);
            remVacationAction->setText("Remove");
            connect(remVacationAction, &QAction::triggered, this, [&](){
                cellUpdated(cell, NONE);
            });

            menu->addAction(remVacationAction);
        } else {
            const auto addVacationAction = new QAction(menu);
            addVacationAction->setText("Add Holiday");
            connect(addVacationAction, &QAction::triggered, this, [&](){
                cellUpdated(cell, FULL);
            });

            const auto addHalfVacationAction = new QAction(menu);
            addHalfVacationAction->setText("Add Half Holiday");
            connect(addHalfVacationAction, &QAction::triggered, this, [&](){
                cellUpdated(cell, HALF);
            });

            const auto addSickDayAction = new QAction(menu);
            addSickDayAction->setText("Add Sick Day");
            connect(addSickDayAction, &QAction::triggered, this, [&](){
                cellUpdated(cell, SICK);
            });

            menu->addAction(addVacationAction);
            menu->addAction(addHalfVacationAction);
            menu->addAction(addSickDayAction);
        }

        menu->exec(QCursor::pos());
    }
}

void MainWindow::cellUpdated(CalendarCell* cell, VacationStatus status) {
    auto vd = currentEmployee->getVacationData();
    auto cvd = companyEmployee->getVacationData();
    const auto date = cell->getDate();

    if (date < currentEmployee->getStartDate()) {
        return;
    }

    if (status != NONE) {
        cell->setBackgroundColor(colors[status]);
        if (status == cvd[date]) {
            vd.remove(date);
        } else {
            vd[date] = status;
        }
    } else {
        cell->setBackgroundColor(cell->getOriginalBackgroundColor());
        if (currentEmployee != companyEmployee && cvd.contains(date)) {
            vd[date] = status;
        } else {
            vd.remove(date);
        }
    }

    currentEmployee->setVacationData(vd);

    updateStatistics();
}

void MainWindow::updateStatistics() {
    qDebug() << "Updating Statistics";
    double vacations, sickDays;
    getVacationCount(currentEmployee, &vacations, &sickDays);
    vacationDaysLabel->setText("Total Vacation Days: " + QString::number(vacations));
    sickDaysLabel->setText("Total Sick Days: " + QString::number(sickDays));
}

void MainWindow::createSummaryWindow() {
    qDebug() << "Creating summary window";
    const auto summaryWindow = new QDialog(this);
    summaryWindow->setWindowTitle("Leave Calendar");
    summaryWindow->setAttribute(Qt::WA_DeleteOnClose);
    summaryWindow->setModal(true);

    // Dialog layout
    const auto summaryLayout = new QGridLayout(summaryWindow);
    summaryLayout->setSpacing(0);

    // Add a summary widget for every employee, company holidays populate the top row
    int i = 0;
    for (const auto &employee : employees) {
        double vacations, sickDays;
        getVacationCount(employee, &vacations, &sickDays);
        if (employee == companyEmployee) {
            summaryLayout->addWidget(new SummaryWidget(employee->getName(), vacations, sickDays), 0, 0, 1, 4);
        } else {
            summaryLayout->addWidget(new SummaryWidget(employee->getName(), vacations, sickDays), i/4+1, i%4);
            i++;
        }
    }

    // Buttons
    const auto bb = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(bb->button(QDialogButtonBox::Close), &QPushButton::clicked, summaryWindow, &QDialog::close);

    summaryLayout->addWidget(bb, i+1, 0, 1, 4);

    summaryWindow->exec();
}

void MainWindow::createColorConfigWindow() {
    const auto ccw = new QDialog(this);
    ccw->setWindowTitle("Leave Calendar");
    ccw->setAttribute(Qt::WA_DeleteOnClose);
    ccw->setModal(true);

    // Dialog layout
    const auto ccwLayout = new QGridLayout(ccw);

    // Collumn Labels
    auto nameLabel = new QLabel("Name");
    auto redLabel = new QLabel("Red");
    auto greenLabel = new QLabel("Green");
    auto blueLabel = new QLabel("Blue");
    ccwLayout->addWidget(nameLabel, 0, 0);
    ccwLayout->addWidget(redLabel, 0, 1);
    ccwLayout->addWidget(greenLabel, 0, 2);
    ccwLayout->addWidget(blueLabel, 0, 3);

    // ColorEdit struct
    struct ColorEdit {
        VacationStatus status;
        QLabel* label;
        QLineEdit* red;
        QLineEdit* green;
        QLineEdit* blue;
    };

    QVector<ColorEdit> colorEdits;

    int i = 1;
    for (const auto &status : colors.keys()) {
        auto name = "";

        switch(status) {
        case FULL:
            name = "Holiday";
            break;
        case HALF:
            name = "Half Holiday";
            break;
        case SICK:
            name = "Sick Day";
            break;
        default:
            break;
        }

        ColorEdit edit;

        edit.status = status;
        edit.label = new QLabel(name);
        edit.red = new QLineEdit(QString::number(colors[status].red()));
        edit.green = new QLineEdit(QString::number(colors[status].green()));
        edit.blue = new QLineEdit(QString::number(colors[status].blue()));
        ccwLayout->addWidget(edit.label, i, 0);
        ccwLayout->addWidget(edit.red, i, 1);
        ccwLayout->addWidget(edit.green, i, 2);
        ccwLayout->addWidget(edit.blue, i, 3);

        colorEdits.push_back(edit);

        i++;
    }

    // Buttons
    const auto bb = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    connect(bb->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, [&](){
        for (const auto &edit : colorEdits) {
            const auto color = QColor(edit.red->text().toInt(), edit.green->text().toInt(), edit.blue->text().toInt());
            colors[edit.status] = color;
            qDebug() << colors[edit.status];
        }

        ccw->accept();
        loadCellData();
    });
    connect(bb->button(QDialogButtonBox::Cancel), &QPushButton::clicked, ccw, &QDialog::reject);

    ccwLayout->addWidget(bb, i, 0, 1, 4);

    ccw->exec();
}

bool MainWindow::read() {
    qDebug() << "Reading";
    const auto appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // Load old save file if it exists
    QFile oldFile(appDataLocation + "/vacations.dat");
    if (oldFile.exists()) {
        qDebug() << "Found old save file!";
        if (!oldFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to load data!";
            return false;
        }

        QMap<QString, QVector<QPair<QDate, int>>> oldVacationData;
        QDataStream ds(&oldFile);
        ds.setVersion(QDataStream::Qt_5_0);
        ds >> oldVacationData;
        oldFile.close();

        // Remove old save file
        oldFile.remove();

        for (auto &name : oldVacationData.keys()) {
            const auto newEmployee = new Employee();
            newEmployee->setName(name);

            QMap<QDate, int> vacationData;

            for (auto &pair : oldVacationData[name]) {
                vacationData[pair.first] = pair.second;
            }

            newEmployee->setVacationData(vacationData);
            employees.push_back(newEmployee);
        }

        // Save to new save file
        write();
        return true;
    }


    QFile file(appDataLocation + "/vacations.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to load data!";
        return false;
    }

    employees.clear();

    const auto doc = QJsonDocument::fromJson(file.readAll());
    const QJsonArray employeeArray = doc.object()["employees"].toArray();
    for (const auto &employee : employeeArray) {
        const auto obj = employee.toObject();
        const auto newEmployee = new Employee();
        newEmployee->read(obj);
        employees.push_back(newEmployee);
    }

    const QJsonArray colorArray = doc.object()["colors"].toArray();
    for (const auto &value : colorArray) {
        const auto obj = value.toObject();
        const auto colorObj = obj["color"].toObject();
        const auto color = QColor(colorObj["r"].toInt(), colorObj["g"].toInt(), colorObj["b"].toInt());
        colors[VacationStatus(obj["status"].toInt())] = color;
    }

    file.close();

    return true;
}

void MainWindow::write() {
    qDebug() << "Writing";
    const auto appDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (!QDir(appDataLocation).exists()) {
        QDir().mkdir(appDataLocation);
    }

    QFile file(appDataLocation + "/vacations.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to save data!";
       return;
    }

    QJsonArray employeeData;
    for (const auto &employee : employees) {
        QJsonObject employeeObject;
        employee->write(employeeObject);
        employeeData.push_back(employeeObject);
    }

    QJsonArray colorData;
    for (const auto &status : colors.keys()) {
        QJsonObject object;
        object["status"] = status;

        QJsonObject rgb;
        rgb["r"] = colors[status].red();
        rgb["g"] = colors[status].green();
        rgb["b"] = colors[status].blue();

        object["color"] = rgb;
        colorData.push_back(object);
    }

    QJsonObject em;
    em["employees"] = employeeData;
    em["colors"] = colorData;
    const QJsonDocument doc(em);

    file.write(doc.toJson());

    file.close();
}

MainWindow::~MainWindow()
{
    write();
}
