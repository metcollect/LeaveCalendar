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

    void setBackgroundColor(const QColor &color) {
        pal.setColor(QPalette::Window, color);
        setPalette(pal);
    }

    void setTextColor(const QColor &color) {
        pal.setColor(QPalette::WindowText, color);
        label->setPalette(pal);
    }

    void setText(const QString &text) {
        label->setText(text);
    }

    const QString getText() {
        return label->text();
    }

    void setDate(const QDate &date) {
        this->date = date;
    }

    const QDate &getDate() {
        return date;
    }

    void mousePressEvent(QMouseEvent*) override {
        emit(clicked());
    }

    const QColor &getOriginalBackgroundColor() {
        return originalBackgroundColor;
    }

private:
    QHBoxLayout* layout;
    QDate date;
    QLabel* label;
    QPalette pal;
    QColor originalBackgroundColor;

signals:
    void clicked();
};

class CalendarView : public QWidget
{
    Q_OBJECT
public:
    QVector<QVector<CalendarCell*>> calendarCells;

    explicit CalendarView(int startingMonth = 1, QWidget *parent = nullptr) : QWidget(parent) {
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
        const auto months = QVector<QString>{"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

        // Corner Label
        const auto cornerlabel = new CalendarCell("Month\\Weekday", QColor(0, 0, 100));
        cornerlabel->setTextColor(white);
        calendar->addWidget(cornerlabel, 0, 0);

        // Add weekday cells
        for(auto i = 0; i < 7*6; i++) {
            const auto weekdayCell = new CalendarCell(weekdays[i%7], QColor(0, 0, 100));
            weekdayCell->setTextColor(white);
            calendar->setColumnMinimumWidth(i+1, 25);
            calendar->addWidget(weekdayCell, 0, i + 1);
        }

        // Months
        for (auto i = 0; i < 12; i++) {
            auto row = QVector<CalendarCell*>();
            // Days
            for (int j = 0; j < 7*6; j++) {
                const auto cell = new CalendarCell("", i%2 ? white : altWhite);
                row.push_back(cell);
                calendar->addWidget(cell, i + 1, j + 1);
            }

            calendarCells.push_back(row); // Add month to vector
        }

        // Add month cells
        for(auto i = 0; i < 12; i++) {
            const auto text = months[(i+startingMonth-1)%12];
            const auto month = new CalendarCell(text, i%2 ? white : altWhite);
            calendar->setRowMinimumHeight(i + 1, 25);

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
            const auto monthDays = QDate(year, month, 1).daysInMonth(); // Days in the month

            // Days
            auto j = 1;
            foreach (auto cell, row) {
                const auto day = j - day1;     // Day is offset by day1

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
    const QColor white = QColor(255, 255, 255);
    const QColor altWhite = QColor(225, 225, 225);

    int startingMonth;
};

#endif // CUSTOMCALENDAR_HPP
