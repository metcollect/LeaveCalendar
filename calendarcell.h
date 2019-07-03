#ifndef CALENDARCELL_H
#define CALENDARCELL_H

#include <QDate>
#include <QDebug>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QObject>
#include <QPushButton>
#include <QString>

enum VacationStatus {
    NONE,
    FULL,
    HALF
};

class CalendarCell : public QFrame
{
    Q_OBJECT
public:
    CalendarCell(QWidget* parent, QString text = "", QColor bgColor = QColor(255, 255, 255));
    void setBackgroundColor(QColor color);
    void setColor(QColor color);
    void setText(QString text);
    bool isClickable();
    void setDate(QDate date);
    QDate getDate();
    VacationStatus getVacationStatus();
    void setVacationStatus(VacationStatus);
    void mousePressEvent(QMouseEvent* event) override;

private:
    QColor originalBackgroundColor;
    VacationStatus vacationStatus = NONE;
    QHBoxLayout* layout; 
    QDate date;
    QLabel* label;
    QString styleSheetString;

public slots:
    void newFullVacation();
    void newHalfVacation();
    void remVacation();

signals:
    void updated(CalendarCell* cell);
};

#endif // CALENDARCELL_H
