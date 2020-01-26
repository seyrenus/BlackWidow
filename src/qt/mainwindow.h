#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "base.hpp"
#include "dbprocessor.hpp"
#include "jsonparser.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionReset_triggered();

private:
    Ui::MainWindow *ui;
    DBProcessor *dbp;
    JsonParser *jsonp;
};
#endif // MAINWINDOW_H
