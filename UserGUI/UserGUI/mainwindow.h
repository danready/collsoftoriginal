// GNU General Public License Agreement
// Copyright (C) 2015-2016 Daniele Berto daniele.fratello@gmail.com
//
// CollSoft is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software Foundation.
// You must retain a copy of this licence in all copies.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// ---------------------------------------------------------------------------------

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QString>
#include "logfileusergui.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_ExpertModeButton_1_clicked();

    void on_EnableUserGUI_Button_1_clicked();

    void on_EnableUpdateButton_1_clicked();

    void on_actionConnect_triggered();

    void readTcpData();

    void error_tcp(QAbstractSocket::SocketError sock_error);

    void on_HomingButton_1_clicked();

    void on_ExpertModeButton_2_clicked();

    void on_ExpertModeButton_3_clicked();

    void on_EnableUpdate_button_2_clicked();

    void on_EnableUpdate_button_3_clicked();

    void on_EnableUserGUI_Button_2_clicked();

    void on_EnableUserGUI_Button_3_clicked();

    void on_HomingButton_2_clicked();

    void on_HomingButton_3_clicked();

    void on_actionQuit_triggered();

    void on_checkBox_all_clicked();

    void on_StartButton_1_clicked();

    void on_StartButton_2_clicked();

    void on_StartButton_3_clicked();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;

    //Variable to records if the user is enabled to use octagonal movimentation interface
    bool enabled_octagonal = false;

    //Variable to records if the user is enabled to use aymmetric movimentation interface
    bool enabled_asymmetric = false;

    //Variable to records if the user is enabled to use singular movimentation interface
    bool enabled_singular = false;

    //Singleton pointer to write to the UserGUI log file
    LogFileUserGui* main_window_log_file;

    //Socket to communicate with the CollSoft server
    QTcpSocket* _pSocket;

    //This function initializes the CollSoft server
    //Main steps:
    //1. Sends "connect /dev/ttyUSB0" command
    //2. Sends "load_encoder_from_file" command
    void AutomaticSetting();

    //This function checks the status of the leds saved in prevalent_color array and changes the color of the associate resuming figure.
    void CheckPrevalentColor();

    //Array to record the actual color of a sled.
    //0 == green (okay)
    //-1 == yellow (position mismath)
    //-2 == red (disconnected)
    int prevalent_color[14];

};

#endif // MAINWINDOW_H
