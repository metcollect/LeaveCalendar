#ifndef EDITEMPLOYEEWINDOW_H
#define EDITEMPLOYEEWINDOW_H

#include "employee.hpp"

#include <QDateEdit>
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
        layout = new QGridLayout(this);
        layout->setColumnMinimumWidth(0, 200);
        setWindowTitle("Leave Calendar");

        auto nameLabel = new QLabel("Name");
        auto startDateLabel = new QLabel("Start Date");

        layout->addWidget(nameLabel, 0, 0);
        layout->addWidget(startDateLabel, 0, 1);

        nameEdit = new QLineEdit(employee->getName());
        startDateEdit = new QDateEdit(QDate::currentDate());

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
        connect(confirmButton, &QPushButton::clicked, this, [&, employee]() mutable {
            employee->setName(nameEdit->text());
            employee->setStartDate(startDateEdit->date());
            qDebug() << "Accepted!";
            accept();
        });

        layout->addWidget(buttonBox, 2, 0, 1, 2);
    }
private:
    QGridLayout *layout;

    QLineEdit *nameEdit;
    QDateEdit *startDateEdit;
};

#endif // EDITEMPLOYEEWINDOW_H
