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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "enablepasswordusergui.h"
#include "ui_enablepasswordusergui.h"
#include "enablepasswordexpertmode.h"
#include "ui_enablepasswordexpertmode.h"
#include "generaldialog.h"
#include "ui_generaldialog.h"
#include "logfileusergui.h"
#include "doublechecking.h"
#include "ui_doublechecking.h"
#include "DefineGeneral.h"
#include "about.h"
#include "ui_about.h"
#include <QtMath>
#include <QTcpSocket>
#include <QProcess>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <sstream>
#include <utility>

using namespace std;

//Support function to analyze the messages sent to this GUI from server.
//This function return a pointer to the second word of the argument string.
//Ex. FindPointer("get_par 2") returns a pointer to 2.
char * FindPointer (char *yytext)
{
    int i = 0;
    char *punt;
    while (yytext[i] != ' ' && yytext[i] != '\t' && i<strlen(yytext)-1)
        i++;
    while ((yytext[i] == ' ' || yytext[i] == '\t') && i<strlen(yytext)-1)
        i++;
    punt = yytext + i;
    return punt;
}

//Support function to separate the commands contained in a message.
std::vector<std::string> explode(std::string const & s, char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); )
    {
        result.push_back(std::move(token));
    }

    return result;
}

//This function initialize the CollSoft server
//Main steps:
//1. Sends "connect /dev/ttyUSB0" command
//2. Sends "load_encoder_from_file" command
//In order to communicate with the server is not used the global socket "_pSocket" but the local socket "_pSocket_1"
void MainWindow::AutomaticSetting()
{

    bool status_1 = false;
    bool status_2 = false;

    //Socket to communicate with CollSoft program
    QTcpSocket* _pSocket_1 = new QTcpSocket( this );

    //Disabling Nagle algorithm in order to have real time response from Server
    _pSocket_1->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    _pSocket_1->connectToHost(COLLSOFT_IP, (quint16)COLLSOFT_PORT);

    _pSocket_1->waitForConnected(1000);

    //Disabling Nagle algorithm in order to have real time response from Server
    _pSocket_1->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    if(_pSocket_1->waitForReadyRead(1000))
    {

        _pSocket_1->readAll(); //?


        //Procedura per aggiornare i dati
        QString tmp = "connect /dev/ttyUSB0";

        QByteArray data(tmp.toUtf8());
        _pSocket_1->write( data );


        if(_pSocket_1->waitForReadyRead(1000))
        {
            QByteArray data_1 = _pSocket_1->readAll();
            QString data_string(data_1);

            //If the server has sent a response to connect programmerpath command:
            QRegularExpression re1;
            QRegularExpressionMatch match;


            //If the server has sent a response to a get_pos_status
            re1.setPattern("(CONNECTION SUCCESS)|(CONNECTION done)|(CONNECTION start)");
            match = re1.match(data_string);

            if (re1.match(data_string).hasMatch()) {


                //GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#007700;\">Connection with the programmer okay!</b</html>");
                //dialog_1->ui->label->setPixmap(QPixmap(":Enable.png"));
                //dialog_1->show();


                status_1 = true;

                _pSocket_1->disconnectFromHost();

            }
            else
            {

                //GeneralDialog* dialog_1 = new GeneralDialog(data_string);
                //dialog_1->ui->label->setPixmap(QPixmap(":Disable.png"));
                //dialog_1->show();

                _pSocket_1->disconnectFromHost();

            }
        }
    }

    if (status_1 = true)
    {
        //Socket to communicate with CollSoft program
        QTcpSocket* _pSocket_1 = new QTcpSocket( this );

        //Disabling Nagle algorithm in order to have real time response from Server
        _pSocket_1->setSocketOption(QAbstractSocket::LowDelayOption, 1);

        _pSocket_1->connectToHost(COLLSOFT_IP, (quint16)COLLSOFT_PORT);

        _pSocket_1->waitForConnected(1000);


        if(_pSocket_1->waitForReadyRead(1000))
        {
            for (int i = 0; i < 10; i++)
            {
                _pSocket_1->waitForReadyRead(100);
                _pSocket_1->readAll();
            }

            //Procedura per aggiornare i dati
            QString tmp = "load_encoder_from_file";

            QByteArray data(tmp.toUtf8());
            _pSocket_1->write( data );


            if(_pSocket_1->waitForReadyRead(1000))
            {
                QByteArray data_1 = _pSocket_1->readAll();
                QString data_string(data_1);

                //If the server has sent a response to connect programmerpath command:
                QRegularExpression re1;
                QRegularExpressionMatch match;


                //If the server has sent a response to a get_pos_status
                re1.setPattern("Loading encoder values from file: okay");
                match = re1.match(data_string);

                if (re1.match(data_string).hasMatch()) {

                    GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#007700;\">Loading automatic setting okay!</b</html>");
                    dialog_1->ui->label->setPixmap(QPixmap(":Enable.png"));
                    dialog_1->show();

                    status_2 = true;

                    _pSocket_1->disconnectFromHost();

                }
                else
                {
                    GeneralDialog* dialog_1 = new GeneralDialog(data_string);
                    dialog_1->ui->label->setPixmap(QPixmap(":Disable.png"));
                    dialog_1->show();

                    _pSocket_1->disconnectFromHost();
                }
            }
        }
    }
}

//MainWindow constructor.
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    //prevalent_color("Red")
{
    ui->setupUi(this);

    //Obtaining an instance of LogFileUserGui singleton
    main_window_log_file = LogFileUserGui::Instance ();

    //Socket to communicate with CollSoft program
    _pSocket = new QTcpSocket( this );

    //Disabling Nagle algorithm in order to have real time response from Server
    _pSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    //Bindind CollSoft with readTcpData()
    connect( _pSocket, SIGNAL(readyRead()), SLOT(readTcpData()) );

    //Binding CollSoft socket with Error SLOT
    connect( _pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error_tcp(QAbstractSocket::SocketError)));

    //Setting prevalent_color array to -2 (==red, disconnected)
    int i;
    for (i = 0; i < 14; i++)
    {
        //-2 means red, disconnected
        prevalent_color[i] = -2;
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

//Slot to catch TCP/IP errors with CollSoft server.
//If TCPINLOGFILE == 1, these errors are written in the log file.
void MainWindow::error_tcp(QAbstractSocket::SocketError sock_error)
{
    if (TCPINLOGFILE == 1)
    {
        QString LogTmpString( "Error communicating with the server: " + _pSocket->errorString() );
        main_window_log_file->LogFileUserGui_WriteOnLogFile(LogTmpString.toUtf8().constData() );
    }

    //Communicating the error with the user
    GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Error communicating with CollSoft: " + _pSocket->errorString() + "</b</html>");
    dialog_1->show();
}

//This function check the status of the leds associated with a figure and consequently changes the color of the figure summary.
void MainWindow::CheckPrevalentColor()
{
    //First figure (sleds 1 to 4)
    int color = 0; //0 means green
    int i;
    for (i = 0; i < 4; i++)
    {
        if (color > prevalent_color[i])
        {
            color = prevalent_color[i];
        }
    }

    if (color == 0)
    {
        ui->Octagon_1->setPixmap(QPixmap(":octagon_green.png"));
        ui->SquareAsym_1->setPixmap(QPixmap(":Square_green.png"));
    }
    else if (color == -1)
    {
        ui->Octagon_1->setPixmap(QPixmap(":octagon_yellow.png"));
        ui->SquareAsym_1->setPixmap(QPixmap(":Square_yellow.png"));
    }
    else if (color == -2)
    {
        ui->Octagon_1->setPixmap(QPixmap(":octagon_red.png"));
        ui->SquareAsym_1->setPixmap(QPixmap(":Square_red.png"));
    }

    //Second figure (sleds 5 to 8)
    color = 0; //0 means green
    for (i = 4; i < 8; i++)
    {
        if (color > prevalent_color[i])
        {
            color = prevalent_color[i];
        }
    }

    if (color == 0)
    {
        ui->Octagon_2->setPixmap(QPixmap(":octagon_green.png"));
        ui->SquareAsym_2->setPixmap(QPixmap(":Square_green.png"));
    }
    else if (color == -1)
    {
        ui->Octagon_2->setPixmap(QPixmap(":octagon_yellow.png"));
        ui->SquareAsym_2->setPixmap(QPixmap(":Square_yellow.png"));
    }
    else if (color == -2)
    {
        ui->Octagon_2->setPixmap(QPixmap(":octagon_red.png"));
        ui->SquareAsym_2->setPixmap(QPixmap(":Square_red.png"));
    }

    //Third figure (sleds 9 to 12)
    color = 0; //0 means green
    for (i = 8; i < 12; i++)
    {
        if (color > prevalent_color[i])
        {
            color = prevalent_color[i];
        }
    }

    if (color == 0)
    {
        ui->Octagon_3->setPixmap(QPixmap(":octagon_green.png"));
        ui->SquareAsym_3->setPixmap(QPixmap(":Square_green.png"));
    }
    else if (color == -1)
    {
        ui->Octagon_3->setPixmap(QPixmap(":octagon_yellow.png"));
        ui->SquareAsym_3->setPixmap(QPixmap(":Square_yellow.png"));
    }
    else if (color == -2)
    {
        ui->Octagon_3->setPixmap(QPixmap(":octagon_red.png"));
        ui->SquareAsym_3->setPixmap(QPixmap(":Square_red.png"));
    }

    //Forth figure (sleds 13 to 14)
    color = 0; //0 means green
    for (i = 12; i < 14; i++)
    {
        if (color > prevalent_color[i])
        {
            color = prevalent_color[i];
        }
    }

    if (color == 0)
    {
        ui->Square_1->setPixmap(QPixmap(":Square_green.png"));
        ui->SquareAsym_4->setPixmap(QPixmap(":Square_green.png"));
    }
    else if (color == -1)
    {
        ui->Square_1->setPixmap(QPixmap(":Square_yellow.png"));
        ui->SquareAsym_4->setPixmap(QPixmap(":Square_yellow.png"));
    }
    else if (color == -2)
    {
        ui->Square_1->setPixmap(QPixmap(":Square_red.png"));
        ui->SquareAsym_4->setPixmap(QPixmap(":Square_red.png"));
    }

}

//Slot to read the incoming message of CollSoft server
void MainWindow::readTcpData()
{

    //Reading the message
    QByteArray data = _pSocket->readAll();

    QString data_string_tmp(data);

    //Extracting the commands from the message
    string tmp(data_string_tmp.toUtf8().constData());
    auto command_list = explode(tmp, '\n');

    //Passing through the command list
    for ( int tmpi = 0; tmpi < command_list.size() ; tmpi ++)
    {
        QString data_string(command_list[tmpi].c_str());

        //If TCPINLOGFILE == 1, the command is written in the log file.
        if (TCPINLOGFILE == 1)
        {
            QString LogTmpString ("Received this data from server: " + data_string);
            main_window_log_file->LogFileUserGui_WriteOnLogFile(LogTmpString.toUtf8().constData());
        }

        //If the server has sent a response to connect programmerpath command:
        QRegularExpression re1;
        QRegularExpressionMatch match;

        //If the server has sent a response to a get_pos_status
        re1.setPattern("^get_pos_status[ ][0-9]{1,2}[ ]-{0,1}[0-9]{1,5}");
        match = re1.match(data_string);

        if (re1.match(data_string).hasMatch()) {

            char* mypunt;
            mypunt = FindPointer(data_string.toLatin1().data());

            int num_drv_value = atoi(mypunt);

            //Be careful: hard coded values!
            if (num_drv_value < 1 || num_drv_value > 14)
            {
                return;
            }

            int status = atoi(FindPointer(mypunt));

            QString tmpStatus("led_");
            QString tmpStatus2 = QString::number(num_drv_value);
            tmpStatus.append(tmpStatus2);
            QLabel* CurrentLabelStatus = findChild<QLabel*>(tmpStatus);

            QString tmpStatus_2("Summary_");
            QString tmpStatus2_2 = QString::number(num_drv_value);
            tmpStatus_2.append(tmpStatus2_2);
            QLabel* CurrentLabelStatus_2 = findChild<QLabel*>(tmpStatus_2);


            QString tmpStatus_3("led_asym_");
            QString tmpStatus2_3 = QString::number(num_drv_value);
            tmpStatus_3.append(tmpStatus2_3);
            QLabel* CurrentLabelStatus_3 = findChild<QLabel*>(tmpStatus_3); //Point to NULL if it doesn't exist.


            QString tmpStatus_4("SummaryAsym_");
            QString tmpStatus2_4 = QString::number(num_drv_value);
            tmpStatus_4.append(tmpStatus2_4);
            QLabel* CurrentLabelStatus_4 = findChild<QLabel*>(tmpStatus_4);

            QString tmpStatus_5("SingularSled_");
            QString tmpStatus2_5 = QString::number(num_drv_value);
            tmpStatus_5.append(tmpStatus2_5);
            QLabel* CurrentLabelStatus_5 = findChild<QLabel*>(tmpStatus_5);

            //It is very important to record the current status of num_drv_value in order to
            //fill the big octagonal image
            prevalent_color[num_drv_value] = status;

            if (status == 0)
            {
                CurrentLabelStatus->setPixmap(QPixmap(":led_green.png"));
                CurrentLabelStatus_2->setPixmap(QPixmap(":Square_green.png"));

                if (num_drv_value % 2 == 1 || num_drv_value == 14) //this check is done in order to exclude inexistent led label.
                    CurrentLabelStatus_3->setPixmap(QPixmap(":led_green.png"));

                CurrentLabelStatus_4->setPixmap(QPixmap(":Square_green.png"));
                CurrentLabelStatus_5->setPixmap(QPixmap(":Square_green.png"));

            }
            else if (status == -1)
            {
                CurrentLabelStatus->setPixmap(QPixmap(":led_yellow.png"));
                CurrentLabelStatus_2->setPixmap(QPixmap(":Square_yellow.png"));

                if (num_drv_value % 2 == 1 || num_drv_value == 14) //this check is done in order to exclude inexistent led label.
                    CurrentLabelStatus_3->setPixmap(QPixmap(":led_yellow.png"));

                CurrentLabelStatus_4->setPixmap(QPixmap(":Square_yellow.png"));
                CurrentLabelStatus_5->setPixmap(QPixmap(":Square_yellow.png"));

            }
            else if (status == -2)
            {
                CurrentLabelStatus->setPixmap(QPixmap(":led_red.png"));
                CurrentLabelStatus_2->setPixmap(QPixmap(":Square_red.png"));

                if (num_drv_value % 2 == 1 || num_drv_value == 14) //this check is done in order to exclude inexistent led label.
                    CurrentLabelStatus_3->setPixmap(QPixmap(":led_red.png"));

                CurrentLabelStatus_4->setPixmap(QPixmap(":Square_red.png"));
                CurrentLabelStatus_5->setPixmap(QPixmap(":Square_red.png"));

            }
            else
            {
                CurrentLabelStatus->setPixmap(QPixmap(":led_red.png"));
                CurrentLabelStatus_2->setPixmap(QPixmap(":Square_red.png"));

                if (num_drv_value % 2 == 1 || num_drv_value == 14) //this check is done in order to exclude inexistent led label.
                    CurrentLabelStatus_3->setPixmap(QPixmap(":led_red.png"));

                CurrentLabelStatus_4->setPixmap(QPixmap(":Square_red.png"));
                CurrentLabelStatus_5->setPixmap(QPixmap(":Square_red.png"));

            }

            CheckPrevalentColor();

        }
    }
}

/*
void MainWindow::on_ChangePixmap_clicked()
{

    ui->led_1->setPixmap(QPixmap(":led_green.png"));
    ui->led_2->setPixmap(QPixmap(":led_green.png"));
    ui->led_3->setPixmap(QPixmap(":led_green.png"));
    ui->led_4->setPixmap(QPixmap(":led_green.png"));
    ui->led_5->setPixmap(QPixmap(":led_green.png"));
    ui->led_6->setPixmap(QPixmap(":led_green.png"));
    ui->led_7->setPixmap(QPixmap(":led_green.png"));
    ui->led_8->setPixmap(QPixmap(":led_green.png"));
    ui->led_9->setPixmap(QPixmap(":led_green.png"));
    ui->led_10->setPixmap(QPixmap(":led_green.png"));
    ui->led_11->setPixmap(QPixmap(":led_green.png"));
    ui->led_12->setPixmap(QPixmap(":led_green.png"));
    ui->led_13->setPixmap(QPixmap(":led_green.png"));
    ui->led_14->setPixmap(QPixmap(":led_green.png"));

    ui->Octagon_1->setPixmap(QPixmap(":octagon_green.png"));
    ui->Octagon_2->setPixmap(QPixmap(":octagon_green.png"));
    ui->Octagon_3->setPixmap(QPixmap(":octagon_green.png"));

    ui->Square_1->setPixmap(QPixmap(":Square_green.png"));

    ui->Summary_1->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_2->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_3->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_4->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_5->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_6->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_7->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_8->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_9->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_10->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_11->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_12->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_13->setPixmap(QPixmap(":Square_green.png"));
    ui->Summary_14->setPixmap(QPixmap(":Square_green.png"));

}
*/

//Slot to activate the expert mode from the octagon movimentation tab
void MainWindow::on_ExpertModeButton_1_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from octagonal movimentation tab...");

    bool accepted = false;
    EnablePasswordExpertMode* enable_1 = new EnablePasswordExpertMode();
    enable_1->setWindowModality(Qt::ApplicationModal);
    accepted = enable_1->exec();
    if (accepted == true && enable_1->GetAuthenticationValue() == 1)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from octagonal movimentation tab completed!");
        QProcess* process = new QProcess();
        QString file = "../../ExpertGUI/bin/ExpertGUI " + enable_1->GetAuthenticationUserName();
        process->start(file);
    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == 0)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from octagonal movimentation tab failed!");
        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, username and/or password incorrect!</b</html>");
        dialog_1->show();
    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == -1)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from singular movimentation tab failed!");
        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, error communicating with LDAP server!</b</html>");
        dialog_1->show();
    }
}

//Slot to activate the expert mode from asymmetric movimentation tab
void MainWindow::on_ExpertModeButton_2_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from asymmetric movimentation tab...");

    bool accepted = false;
    EnablePasswordExpertMode* enable_1 = new EnablePasswordExpertMode();
    enable_1->setWindowModality(Qt::ApplicationModal);
    accepted = enable_1->exec();
    if (accepted == true && enable_1->GetAuthenticationValue() == 1)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from asymmetric movimentation tab completed!");
        QProcess* process = new QProcess();
        QString file = "../../ExpertGUI/bin/ExpertGUI " + enable_1->GetAuthenticationUserName();
        process->start(file);
    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == 0)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from asymmetric movimentation tab failed!");
        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, username and/or password incorrect!</b</html>");
        dialog_1->show();
    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == -1)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from singular movimentation tab failed!");
        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, error communicating with LDAP server!</b</html>");
        dialog_1->show();
    }
}

//Slot to activate the expert mode from singular movimentation tab
void MainWindow::on_ExpertModeButton_3_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from singular movimentation tab...");

    bool accepted = false;
    EnablePasswordExpertMode* enable_1 = new EnablePasswordExpertMode();
    enable_1->setWindowModality(Qt::ApplicationModal);
    accepted = enable_1->exec();
    if (accepted == true && enable_1->GetAuthenticationValue() == 1)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from singular movimentation tab completed!");
        QProcess* process = new QProcess();
        QString file = "../../ExpertGUI/bin/ExpertGUI " + enable_1->GetAuthenticationUserName();
        process->start(file);
    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == 0)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from singular movimentation tab failed!");
        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, username and/or password incorrect!</b</html>");
        dialog_1->show();
    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == -1)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Attempt to access the expert mode from singular movimentation tab failed!");
        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, error communicating with LDAP server!</b</html>");
        dialog_1->show();
    }
}

//Slot to update the octagonal movimentation tab
void MainWindow::on_EnableUpdateButton_1_clicked()
{

    if (enabled_octagonal == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Octagonal update button clicked from unknown user");
        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, you have to be enabled first!</b</html>");
        dialog_1->show();
    }
    else
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Octagonal update button clicked from enabled user");
        //Procedura per aggiornare i dati
        QString tmp = "get_all_parameter\n";

        QByteArray data(tmp.toUtf8());
        _pSocket->write( data );
    }
}

//Slot to update the asymmetric movimentation tab
void MainWindow::on_EnableUpdate_button_2_clicked()
{

    if (enabled_asymmetric == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Asymmetric update button clicked from unknown user");
        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, you have to be enabled first!</b</html>");
        dialog_1->show();
    }
    else
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Asymmetric update button clicked from enabled user");
        //Procedura per aggiornare i dati
        QString tmp = "get_all_parameter\n";

        QByteArray data(tmp.toUtf8());
        _pSocket->write( data );
    }
}

//Slot to update the singular movimentation tab
void MainWindow::on_EnableUpdate_button_3_clicked()
{

    if (enabled_singular == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Singular update button clicked from unknown user");
        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, you have to be enabled first!</b</html>");
        dialog_1->show();
    }
    else
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Singular update button clicked from enabled user");
        //Procedura per aggiornare i dati
        QString tmp = "get_all_parameter\n";

        QByteArray data(tmp.toUtf8());
        _pSocket->write( data );
    }
}

//Slot to enable the octagonal movimentation tab
void MainWindow::on_EnableUserGUI_Button_1_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Octagonal movimentation tab authentication attempt...");

    bool accepted = false;

    enabled_octagonal = false;

    EnablePasswordUserGUI* enable_1 = new EnablePasswordUserGUI();
    enable_1->setWindowModality(Qt::ApplicationModal);
    accepted = enable_1->exec();

    if (accepted == true && enable_1->GetAuthenticationValue() == 1)
    {        
        ui->LockIcon_1->setPixmap(QPixmap(":lock_green.png"));

        enabled_octagonal = true;

        main_window_log_file->SetUserHeader(enable_1->GetAuthenticationUserName().toUtf8().constData());

        main_window_log_file->LogFileUserGui_WriteOnLogFile("Octagonal movimentation tab user enabled!");
    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == 0)
    {        
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Octagonal movimentation tab authentication attempt failed: username and/or password incorrect!");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, username and/or password incorrect!</b</html>");
        dialog_1->show();

        ui->LockIcon_1->setPixmap(QPixmap(":lock_red.png"));

    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == -1)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Octagonal movimentation tab authentication attempt failed: error communicating with LDAP server!");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, error communicating with LDAP server!</b</html>");
        dialog_1->show();

        ui->LockIcon_1->setPixmap(QPixmap(":lock_red.png"));
    }
    else if (accepted == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Octagonal movimentation tab authentication attempt failed because user doesn't accept to procede!");
    }
}

//Slot to enable the asymmetric movimentation tab
void MainWindow::on_EnableUserGUI_Button_2_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Asymmetric movimentation tab authentication attempt...");

    bool accepted = false;

    enabled_asymmetric = false;

    EnablePasswordUserGUI* enable_1 = new EnablePasswordUserGUI();
    enable_1->setWindowModality(Qt::ApplicationModal);
    accepted = enable_1->exec();

    if (accepted == true && enable_1->GetAuthenticationValue() == 1)
    {
        ui->LockIcon_2->setPixmap(QPixmap(":lock_green.png"));

        enabled_asymmetric = true;

        main_window_log_file->SetUserHeader(enable_1->GetAuthenticationUserName().toUtf8().constData());

        main_window_log_file->LogFileUserGui_WriteOnLogFile("Asymmetric movimentation tab user enabled!");
    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == 0)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Asymmetric movimentation tab authentication attempt failed: username and/or password incorrect!");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, username and/or password incorrect!</b</html>");
        dialog_1->show();

        ui->LockIcon_2->setPixmap(QPixmap(":lock_red.png"));

    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == -1)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Asymmetric movimentation tab authentication attempt failed: error communicating with LDAP server!");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, error communicating with LDAP server!</b</html>");
        dialog_1->show();

        ui->LockIcon_2->setPixmap(QPixmap(":lock_red.png"));
    }
    else if (accepted == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Asymmetric movimentation tab authentication attempt failed because user doesn't accept to procede!");
    }
}

//Slot to enable the syngular movimentation tab
void MainWindow::on_EnableUserGUI_Button_3_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Singular movimentation tab authentication attempt...");

    bool accepted = false;

    enabled_singular = false;

    EnablePasswordUserGUI* enable_1 = new EnablePasswordUserGUI();
    enable_1->setWindowModality(Qt::ApplicationModal);
    accepted = enable_1->exec();

    if (accepted == true && enable_1->GetAuthenticationValue() == 1)
    {
        ui->LockIcon_3->setPixmap(QPixmap(":lock_green.png"));

        enabled_singular = true;

        main_window_log_file->SetUserHeader(enable_1->GetAuthenticationUserName().toUtf8().constData());

        main_window_log_file->LogFileUserGui_WriteOnLogFile("Singular movimentation tab user enabled!");
    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == 0)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Singular movimentation tab authentication attempt failed: username and/or password incorrect!");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, username and/or password incorrect!</b</html>");
        dialog_1->show();

        ui->LockIcon_3->setPixmap(QPixmap(":lock_red.png"));

    }
    else if (accepted == true && enable_1->GetAuthenticationValue() == -1)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Singular movimentation tab authentication attempt failed: error communicating with LDAP server!");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, error communicating with LDAP server!</b</html>");
        dialog_1->show();

        ui->LockIcon_3->setPixmap(QPixmap(":lock_red.png"));
    }
    else if (accepted == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Singular movimentation tab authentication attempt failed because user doesn't accept to procede!");
    }
}

//Slot to start the homing procedure from the octagonal movimentation tab
void MainWindow::on_HomingButton_1_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from octagonal movimentation tab started...");

    if (enabled_octagonal == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from octagonal movimentation tab failed because user is not enabled...");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, you have to be enabled first!</b</html>");
        dialog_1->show();
    }
    else
    {
        bool accepted = false;

        DoubleChecking* SurePannel = new DoubleChecking(this);

        accepted = SurePannel->exec();

        if (accepted == true)
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from octagonal movimentation tab successfully started.");

            //Procedura per aggiornare i dati
            QString tmp = "homing_mult 1 2 3 4 5 6 7 8 9 10 11 12 13 14\n";

            QByteArray data(tmp.toUtf8());
            _pSocket->write( data );
        }
        else
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from octagonal movimentation tab failed because user doesn't accept to procede.");
        }
    }
}

//Slot to start the homing procedure from the asymmetric movimentation tab
void MainWindow::on_HomingButton_2_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from asymmetric movimentation tab started...");

    if (enabled_asymmetric == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from asymmetric movimentation tab failed because user is not enabled.");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, you have to be enabled first!</b</html>");
        dialog_1->show();
    }
    else
    {
        bool accepted = false;

        DoubleChecking* SurePannel = new DoubleChecking(this);

        accepted = SurePannel->exec();

        if (accepted == true)
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from asymmetric movimentation tab successfully started.");

            //Procedura per aggiornare i dati
            QString tmp = "homing_mult 1 2 3 4 5 6 7 8 9 10 11 12 13 14\n";

            QByteArray data(tmp.toUtf8());
            _pSocket->write( data );
        }
        else
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from asymmetric movimentation tab failed because user doesn't accept to procede.");
        }
    }
}

//Slot to start the homing procedure from the singular movimentation tab
void MainWindow::on_HomingButton_3_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from singular movimentation tab started...");

    if (enabled_singular == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from singular movimentation tab failed because user is not enabled...");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, you have to be enabled first!</b</html>");
        dialog_1->show();
    }
    else
    {
        bool accepted = false;

        DoubleChecking* SurePannel = new DoubleChecking(this);

        accepted = SurePannel->exec();

        if (accepted == true)
        {

            main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from singular movimentation tab successfully started...");

            //Procedura per aggiornare i dati
            QString tmp;

            QByteArray data;

            if (ui->checkBox_1->checkState() ==  Qt::Checked)
            {
                tmp = "homing 1\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_2->checkState() ==  Qt::Checked)
            {
                tmp = "homing 2\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_3->checkState() ==  Qt::Checked)
            {
                tmp = "homing 3\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_4->checkState() ==  Qt::Checked)
            {
                tmp = "homing 4\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_5->checkState() ==  Qt::Checked)
            {
                tmp = "homing 5\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_6->checkState() ==  Qt::Checked)
            {
                tmp = "homing 6\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_7->checkState() ==  Qt::Checked)
            {
                tmp = "homing 7\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_8->checkState() ==  Qt::Checked)
            {
                tmp = "homing 8\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_9->checkState() ==  Qt::Checked)
            {
                tmp = "homing 9\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_10->checkState() ==  Qt::Checked)
            {
                tmp = "homing 10\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_11->checkState() ==  Qt::Checked)
            {
                tmp = "homing 11\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_12->checkState() ==  Qt::Checked)
            {
                tmp = "homing 12\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_13->checkState() ==  Qt::Checked)
            {
                tmp = "homing 13\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_14->checkState() ==  Qt::Checked)
            {
                tmp = "homing 14\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }

            _pSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

        }
        else
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Homing procedure from singular movimentation tab successfully failed because user doesn't accept to procede.");
        }
    }
}

//Slot to connect the UserGUI to the server
void MainWindow::on_actionConnect_triggered()
{

    main_window_log_file->LogFileUserGui_WriteOnLogFile("Starting the connection to the server...");

    //AutomaticSetting();
    AutomaticSetting();

    _pSocket->connectToHost(COLLSOFT_IP, (quint16)COLLSOFT_PORT);

    if( _pSocket->waitForConnected(1000) ) {

        //New test
        //Disabling Nagle algorithm in order to have real time response from Server
        _pSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#007700;\">Connection with the server okay!</b</html>");
        dialog_1->ui->label->setPixmap(QPixmap(":Enable.png"));
        dialog_1->show();

    }
    else
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Connection to the server failed...");
    }

}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

//Function to activate all the checkBox if the user activate the "All" one.
void MainWindow::on_checkBox_all_clicked()
{
    if(ui->checkBox_all->checkState() ==  Qt::Unchecked)
    {
         ui->checkBox_1->setCheckState(Qt::Unchecked);
         ui->checkBox_2->setCheckState(Qt::Unchecked);
         ui->checkBox_3->setCheckState(Qt::Unchecked);
         ui->checkBox_4->setCheckState(Qt::Unchecked);
         ui->checkBox_5->setCheckState(Qt::Unchecked);
         ui->checkBox_6->setCheckState(Qt::Unchecked);
         ui->checkBox_7->setCheckState(Qt::Unchecked);
         ui->checkBox_8->setCheckState(Qt::Unchecked);
         ui->checkBox_9->setCheckState(Qt::Unchecked);
         ui->checkBox_10->setCheckState(Qt::Unchecked);
         ui->checkBox_11->setCheckState(Qt::Unchecked);
         ui->checkBox_12->setCheckState(Qt::Unchecked);
         ui->checkBox_13->setCheckState(Qt::Unchecked);
         ui->checkBox_14->setCheckState(Qt::Unchecked);
    }
    else
    {
        ui->checkBox_1->setCheckState(Qt::Checked);
        ui->checkBox_2->setCheckState(Qt::Checked);
        ui->checkBox_3->setCheckState(Qt::Checked);
        ui->checkBox_4->setCheckState(Qt::Checked);
        ui->checkBox_5->setCheckState(Qt::Checked);
        ui->checkBox_6->setCheckState(Qt::Checked);
        ui->checkBox_7->setCheckState(Qt::Checked);
        ui->checkBox_8->setCheckState(Qt::Checked);
        ui->checkBox_9->setCheckState(Qt::Checked);
        ui->checkBox_10->setCheckState(Qt::Checked);
        ui->checkBox_11->setCheckState(Qt::Checked);
        ui->checkBox_12->setCheckState(Qt::Checked);
        ui->checkBox_13->setCheckState(Qt::Checked);
        ui->checkBox_14->setCheckState(Qt::Checked);
    }
}

//Slot to start the octagonal movimentation procedure.
void MainWindow::on_StartButton_1_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure started from octagonal movimentation tab...");

    if (enabled_octagonal == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure from octagonal movimentation tab failed because user is not enabled.");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, you have to be enabled first!</b</html>");
        dialog_1->show();
    }
    else
    {        

        bool accepted = false;

        DoubleChecking* SurePannel = new DoubleChecking(this);

        accepted = SurePannel->exec();

        if (accepted == true)
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure from octagonal movimentation tab successfully started.");

            float radiants_tmp = qDegreesToRadians(ui->LineEditOpenAngle->text().toFloat());

            ////First octagon
            int first_octagon_distance_micron = ui->LineEditOpeningValue->text().toInt();

            int distance_value_tmp_step = qRound(first_octagon_distance_micron*SLED_CONVERSION_1);

            //Procedura per aggiornare i dati
            QString tmp = "move_to 1 " + QString::number(distance_value_tmp_step) + "\n";

            QByteArray data(tmp.toUtf8());
            _pSocket->write( data );

            distance_value_tmp_step = qRound(first_octagon_distance_micron*SLED_CONVERSION_2);
            //Procedura per aggiornare i dati
            tmp = "move_to 2 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            distance_value_tmp_step = qRound(first_octagon_distance_micron*SLED_CONVERSION_3);
            //Procedura per aggiornare i dati
            tmp = "move_to 3 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            distance_value_tmp_step = qRound(first_octagon_distance_micron*SLED_CONVERSION_4);
            //Procedura per aggiornare i dati
            tmp = "move_to 4 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            ////Second octagon
            int second_octagon_distance_micron = qRound(first_octagon_distance_micron + qTan(radiants_tmp)*120000);

            distance_value_tmp_step = qRound(second_octagon_distance_micron*SLED_CONVERSION_5);

            //Procedura per aggiornare i dati
            tmp = "move_to 5 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            distance_value_tmp_step = qRound(second_octagon_distance_micron*SLED_CONVERSION_6);
            //Procedura per aggiornare i dati
            tmp = "move_to 6 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            distance_value_tmp_step = qRound(second_octagon_distance_micron*SLED_CONVERSION_7);
            //Procedura per aggiornare i dati
            tmp = "move_to 7 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            distance_value_tmp_step = qRound(second_octagon_distance_micron*SLED_CONVERSION_8);
            //Procedura per aggiornare i dati
            tmp = "move_to 8 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            ////Third octagon
            int third_octagon_distance_micron = qRound(second_octagon_distance_micron + qTan(radiants_tmp)*240000);

            distance_value_tmp_step = qRound(third_octagon_distance_micron*SLED_CONVERSION_9);

            //Procedura per aggiornare i dati
            tmp = "move_to 9 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            distance_value_tmp_step = qRound(third_octagon_distance_micron*SLED_CONVERSION_10);
            //Procedura per aggiornare i dati
            tmp = "move_to 10 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            distance_value_tmp_step = qRound(third_octagon_distance_micron*SLED_CONVERSION_11);
            //Procedura per aggiornare i dati
            tmp = "move_to 11 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            distance_value_tmp_step = qRound(third_octagon_distance_micron*SLED_CONVERSION_12);
            //Procedura per aggiornare i dati
            tmp = "move_to 12 " + QString::number(distance_value_tmp_step) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            _pSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

        }
        else
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure from octagonal movimentation tab failed because user doesn't accept to procede.");
        }
    }
}

//Slot to start the asymmetric movimentation procedure.
void MainWindow::on_StartButton_2_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure started from asymmetric movimentation tab...");

    if (enabled_asymmetric == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure from asymmetric movimentation tab failed because user is not enabled.");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, you have to be enabled first!</b</html>");
        dialog_1->show();
    }
    else
    {
        bool accepted = false;

        DoubleChecking* SurePannel = new DoubleChecking(this);

        accepted = SurePannel->exec();

        if (accepted == true)
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure from asymmetric movimentation tab successfully started.");

            //Metterle tutte all'homing.
            //Aprire le sei slitte a 45° e muovere le altre 8 di qualcosa di corrispondente.
            //Procedura per aggiornare i dati
            QString tmp;
            QByteArray data;

            //////////////////////////45 degree sleds
            tmp = "move_to 2 " + QString::number(qRound(MAX_OPENING_MICRON*SLED_CONVERSION_2)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            tmp = "move_to 4 " + QString::number(qRound(MAX_OPENING_MICRON*SLED_CONVERSION_4)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            tmp = "move_to 6 " + QString::number(qRound(MAX_OPENING_MICRON*SLED_CONVERSION_6)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            tmp = "move_to 8 " + QString::number(qRound(MAX_OPENING_MICRON*SLED_CONVERSION_8)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            tmp = "move_to 10 " + QString::number(qRound(MAX_OPENING_MICRON*SLED_CONVERSION_10)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            tmp = "move_to 12 " + QString::number(qRound(MAX_OPENING_MICRON*SLED_CONVERSION_12)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );
            //////////////////////////45 degree sleds


            //AY
            tmp = "move_to 1 " + QString::number(qRound(ui->AY_LineEdit->text().toInt()*SLED_CONVERSION_1)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            //AX
            tmp = "move_to 3 " + QString::number(qRound(ui->AX_LineEdit->text().toInt()*SLED_CONVERSION_3)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            //BY
            tmp = "move_to 5 " + QString::number(qRound(ui->BY_LineEdit->text().toInt()*SLED_CONVERSION_5)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            //BX
            tmp = "move_to 7 " + QString::number(qRound(ui->BX_LineEdit->text().toInt()*SLED_CONVERSION_7)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            //CY
            tmp = "move_to 10 " + QString::number(qRound(ui->CY_LineEdit->text().toInt()*SLED_CONVERSION_10)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            //CX
            tmp = "move_to 12 " + QString::number(qRound(ui->CX_LineEdit->text().toInt()*SLED_CONVERSION_12)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            //DY
            tmp = "move_to 13 " + QString::number(qRound(ui->DY_LineEdit->text().toInt()*SLED_CONVERSION_13)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            //DX
            tmp = "move_to 14 " + QString::number(qRound(ui->DX_LineEdit->text().toInt()*SLED_CONVERSION_14)) + "\n";

            data = tmp.toUtf8();
            _pSocket->write( data );

            _pSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

        }
        else
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure from asymmetric movimentation tab failed because user doesn't accept to procede.");
        }
    }
}

//Slot to start the singular movimentation procedure
void MainWindow::on_StartButton_3_clicked()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure started from singular movimentation tab...");

    if (enabled_singular == false)
    {
        main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure from singular movimentation tab failed because user is not enabled.");

        GeneralDialog* dialog_1 = new GeneralDialog("<html><b style=\" font-size:14pt; color:#ff0000;\">Warning, you have to be enabled first!</b</html>");
        dialog_1->show();
    }
    else
    {
        bool accepted = false;

        DoubleChecking* SurePannel = new DoubleChecking(this);

        accepted = SurePannel->exec();

        if (accepted == true)
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure from singular movimentation tab successfully started.");

            QString tmp;
            QByteArray data;
            int distance_value_tmp_step;

            int distance_micron = ui->SingularSledMovimentationValue->text().toInt();

            if (ui->checkBox_1->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_1);
                tmp = "move_to 1 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_2->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_2);
                tmp = "move_to 2 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_3->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_3);
                tmp = "move_to 3 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_4->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_4);
                tmp = "move_to 4 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_5->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_5);
                tmp = "move_to 5 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_6->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_6);
                tmp = "move_to 6 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_7->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_7);
                tmp = "move_to 7 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_8->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_8);
                tmp = "move_to 8 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_9->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_9);
                tmp = "move_to 9 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_10->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_10);
                tmp = "move_to 10 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_11->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_11);
                tmp = "move_to 11 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_12->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_12);
                tmp = "move_to 12 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_13->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_13);
                tmp = "move_to 13 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }
            if (ui->checkBox_14->checkState() ==  Qt::Checked)
            {
                distance_value_tmp_step = qRound(distance_micron*SLED_CONVERSION_14);
                tmp = "move_to 14 " + QString::number(distance_value_tmp_step) + "\n";
                data = tmp.toUtf8();
                _pSocket->write( data );
            }

            _pSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

        }
        else
        {
            main_window_log_file->LogFileUserGui_WriteOnLogFile("Movimentation procedure from singular movimentation tab failed because the user doesn't accepted to procede.");
        }
    }
}

//Slot to print about information
void MainWindow::on_actionAbout_triggered()
{
    main_window_log_file->LogFileUserGui_WriteOnLogFile("About information request....");

    About* about = new About();
    about->exec();
}
