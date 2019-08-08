#ifndef CUSTOMCALENDAR_HPP
#define CUSTOMCALENDAR_HPP

#include <QColor>
#include <QDate>
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

class CalendarCell : public QFrame
{
    Q_OBJECT
public:
    CalendarCell(QString text = "", QColor bgColor = QColor(255, 255, 255), QWidget* parent = nullptr) : QFrame(parent) {
        setFrameShape(Box);
        setAutoFillBackground(true);

        layout = new QHBoxLayout(this);
        layout->setMargin(0);

        label = new QLabel(text, this);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);

        originalBackgroundColor = bgColor;
        setBackgroundColor(bgColor);
    }

    void setBackgroundColor(QColor color) {
        pal.setColor(QPalette::Window, color);
        setPalette(pal);
    }

    void setTextColor(QColor color) {
        pal.setColor(QPalette::WindowText, color);
        label->setPalette(pal);
    }

    void setText(QString text) {
        label->setText(text);
    }

    QString getText() {
        return label->text();
    }

    void setDate(QDate date) {
        this->date = date;
    }

    QDate getDate() {
        return date;
    }

    void mousePressEvent(QMouseEvent*) override {
        emit(clicked());
    }

    QColor originalBackgroundColor;

private:
    QHBoxLayout* layout;
    QDate date;
    QLabel* label;
    QPalette pal;

signals:
    void clicked();
};

class CustomCalendar : public QWidget
{
    Q_OBJECT
public:
    QVector<QVector<CalendarCell*>> calendarCells;

    explicit CustomCalendar(int startingMonth = 1, QWidget *parent = nullptr) : QWidget(parent) {
        this->startingMonth = startingMonth;

        // Set up calendar layout
        calendar = new QGridLayout(this);
        calendar->setSpacing(0);
        calendar->setMargin(0);

        // Day and Month names
        auto weekdays = QVector<QString>{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        for (int i = 0; i < weekdays.length(); i++) {
            weekdays[i].insert(1, '\n').insert(3, '\n');
        }
        auto months = QVector<QString>{"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

        // Corner Label
        auto cornerlabel = new CalendarCell("Month\\Weekday", QColor(0, 0, 100));
        cornerlabel->setTextColor(QColor(255, 255, 255));
        calendar->addWidget(cornerlabel, 0, 0);

        // Add weekday cells
        for(auto i = 0; i < 7*6; i++) {
            auto weekdayCell = new CalendarCell(weekdays[i%7], QColor(0, 0, 100));
            weekdayCell->setTextColor(QColor(255, 255, 255));
            calendar->setColumnMinimumWidth(i+1, 20);
            calendar->addWidget(weekdayCell, 0, i + 1);
        }

        // Months
        for (auto i = 0; i < 12; i++) {
            auto row = QVector<CalendarCell*>();
            // Days
            for (int j = 0; j < 7*6; j++) {
                auto cell = new CalendarCell("", i%2 ? white : altWhite);
                row.push_back(cell);
                calendar->addWidget(cell, i + 1, j + 1);
            }

            calendarCells.push_back(row); // Add month to vector
        }

        // Add month cells
        for(auto i = 0; i < 12; i++) {
            auto text = months[(i+startingMonth-1)%12];
            CalendarCell* month = new CalendarCell(text, i%2 ? white : altWhite);

            calendar->addWidget(month, i + 1, 0);
        }
    }

    void setYear(int selectedYear) {
        // Months
        auto i = 0;
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
            auto j = 1;
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
    }

private:
    QGridLayout* calendar;
    QColor white = QColor(255, 255, 255);
    QColor altWhite = QColor(225, 225, 225);

    int startingMonth;
};

#endif // CUSTOMCALENDAR_HPP
