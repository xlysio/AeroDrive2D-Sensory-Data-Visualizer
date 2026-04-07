/**
 * @file main.cpp
 * @brief Creates the QApplication instance and displays the main window.
 */

#include "mainwindow.h"

#include <QApplication>

/**
 *  @brief Application's main function
 * @param argc Number of arguments.
 * @param argv Array of type string arguments.
 * @return Exit code returned by Qt.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QCoreApplication::exec();
}
