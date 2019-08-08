#ifndef EMPLOYEECONFIGWINDOW_HPP
#define EMPLOYEECONFIGWINDOW_HPP

#include <QDialog>
#include <QVBoxLayout>

class EmployeeConfigWindow : public QDialog {
    Q_OBJECT
public:
    explicit EmployeeConfigWindow(QWidget* parent = nullptr) : QDialog(parent) {
        setAttribute(Qt::WA_DeleteOnClose);
        setModal(true);
    }

    QVBoxLayout* getLayout() {
       return layout;
    }

private:
    QVBoxLayout* layout = new QVBoxLayout(this);
};

#endif // EMPLOYEECONFIGWINDOW_HPP
