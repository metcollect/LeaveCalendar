#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle("Leave Calendar");

    auto viewMenu = new QMenu("View", this);
    auto viewSummaryAction = new QAction("View Summary", viewMenu);
    connect(viewSummaryAction, &QAction::triggered, this, &MainWindow::createSummaryWindow);
    viewMenu->addAction(viewSummaryAction);
    menuBar()->addMenu(viewMenu);

    calendar = new CustomCalendar(this);

    layout = new QGridLayout();
    layout->addWidget(calendar);

    wid = new QWidget(this);
    setCentralWidget(wid);

    wid->setLayout(layout);
}

void MainWindow::createSummaryWindow() {
    auto summaryWindow = new SummaryWindow(calendar->getYear(), this);
    summaryWindow->setModal(true);
    summaryWindow->show();
}

MainWindow::~MainWindow()
{
    delete layout;
}
