/****************************************************************************
 *                                                                          *
 *   GBCR                                                                   *
 *   Copyright (C) 2021 Ivo Filot <ivo@ivofilot.nl>                         *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as         *
 *   published by the Free Software Foundation, either version 3 of the     *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public license      *
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>. *
 *                                                                          *
 ****************************************************************************/

#include <QApplication>
#include <QSurfaceFormat>
#include <QStatusBar>
#include <QDebug>
#include <QScreen>
#include <QStringList>

#include <iostream>
#include <iomanip>
#include <ctime>
#include <memory>

#include "mainwindow.h"
#include "config.h"

std::shared_ptr<QStringList> log_messages;

/**
 * @brief custom function for storing and display messages
 * @param type
 * @param context
 * @param msg
 */
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QString local_msg = QString(msg.toLocal8Bit());

    switch (type) {
    case QtDebugMsg:
        log_messages->append("[D] " + local_msg);
        std::cout << "[D] " << msg.toStdString() << std::endl;
        break;
    case QtInfoMsg:
        log_messages->append("[I] " + local_msg);
        std::cout << "[I] " << msg.toStdString() << std::endl;
        break;
    case QtWarningMsg:
        log_messages->append("[W] " + local_msg);
        std::cout << "[W] " << msg.toStdString() << std::endl;
        break;
    case QtCriticalMsg:
        log_messages->append("[C] " + local_msg);
        std::cerr << "[C] " << msg.toStdString() << std::endl;
        break;
    case QtFatalMsg:
        log_messages->append("[F] " + local_msg);
        std::cerr << "[F] " << msg.toStdString() << std::endl;
        break;
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    std::unique_ptr<MainWindow> mainWindow;
    log_messages = std::make_shared<QStringList>();

    try {
        // build main window
        qInstallMessageHandler(myMessageOutput);
        mainWindow = std::make_unique<MainWindow>(log_messages);
        mainWindow->setWindowTitle(QString(PROGRAM_NAME));
        mainWindow->resize(400,100);

    } catch(const std::exception& e) {
        // if any errors are caught in the process of starting up the application,
        // they will be printed in the execution.log file
        std::cerr << "Error detected!" << std::endl;
        std::cerr << e.what() << std::endl;
        std::cerr << "Abnormal closing of program." << std::endl;
    }

    mainWindow->show();

    return app.exec();
}
