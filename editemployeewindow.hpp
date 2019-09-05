#ifndef EDITEMPLOYEEWINDOW_H
#define EDITEMPLOYEEWINDOW_H

#include "employee.hpp"

#include <QDateTimeEdit>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class EditEmployeeWindow : public QDialog
{
    Q_OBJECT
public:
    explicit EditEmployeeWindow(Employee *employee, QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Leave Calendar");
        setAttribute(Qt::WA_DeleteOnClose);
        setModal(true);

        const auto layout = new QGridLayout(this);
        layout->setColumnMinimumWidth(0, 200);

        const auto nameLabel = new QLabel("Name");
        const auto startDateLabel = new QLabel("Start Date");

        layout->addWidget(nameLabel, 0, 0);
        layout->addWidget(startDateLabel, 0, 1);

        const auto nameEdit = new QLineEdit(employee->getName());
        const auto startDateEdit = new QDateTimeEdit(QDate::currentDate());
        startDateEdit->setCalendarPopup(true);

        if (employee->getName() == "") {
            nameEdit->setPlaceholderText("New Employee");
        } else {
            startDateEdit->setDate(employee->getStartDate());
        }

        layout->addWidget(nameEdit, 1, 0);
        layout->addWidget(startDateEdit, 1, 1);

        auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Apply);

        auto cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
        connect(cancelButton, &QPushButton::clicked, this, &EditEmployeeWindow::reject);
        auto confirmButton = buttonBox->button(QDialogButtonBox::Apply);
        connect(confirmButton, &QPushButton::clicked, this, [&, employee, nameEdit, startDateEdit]() {
            employee->setName(nameEdit->text());
            employee->setStartDate(startDateEdit->date());
            qDebug() << "Accepted!";
            accept();
        });

        layout->addWidget(buttonBox, 2, 0, 1, 2);
    }
};

#endif // EDITEMPLOYEEWINDOW_H
