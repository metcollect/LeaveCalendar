#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLayout>
#include <QCalendarWidget>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QTableView>

#include "customcalendar.h"
#include "summarywindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    QWidget* wid;
    QGridLayout* layout;

    CustomCalendar* calendar;

    void createSummaryWindow();
};

#endif // MAINWINDOW_H
