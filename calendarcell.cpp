#include "calendarcell.h"

CalendarCell::CalendarCell(QWidget* parent, QString text, QColor bgColor): QFrame(parent)
{
    setFrameShape(QFrame::Box);

    layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    label = new QLabel(text, this);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);

    originalBackgroundColor = bgColor;
    setBackgroundColor(bgColor);

    setLayout(layout);
}

void CalendarCell::setBackgroundColor(QColor color) {
    int r, g, b;
    color.getRgb(&r, &g, &b);
    styleSheetString += QString("background-color: rgb(%1, %2, %3);").arg(QString::number(r), QString::number(g), QString::number(b));
    setStyleSheet(styleSheetString);
}

void CalendarCell::setColor(QColor color) {
    int r, g, b;
    color.getRgb(&r, &g, &b);
    styleSheetString += QString("color: rgb(%1, %2, %3);").arg(QString::number(r), QString::number(g), QString::number(b));
    setStyleSheet(styleSheetString);
}

void CalendarCell::setText(QString text) {
    label->setText(text);
}

bool CalendarCell::isClickable() {
    return !label->text().isEmpty();
}

void CalendarCell::setDate(QDate date) {
    this->date = date;
}

QDate CalendarCell::getDate() {
    return date;
}

VacationStatus CalendarCell::getVacationStatus() {
    return vacationStatus;
}

void CalendarCell::setVacationStatus(VacationStatus vacation) {
    this->vacationStatus = vacation;
}

void CalendarCell::mousePressEvent(QMouseEvent*) {
    if (isClickable()) {
        auto menu = new QMenu(this);
        menu->setStyleSheet("color: rgb(0, 0, 0);"
                            "background-color: rgb(255, 255, 255);");
        menu->setAttribute(Qt::WA_DeleteOnClose);

        if (vacationStatus == NONE) {
            auto addVacationAction = new QAction(menu);
            addVacationAction->setText("Add Vacation");
            connect(addVacationAction, SIGNAL(triggered()), this, SLOT(newFullVacation()));

            auto addHalfVacationAction = new QAction(menu);
            addHalfVacationAction->setText("Add Half Vacation");
            connect(addHalfVacationAction, SIGNAL(triggered()), this, SLOT(newHalfVacation()));

            menu->addAction(addVacationAction);
            menu->addAction(addHalfVacationAction);
        } else {
            auto remVacationAction = new QAction(menu);
            remVacationAction->setText("Remove Vacation");
            connect(remVacationAction, SIGNAL(triggered()), this, SLOT(remVacation()));

            menu->addAction(remVacationAction);
        }

        menu->exec(QCursor::pos());
    }
}

void CalendarCell::newFullVacation() {
    setBackgroundColor(QColor(200, 0, 0));

    vacationStatus = FULL;
    emit(updated(this));
}

void CalendarCell::newHalfVacation() {
    setBackgroundColor(QColor(255, 165, 0));

    vacationStatus = HALF;
    emit(updated(this));
}

void CalendarCell::remVacation() {
    setBackgroundColor(originalBackgroundColor);
    vacationStatus = NONE;
    emit(updated(this));
}
