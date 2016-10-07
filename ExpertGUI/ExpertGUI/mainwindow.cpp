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
#include "dialog.h"
#include "ui_dialog.h"
#include "about.h"
#include "ui_about.h"
#include "alldriversettingscheck.h"
#include "ui_alldriversettingscheck.h"
#include <QTcpSocket>
#include <iostream>
#include <QIODevice>
#include <QRegularExpression>
#include <QPixmap>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <QDialog>
#include <unistd.h>

#include <vector>
#include <sstream>
#include <utility>

using namespace std;

//using std::cout;
//using std::endl;

#define TCPINLOGFILE 0 //1==yes tcp message in log file
                       //0 or other == no tcp message in log file

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

//This function is used to extract the commands contained in a TCP/IP message.
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

//This is the constructor of MainWindow class
MainWindow::MainWindow(char* user, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Obtaining an instance of LogFileExpertGUI singleton
    main_window_log_file = LogFileExpertGUI::Instance ();

    //Set the name of the user who has activated the interface.
    main_window_log_file->SetUserHeader(user);

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("User activated.");

    //AppendMessageToQtextEdit(ui->CheckExistenceLog, user);

    //AppendMessageToQtextEdit(ui->CheckExistenceLog, pass);

    //Socket to communicate with CollSoft program
    _pSocket = new QTcpSocket( this );

    //Socket to communicate with CheckExistence program
    _pSocket_existence = new QTcpSocket( this );

    //Disabling Nagle algorithm in order to have real time response from Server
    _pSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    _pSocket_existence->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    //Binding CollSoft socket with readTcpData() and with Error SLOT
    connect( _pSocket, SIGNAL(readyRead()), SLOT(readTcpData()) );
    connect( _pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Errore(QAbstractSocket::SocketError)));

    //Binding CheckExistence socket with readTcpData() and with Error SLOT
    connect( _pSocket_existence, SIGNAL(readyRead()), SLOT(readTcpData_existence()) );
    connect( _pSocket_existence, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(Errore_existence(QAbstractSocket::SocketError)));


    connect(ui->CommandString, SIGNAL(returnPressed()),ui->CommandStringButton,SIGNAL(clicked()));

    //Setting status label to red.
    ui->label_status1->setStyleSheet("QLabel { color : red; }");
    ui->label_status2->setStyleSheet("QLabel { color : red; }");
    ui->label_status3->setStyleSheet("QLabel { color : red; }");
    ui->label_Accepted->setStyleSheet("QLabel { color : red; }");
    ui->label_Accepted_Par->setStyleSheet("QLabel { color : red; }");
    ui->label_Accepted_Encode->setStyleSheet("QLabel { color : red; }");
    ui->label_statusAll->setStyleSheet("QLabel { color : red; }");

    ui->InternalEncoderValuesStatus->setStyleSheet("QLabel { color : red; }");
    ui->InternalParameterStatus->setStyleSheet("QLabel { color : red; }");
    ui->InternalSerialNumberStatus->setStyleSheet("QLabel { color : red; }");

    ui->LabelPositionStatus_1->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_2->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_3->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_4->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_5->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_6->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_7->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_8->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_9->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_10->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_11->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_12->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_13->setStyleSheet("QTextEdit { color : red; }");
    ui->LabelPositionStatus_14->setStyleSheet("QTextEdit { color : red; }");

    ui->LabelStatusDriver1->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver2->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver3->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver4->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver5->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver6->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver7->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver8->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver9->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver10->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver11->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver12->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver13->setStyleSheet("QLabel { color : red; }");
    ui->LabelStatusDriver14->setStyleSheet("QLabel { color : red; }");

    ui->LabelAllStatus_1->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_2->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_3->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_4->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_5->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_6->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_7->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_8->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_9->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_10->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_11->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_12->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_13->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllStatus_14->setStyleSheet("QLabel { color : red; }");

    ui->LabelAllPosStatus_1->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_2->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_3->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_4->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_5->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_6->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_7->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_8->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_9->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_10->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_11->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_12->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_13->setStyleSheet("QLabel { color : red; }");
    ui->LabelAllPosStatus_14->setStyleSheet("QLabel { color : red; }");

}

MainWindow::~MainWindow()
{
    delete ui;
}

//This slot is used to catch the errors from the TCP/IP communication between the ExpertGUI and the CollSoft server.
void MainWindow::Errore(QAbstractSocket::SocketError sock_error)
{
    QString data_string;
    data_string += "Error communicating with CollSoft server: " + _pSocket->errorString() + "\n";

    if (TCPINLOGFILE == 1)
    {
        main_window_log_file->LogFileExpertGUI_WriteOnLogFile(data_string.toUtf8().constData() );
    }

    AppendMessageToQtextEdit(ui->ExpertModeMessage, data_string);
    AppendMessageToQtextEdit(ui->CollSoftConnectionLog, data_string);

    ui->label_status2->setText("FAILED");
    ui->label_status2->setStyleSheet("QLabel { color : red; }");

    CheckGeneralStatus();

}

//This slot is used to catch the errors from the TCP/IP communication between the ExpertGUI and the CheckExistence server.
void MainWindow::Errore_existence(QAbstractSocket::SocketError sock_error)
{
    QString data_string;
    data_string += "Error communicating with CheckExistence server: " + _pSocket_existence->errorString() + "\n";

    if (TCPINLOGFILE == 1)
    {
        main_window_log_file->LogFileExpertGUI_WriteOnLogFile(data_string.toUtf8().constData() );
    }

    AppendMessageToQtextEdit(ui->CheckExistenceLog, data_string);

    ui->label_status1->setText("FAILED");
    ui->label_status1->setStyleSheet("QLabel { color : red; }");

    CheckGeneralStatus();

}

//This methods analyze the input sent to this GUI from the server
void MainWindow::readTcpData()
{
    //Reading the message
    QByteArray data = _pSocket->readAll();

    QString data_string(data);

    //If TCPINLOGFILE == 1 the message is printed in the log file.
    if (TCPINLOGFILE == 1)
    {
        QString LogTmpString ("Received this data from CollSoft server: " + data_string);

        main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LogTmpString.toUtf8().constData());
    }

    //Cut the message into the single commands.
    string tmp(data_string.toUtf8().constData());
    auto command_list = explode(tmp, '\n');

    //Some messages don't need to be cut.

    //Prints the list of the physical devices connected with the CollSoft server
    QRegularExpression re1("^Device list:");
    QRegularExpressionMatch match = re1.match(data_string);

    if (re1.match(data_string).hasMatch()) {

        AppendMessageToQtextEdit(ui->ProgrammerConnectionLog, data_string);

    }

    //If the server has sent a response to a read log file request:
    re1.setPattern("^Reading LogFile...");
    match = re1.match(data_string);

    if (re1.match(data_string).hasMatch()) {

        AppendMessageToQtextEdit(ui->GeneralConnectionLog, data_string);

    }

    //If the server has sent a response to loading encoder values from file command or to reading actual encoder values
    re1.setPattern("^Loading encoder values from file:");
    match = re1.match(data_string);

    if (re1.match(data_string).hasMatch()) {

        AppendMessageToQtextEdit(ui->GeneralConnectionLog, data_string);

    }

    //If the server has sent a response to a check_drv_assoc command.
    re1.setPattern("^Check Drv Assoc:");
    match = re1.match(data_string);

    if (re1.match(data_string).hasMatch()) {
        int accepted = 0;
        Dialog* dialog = new Dialog(this);
        dialog->setWindowModality(Qt::ApplicationModal);

        AppendMessageToQtextEdit(dialog->ui->CheckDrvRestitution, data_string);

        accepted = dialog->exec();
        if (accepted == 1)
        {
            QByteArray data("y"); // <-- fill with data
            _pSocket->write( data );
            ui->label_Accepted->setText("Serial Numbers Accepted");
            ui->label_Accepted->setStyleSheet("QLabel { color : green; }");
        }
        else
        {
            QByteArray data("n"); // <-- fill with data
            _pSocket->write( data );
            ui->label_Accepted->setText("Serial Numbers Not Accepted");
            ui->label_Accepted->setStyleSheet("QLabel { color : red; }");
        }
    }

    //If the server has sent a response to a check_par_assoc command.
    re1.setPattern("^Check Par Assoc:");
    match = re1.match(data_string);

    if (re1.match(data_string).hasMatch()) {
        int accepted = 0;
        Dialog* dialog = new Dialog(this);
        dialog->setWindowModality(Qt::ApplicationModal);

        AppendMessageToQtextEdit(dialog->ui->CheckDrvRestitution, data_string);

        accepted = dialog->exec();
        if (accepted == 1)
        {
            QByteArray data("y"); // <-- fill with data
            _pSocket->write( data );
            ui->label_Accepted_Par->setText("Parameters Accepted");
            ui->label_Accepted_Par->setStyleSheet("QLabel { color : green; }");
        }
        else
        {
            QByteArray data("n"); // <-- fill with data
            _pSocket->write( data );
            ui->label_Accepted_Par->setText("Parameters Not Accepted");
            ui->label_Accepted_Par->setStyleSheet("QLabel { color : red; }");
        }
    }

    //If the server has sent a response to a check_encode_assoc command.
    re1.setPattern("^Check Encode Assoc:");
    match = re1.match(data_string);

    if (re1.match(data_string).hasMatch()) {
        int accepted = 0;
        Dialog* dialog = new Dialog(this);
        dialog->setWindowModality(Qt::ApplicationModal);

        AppendMessageToQtextEdit(dialog->ui->CheckDrvRestitution, data_string);

        accepted = dialog->exec();
        if (accepted == 1)
        {
            QByteArray data("y"); // <-- fill with data
            _pSocket->write( data );
            ui->label_Accepted_Encode->setText("Encoder Values Accepted");
            ui->label_Accepted_Encode->setStyleSheet("QLabel { color : green; }");
        }
        else
        {
            QByteArray data("n"); // <-- fill with data
            _pSocket->write( data );
            ui->label_Accepted_Encode->setText("Encoder Values Not Accepted");
            ui->label_Accepted_Encode->setStyleSheet("QLabel { color : red; }");
        }
    }


    for ( int tmpi = 0; tmpi < command_list.size() ; tmpi ++)
    {
        QString data_string(command_list[tmpi].c_str());

        if (TCPINLOGFILE == 1)
        {
            QString LogTmpString ("Received this data from CollSoft server: " + data_string);

            main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LogTmpString.toUtf8().constData());
        }

        //If the server has sent a response to connect programmerpath command:
        re1.setPattern("^Connect:");
        match = re1.match(data_string);

        if (re1.match(data_string).hasMatch()) {

            AppendMessageToQtextEdit(ui->ProgrammerConnectionLog, data_string);

            //Checking if the connection status is okay
            QRegularExpression re2("(MODBUS CONNECTION SUCCESS)|(done)");
            QRegularExpressionMatch match = re2.match(data_string);

            if (re2.match(data_string).hasMatch()) {
                ui->label_status3->setText("OK");
                ui->label_status3->setStyleSheet("QLabel { color : green; }");
            }


        }

        //If the server has sent a response to a connection attempt
        re1.setPattern("^Welcome:");
        match = re1.match(data_string);

        if (re1.match(data_string).hasMatch()) {
            ui->label_status2->setText("OK");
            ui->label_status2->setStyleSheet("QLabel { color : green; }");

            AppendMessageToQtextEdit(ui->CollSoftConnectionLog, data_string);

        }


        //This message could be sent from the server when a check_position command is executed before
        //the user has executed a load_encoder_from file command.
        re1.setPattern("^Check position warning!");
        match = re1.match(data_string);

        if (re1.match(data_string).hasMatch()) {
            QString choose_parameter = data_string;
            About* tmp = new About();
            tmp->SetAboutLabel(choose_parameter);
            tmp->setWindowModality(Qt::ApplicationModal);
            tmp->exec();
        }


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

            QString tmpStatus("LabelPositionStatus_");
            QString tmpStatus2 = QString::number(num_drv_value);
            tmpStatus.append(tmpStatus2);
            QTextEdit* CurrentLabelStatus = findChild<QTextEdit*>(tmpStatus);

            QString tmpStatus_all("LabelAllPosStatus_");
            QString tmpStatus2_all = QString::number(num_drv_value);
            tmpStatus_all.append(tmpStatus2_all);
            QLabel* CurrentLabelStatus_all = findChild<QLabel*>(tmpStatus_all);


            if (status == 0)
            {
                CurrentLabelStatus->setText("Okay");
                CurrentLabelStatus_all->setText("Okay");

                CurrentLabelStatus->setStyleSheet("QTextEdit { color : green; }");
                CurrentLabelStatus_all->setStyleSheet("QLabel { color : green; }");
            }
            else if (status == -1)
            {
                CurrentLabelStatus->setText("Position mismatch");
                CurrentLabelStatus_all->setText("Pos. mismatch");

                CurrentLabelStatus->setStyleSheet("QTextEdit { color : orange; }");
                CurrentLabelStatus_all->setStyleSheet("QLabel { color : orange; }");
            }
            else if (status == -2)
            {
                CurrentLabelStatus->setText("Disconnected");
                CurrentLabelStatus_all->setText("Disconnected");

                CurrentLabelStatus->setStyleSheet("QTextEdit { color : red; }");
                CurrentLabelStatus_all->setStyleSheet("QLabel { color : red; }");
            }
            else
            {
                QString tmp_string = "Blocked in an invalid status ";
                tmp_string += QString::number(status);
                CurrentLabelStatus->setText(tmp_string);
                CurrentLabelStatus_all->setText("Invalid state");

                CurrentLabelStatus->setStyleSheet("QTextEdit { color : red; }");
                CurrentLabelStatus_all->setStyleSheet("QLabel { color : red; }");

            }

            AppendMessageToQtextEdit(ui->GeneralConnectionLog, data_string);

        }

        //A response a check_internal_status command. In particular, InternalStatusSerial records if the user has just executed
        //a check_drv_assoc command.
        re1.setPattern("^InternalStatusSerial: [01]$");
        match = re1.match(data_string);

        if (re1.match(data_string).hasMatch()) {

            AppendMessageToQtextEdit(ui->GeneralConnectionLog, data_string);

            if (data_string[22] == '1')
            {
                ui->InternalSerialNumberStatus->setText("Serial number: ok");
                ui->InternalSerialNumberStatus->setStyleSheet("QLabel { color : green; }");
            }
            else
            {
                ui->InternalSerialNumberStatus->setText("Serial number: failed");
                ui->InternalSerialNumberStatus->setStyleSheet("QLabel { color : red; }");
            }

        }

        //A response a check_internal_status command. In particular, InternalStatusSerial records if the user has just executed
        //a check_par_assoc command.
        re1.setPattern("^InternalStatusParameter: [01]$");
        match = re1.match(data_string);

        if (re1.match(data_string).hasMatch()) {

            AppendMessageToQtextEdit(ui->GeneralConnectionLog, data_string);

            if (data_string[25] == '1')
            {
                ui->InternalParameterStatus->setText("Parameter: ok");
                ui->InternalParameterStatus->setStyleSheet("QLabel { color : green; }");
            }
            else
            {
                ui->InternalParameterStatus->setText("Parameter: failed");
                ui->InternalParameterStatus->setStyleSheet("QLabel { color : red; }");
            }


        }

        //A response a check_internal_status command. In particular, InternalStatusSerial records if the user has just executed
        //a check_encode_assoc command.
        re1.setPattern("^InternalStatusEncoder: [01]$");
        match = re1.match(data_string);

        if (re1.match(data_string).hasMatch()) {

            AppendMessageToQtextEdit(ui->GeneralConnectionLog, data_string);

            if (data_string[23] == '1')
            {
                ui->InternalEncoderValuesStatus->setText("Encoder: ok");
                ui->InternalEncoderValuesStatus->setStyleSheet("QLabel { color : green; }");
            }
            else
            {
                ui->InternalEncoderValuesStatus->setText("Encoder: failed");
                ui->InternalEncoderValuesStatus->setStyleSheet("QLabel { color : red; }");
            }


        }

        //This message contains useful information about the parameters of a driver.
        re1.setPattern("^getpar[ ][0-9]{1,2}[ ]((MaxVel)|(VelHome)|(Acceleration)|(Deceleration)|(PhaseCurrent)|(AnalogOutput0))[ ]-{0,1}[0-9]{1,20}$");
        match = re1.match(data_string);

        if (re1.match(data_string).hasMatch()) {

            char* mypunt;
            mypunt = FindPointer(data_string.toLatin1().data());
            int num_drv_value = atoi(mypunt);
            mypunt = FindPointer(mypunt);
            char switch_char = mypunt[0];
            char switch_char2 = mypunt[1];
            mypunt = FindPointer(mypunt);
            int value = atoi(mypunt);

            //Be careful: hard coded values!
            if (num_drv_value < 1 || num_drv_value > 14)
            {
                return;
            }

            QString tmpStatus("LabelStatusDriver");
            QString tmpStatus2 = QString::number(num_drv_value);
            tmpStatus.append(tmpStatus2);
            QLabel* CurrentLabelStatus = findChild<QLabel*>(tmpStatus);


            QString tmpStatus_all("LabelAllStatus_");
            QString tmpStatus2_all = QString::number(num_drv_value);
            tmpStatus_all.append(tmpStatus2_all);
            QLabel* CurrentLabelStatus_all = findChild<QLabel*>(tmpStatus_all);

            //Case MaxVel
            if (switch_char == 'M')
            {
                QString tmp("LabelMaxVelSetup_");
                QString tmp3 = QString::number(num_drv_value);
                tmp.append(tmp3);
                QLabel* CurrentLabel = findChild<QLabel*>(tmp);
                QString tmp2 = QString::number(value);
                CurrentLabel->setText(tmp2);

                QString tmp_l("LineEditMaxVelSetup_");
                QString tmp3_l = QString::number(num_drv_value);
                tmp_l.append(tmp3_l);
                QLineEdit* CurrentLabel_l = findChild<QLineEdit*>(tmp_l);
                QString tmp2_l = QString::number(value);
                CurrentLabel_l->setText(tmp2_l);

                if (value != -1)
                {
                    CurrentLabelStatus->setText("Connected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : green; }");
                }
                else
                {
                    CurrentLabelStatus->setText("Disconnected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : red; }");
                }
            }

            //Case VelHome
            if (switch_char == 'V')
            {
                QString tmp("LabelVelHomeSetup_");
                QString tmp3 = QString::number(num_drv_value);
                tmp.append(tmp3);
                QLabel* CurrentLabel = findChild<QLabel*>(tmp);
                QString tmp2 = QString::number(value);
                CurrentLabel->setText(tmp2);

                QString tmp_l("LineEditVelHomeSetup_");
                QString tmp3_l = QString::number(num_drv_value);
                tmp_l.append(tmp3_l);
                QLineEdit* CurrentLabel_l = findChild<QLineEdit*>(tmp_l);
                QString tmp2_l = QString::number(value);
                CurrentLabel_l->setText(tmp2_l);

                if (value != -1)
                {
                    CurrentLabelStatus->setText("Connected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : green; }");
                }
                else
                {
                    CurrentLabelStatus->setText("Disconnected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : red; }");
                }
            }

            //Case Acceleration
            else if (switch_char == 'A' && switch_char2 == 'c')
            {
                QString tmp("LabelAccelerationSetup_");
                QString tmp3 = QString::number(num_drv_value);
                tmp.append(tmp3);
                QLabel* CurrentLabel = findChild<QLabel*>(tmp);
                QString tmp2 = QString::number(value);
                CurrentLabel->setText(tmp2);

                QString tmp_l("LineEditAccelerationSetup_");
                QString tmp3_l = QString::number(num_drv_value);
                tmp_l.append(tmp3_l);
                QLineEdit* CurrentLabel_l = findChild<QLineEdit*>(tmp_l);
                QString tmp2_l = QString::number(value);
                CurrentLabel_l->setText(tmp2_l);


                if (value != -1)
                {
                    CurrentLabelStatus->setText("Connected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : green; }");
                }
                else
                {
                    CurrentLabelStatus->setText("Disconnected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : red; }");
                }

            }

            //Case AnalogOutput0
            else if (switch_char == 'A' && switch_char2 == 'n')
            {
                QString tmp("LabelAnalogOutputSetup_");
                QString tmp3 = QString::number(num_drv_value);
                tmp.append(tmp3);
                QLabel* CurrentLabel = findChild<QLabel*>(tmp);
                QString tmp2 = QString::number(value);
                CurrentLabel->setText(tmp2);


                QString tmp_l("LineEditAnalogOutputSetup_");
                QString tmp3_l = QString::number(num_drv_value);
                tmp_l.append(tmp3_l);
                QLineEdit* CurrentLabel_l = findChild<QLineEdit*>(tmp_l);
                QString tmp2_l = QString::number(value);
                CurrentLabel_l->setText(tmp2_l);

                QString tmp_all("LabelAllAO_");
                QString tmp3_all = QString::number(num_drv_value);
                tmp_all.append(tmp3_all);
                QLabel* CurrentLabel_all = findChild<QLabel*>(tmp_all);
                QString tmp2_all = QString::number(value);
                CurrentLabel_all->setText(tmp2_all);


                if (value != -1)
                {
                    CurrentLabelStatus->setText("Connected");
                    CurrentLabelStatus_all->setText("Connected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : green; }");
                    CurrentLabelStatus_all->setStyleSheet("QLabel { color : green; }");
                }
                else
                {
                    CurrentLabelStatus->setText("Disconnected");
                    CurrentLabelStatus_all->setText("Disconnected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : red; }");
                    CurrentLabelStatus_all->setStyleSheet("QLabel { color : red; }");
                }

            }

            //Case deceleration
            else if (switch_char == 'D')
            {
                QString tmp("LabelDecelerationSetup_");
                QString tmp3 = QString::number(num_drv_value);
                tmp.append(tmp3);
                QLabel* CurrentLabel = findChild<QLabel*>(tmp);
                QString tmp2 = QString::number(value);
                CurrentLabel->setText(tmp2);


                QString tmp_l("LineEditDecelerationSetup_");
                QString tmp3_l = QString::number(num_drv_value);
                tmp_l.append(tmp3_l);
                QLineEdit* CurrentLabel_l = findChild<QLineEdit*>(tmp_l);
                QString tmp2_l = QString::number(value);
                CurrentLabel_l->setText(tmp2_l);



                if (value != -1)
                {
                    CurrentLabelStatus->setText("Connected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : green; }");
                }
                else
                {
                    CurrentLabelStatus->setText("Disconnected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : red; }");
                }

            }

            //Case PhaseCurrent
            else if (switch_char == 'P')
            {
                QString tmp("LabelPhaseCurrentSetup_");
                QString tmp3 = QString::number(num_drv_value);
                tmp.append(tmp3);
                QLabel* CurrentLabel = findChild<QLabel*>(tmp);
                QString tmp2 = QString::number(value);
                CurrentLabel->setText(tmp2);


                QString tmp_l("LineEditPhaseCurrentSetup_");
                QString tmp3_l = QString::number(num_drv_value);
                tmp_l.append(tmp3_l);
                QLineEdit* CurrentLabel_l = findChild<QLineEdit*>(tmp_l);
                QString tmp2_l = QString::number(value);
                CurrentLabel_l->setText(tmp2_l);



                if (value != -1)
                {
                    CurrentLabelStatus->setText("Connected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : green; }");
                }
                else
                {
                    CurrentLabelStatus->setText("Disconnected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : red; }");
                }

            }
            else
            {
                return;
            }

            AppendMessageToQtextEdit(ui->GeneralConnectionLog, data_string);

        }

        //This message contains useful information about the position of the driver: CurrentPosition and AnalogInput0
        re1.setPattern("^get_mov_par[ ][0-9]{1,2}[ ]((CurrentPosition)|(AnalogInput0))[ ]-{0,1}[0-9]{1,20}$");
        match = re1.match(data_string);

        if (re1.match(data_string).hasMatch()) {

            char* mypunt;
            mypunt = FindPointer(data_string.toLatin1().data());
            int num_drv_value = atoi(mypunt);
            mypunt = FindPointer(mypunt);
            char switch_char = mypunt[0];
            char switch_char2 = mypunt[1];
            mypunt = FindPointer(mypunt);
            int value = atoi(mypunt);

            //Be careful: hard coded values!
            if (num_drv_value < 1 || num_drv_value > 14)
            {
                return;
            }

            QString tmpStatus("LabelStatusDriver");
            QString tmpStatus2 = QString::number(num_drv_value);
            tmpStatus.append(tmpStatus2);
            QLabel* CurrentLabelStatus = findChild<QLabel*>(tmpStatus);

            QString tmpStatus_all("LabelAllStatus_");
            QString tmpStatus2_all = QString::number(num_drv_value);
            tmpStatus_all.append(tmpStatus2_all);
            QLabel* CurrentLabelStatus_all = findChild<QLabel*>(tmpStatus_all);

            //Case AnalogInput0
            if (switch_char == 'A')
            {
                QString tmp("LabelMovAnalogInput0_");
                QString tmp3 = QString::number(num_drv_value);
                tmp.append(tmp3);
                QLabel* CurrentLabel = findChild<QLabel*>(tmp);
                QString tmp2 = QString::number(value);
                CurrentLabel->setText(tmp2);

                QString tmp_all("LabelAllAI0_");
                QString tmp3_all = QString::number(num_drv_value);
                tmp_all.append(tmp3_all);
                QLabel* CurrentLabel_all = findChild<QLabel*>(tmp_all);
                QString tmp2_all = QString::number(value);
                CurrentLabel_all->setText(tmp2_all);

                if (value != -1)
                {
                    CurrentLabelStatus->setText("Connected");
                    CurrentLabelStatus_all->setText("Connected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : green; }");
                    CurrentLabelStatus_all->setStyleSheet("QLabel { color : green; }");
                }
                else
                {
                    CurrentLabelStatus->setText("Disconnected");
                    CurrentLabelStatus_all->setText("Disconnected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : red; }");
                    CurrentLabelStatus_all->setStyleSheet("QLabel { color : red; }");
                }
            }

            //Case CurrentPosition
            else if (switch_char == 'C')
            {
                QString tmp("LabelMovCurrentPosition_");
                QString tmp3 = QString::number(num_drv_value);
                tmp.append(tmp3);
                QLabel* CurrentLabel = findChild<QLabel*>(tmp);
                QString tmp2 = QString::number(value);
                CurrentLabel->setText(tmp2);

                QString tmp_all("LabelAllCurrPos_");
                QString tmp3_all = QString::number(num_drv_value);
                tmp_all.append(tmp3_all);
                QLabel* CurrentLabel_all = findChild<QLabel*>(tmp_all);
                QString tmp2_all = QString::number(value);
                CurrentLabel_all->setText(tmp2_all);

                if (value != -1)
                {
                    CurrentLabelStatus->setText("Connected");
                    CurrentLabelStatus_all->setText("Connected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : green; }");
                    CurrentLabelStatus_all->setStyleSheet("QLabel { color : green; }");
                }
                else
                {
                    CurrentLabelStatus->setText("Disconnected");
                    CurrentLabelStatus_all->setText("Disconnected");
                    CurrentLabelStatus->setStyleSheet("QLabel { color : red; }");
                    CurrentLabelStatus_all->setStyleSheet("QLabel { color : red; }");
                }

            }
            else
            {
                return;
            }

            AppendMessageToQtextEdit(ui->GeneralConnectionLog, data_string);

        }

    }

    //In any case all the messages received from the server are printed in the ExpertMode QTextEdit.
    //Sulla expert mode va scritto proprio tutto!!!
    AppendMessageToQtextEdit(ui->ExpertModeMessage, data_string);

    CheckGeneralStatus();

}

//The following function is very simpler than the previous one: CheckExistence Server message are written only in the small window
//and in the expert window
void MainWindow::readTcpData_existence()
{

    //Reading the message
    QByteArray data = _pSocket_existence->readAll();

    QString data_string(data);

    //If TCPINLOGFILE == 1 the message is printed in the log file.
    if (TCPINLOGFILE == 1)
    {
        QString LogTmpString ("Received this data from CheckExistence server: " + data_string);

        main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LogTmpString.toUtf8().constData());
    }

    AppendMessageToQtextEdit(ui->ExpertModeMessage, data_string);

    AppendMessageToQtextEdit(ui->CheckExistenceLog, data_string);

    CheckGeneralStatus();

}

//Sending the message to CollSoft server from the expert mode.
//This means that the user types the command as if he were using a terminal.
//Pulsante per inviare il comando dalla expert mode
void MainWindow::on_CommandStringButton_clicked()
{
    QByteArray data(ui->CommandString->text().toUtf8()); // <-- fill with data
    _pSocket->write( data );

    QString LocalTmp("Command sent to server by terminal: " + ui->CommandString->text());
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmp.toUtf8().constData());

    AppendMessageToQtextEdit(ui->ExpertModeMessage, "Command sent...");
}

//Slot to connect the ExpertGUI to the server.
void MainWindow::on_ConnectToServerButton_clicked()
{

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Connect to CollSoft server command clicked.");

    _pSocket->connectToHost(ui->ServerIp->text().toUtf8(), (quint16)ui->ServerPort->text().toInt());
    if( _pSocket->waitForConnected(1000) ) {
        ;
    }

}

//Slot to send to the server a connect command
void MainWindow::on_ConnectProgrammer_clicked()
{

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Connect the server to the programmer button clicked.");

    QString tmp = "connect " + ui->ConnectProgrammerPath->text() + "\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );

    AppendMessageToQtextEdit(ui->ExpertModeMessage, "Connect Programmer request sent\n");

    AppendMessageToQtextEdit(ui->ProgrammerConnectionLog, "Connect Programmer request sent\n");

}

void MainWindow::on_CheckDrvAssoc_clicked()
{

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Clicked the button to check the association between driver serial number and logfile serial number.");

    QString tmp = "check_drv_assoc\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );

    AppendMessageToQtextEdit(ui->GeneralConnectionLog, "check_drv_assoc_sent\nwait a moment...\n");

}

void MainWindow::on_CheckExistence_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Clicked the button to check if the CollSoft process exists.");

    QString tmp = "check_process\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket_existence->write( data );
}

void MainWindow::on_ConnectExistence_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Clicked the button to connect the ExpertGUI with the CheckExistence server.");

    _pSocket_existence->connectToHost(ui->CheckExistenceLineIpEdit->text().toUtf8(), (quint16)ui->CheckExistenceLinePortEdit->text().toInt());
    if( _pSocket_existence->waitForConnected(1000) ) {
        //*//_pSocket_existence->write( data );
        ui->label_status1->setText("OK");
        ui->label_status1->setStyleSheet("QLabel { color : green; }");
    }
}

void MainWindow::on_KillProcess_CollSoft_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Clicked the button to kill the CollSoft server process.");

    ////////////////////////////
    //DB 20160624: be careful, these operation reproduce the on_DisconnectToServerButton_clicked() slot.
    //Executing this operation, the output of the program maybe inconsistent
    _pSocket->disconnectFromHost();
    ui->label_status2->setText("FAILED");
    ui->label_status2->setStyleSheet("QLabel { color : red; }");

    CheckGeneralStatus();

    AppendMessageToQtextEdit(ui->CollSoftConnectionLog, "Disconnected from CollSoft server\n");

    QString tmp = "kill_process\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket_existence->write( data );
}

void MainWindow::on_NewProcess_CollSoft_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Clicked the button to create a new CollSoft process.");

    QString tmp = "new_process\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket_existence->write( data );
}

void MainWindow::on_DisconnectExistence_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Clicked the button to disconnect the CheckExistence server from the ExpertGUI.");

    _pSocket_existence->disconnectFromHost();
    ui->label_status1->setText("FAILED");
    ui->label_status1->setStyleSheet("QLabel { color : red; }");

    CheckGeneralStatus();

    AppendMessageToQtextEdit(ui->CheckExistenceLog, "Disconnected from CheckExistence server\n");

}

void MainWindow::on_DisconnectToServerButton_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Clicked the button to disconnect the CollSoft server from the ExpertGUI.");

    _pSocket->disconnectFromHost();
    ui->label_status2->setText("FAILED");
    ui->label_status2->setStyleSheet("QLabel { color : red; }");

    CheckGeneralStatus();

    AppendMessageToQtextEdit(ui->CollSoftConnectionLog, "Disconnected from CollSoft server\n");
}

void MainWindow::closing()
{
    _pSocket->disconnectFromHost();
    _pSocket_existence->disconnectFromHost();
}

void MainWindow::on_ReadSerialDrvLog_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Reading the contents of the SerialDrvLog.txt file");

    QString tmp = "read_serial_log\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_ReadFileParLog_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Reading the contents of the FileParLog.txt file");

    QString tmp = "read_par_log\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_CheckParAssoc_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Clicked the button to check the association between the current parameter of the drivers and the content of FileParLog.txt");

    QString tmp = "check_par_assoc\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );

    AppendMessageToQtextEdit(ui->GeneralConnectionLog, "check_par_assoc_sent\nwait a moment...\n");
}


//Getting parameter signal
void MainWindow::on_GetParameterDriver_1_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 1...");

    QString tmp = "get_par 1\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_2_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 2...");

    QString tmp = "get_par 2\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_3_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 3...");

    QString tmp = "get_par 3\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_4_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 4...");

    QString tmp = "get_par 4\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_5_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 5...");

    QString tmp = "get_par 5\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_6_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 6...");

    QString tmp = "get_par 6\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_7_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 7...");

    QString tmp = "get_par 7\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_8_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 8...");

    QString tmp = "get_par 8\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_9_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 9...");

    QString tmp = "get_par 9\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_10_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 10...");

    QString tmp = "get_par 10\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_11_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 11...");

    QString tmp = "get_par 11\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_12_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 12...");

    QString tmp = "get_par 12\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_13_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 13...");

    QString tmp = "get_par 13\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterDriver_14_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting the parameters of the driver 14...");

    QString tmp = "get_par 14\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_SetParameterDriver_1_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 1...");

    if(ui->LineEditMaxVelSetup_1->text() != "" && ui->LineEditVelHomeSetup_1->text() != "" && ui->LineEditAccelerationSetup_1->text() != "" &&
            ui->LineEditDecelerationSetup_1->text() != "" && ui->LineEditPhaseCurrentSetup_1->text() != "" && ui->LineEditAnalogOutputSetup_1->text() != "")
    {
        QString tmp = "set_par 1 " + ui->LineEditMaxVelSetup_1->text() + " " + ui->LineEditVelHomeSetup_1->text() + " " + ui->LineEditAccelerationSetup_1->text() +
        " " + ui->LineEditDecelerationSetup_1->text() + " " + ui->LineEditPhaseCurrentSetup_1->text() + " " + ui->LineEditAnalogOutputSetup_1->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }

}

void MainWindow::on_SetParameterDriver_2_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 2...");

    if(ui->LineEditMaxVelSetup_2->text() != "" && ui->LineEditVelHomeSetup_2->text() != "" && ui->LineEditAccelerationSetup_2->text() != "" &&
            ui->LineEditDecelerationSetup_2->text() != "" && ui->LineEditPhaseCurrentSetup_2->text() != "" && ui->LineEditAnalogOutputSetup_2->text() != "")
    {
        QString tmp = "set_par 2 " + ui->LineEditMaxVelSetup_2->text() + " " + ui->LineEditVelHomeSetup_2->text() + " " + ui->LineEditAccelerationSetup_2->text() +
        " " + ui->LineEditDecelerationSetup_2->text() + " " + ui->LineEditPhaseCurrentSetup_2->text() + " " + ui->LineEditAnalogOutputSetup_2->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_3_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 3...");

    if(ui->LineEditMaxVelSetup_3->text() != "" && ui->LineEditVelHomeSetup_3->text() != "" && ui->LineEditAccelerationSetup_3->text() != "" &&
            ui->LineEditDecelerationSetup_3->text() != "" && ui->LineEditPhaseCurrentSetup_3->text() != "" && ui->LineEditAnalogOutputSetup_3->text() != "")
    {
        QString tmp = "set_par 3 " + ui->LineEditMaxVelSetup_3->text() + " " + ui->LineEditVelHomeSetup_3->text() + " " + ui->LineEditAccelerationSetup_3->text() +
        " " + ui->LineEditDecelerationSetup_3->text() + " " + ui->LineEditPhaseCurrentSetup_3->text() + " " + ui->LineEditAnalogOutputSetup_3->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }

}

void MainWindow::on_SetParameterDriver_4_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 4...");

    if(ui->LineEditMaxVelSetup_4->text() != "" && ui->LineEditVelHomeSetup_4->text() != "" && ui->LineEditAccelerationSetup_4->text() != "" &&
            ui->LineEditDecelerationSetup_4->text() != "" && ui->LineEditPhaseCurrentSetup_4->text() != "" && ui->LineEditAnalogOutputSetup_4->text() != "")
    {
        QString tmp = "set_par 4 " + ui->LineEditMaxVelSetup_4->text() + " " + ui->LineEditVelHomeSetup_4->text() + " " + ui->LineEditAccelerationSetup_4->text() +
        " " + ui->LineEditDecelerationSetup_4->text() + " " + ui->LineEditPhaseCurrentSetup_4->text() + " " + ui->LineEditAnalogOutputSetup_4->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_5_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 5...");

    if(ui->LineEditMaxVelSetup_5->text() != "" && ui->LineEditVelHomeSetup_5->text() != "" && ui->LineEditAccelerationSetup_5->text() != "" &&
            ui->LineEditDecelerationSetup_5->text() != "" && ui->LineEditPhaseCurrentSetup_5->text() != "" && ui->LineEditAnalogOutputSetup_5->text() != "")
    {
        QString tmp = "set_par 5 " + ui->LineEditMaxVelSetup_5->text() + " " + ui->LineEditVelHomeSetup_5->text() + " " + ui->LineEditAccelerationSetup_5->text() +
        " " + ui->LineEditDecelerationSetup_5->text() + " " + ui->LineEditPhaseCurrentSetup_5->text() + " " + ui->LineEditAnalogOutputSetup_5->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_6_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 6...");

    if(ui->LineEditMaxVelSetup_6->text() != "" && ui->LineEditVelHomeSetup_6->text() != "" && ui->LineEditAccelerationSetup_6->text() != "" &&
            ui->LineEditDecelerationSetup_6->text() != "" && ui->LineEditPhaseCurrentSetup_6->text() != "" && ui->LineEditAnalogOutputSetup_6->text() != "")
    {
        QString tmp = "set_par 6 " + ui->LineEditMaxVelSetup_6->text() + " " + ui->LineEditVelHomeSetup_6->text() + " " + ui->LineEditAccelerationSetup_6->text() +
        " " + ui->LineEditDecelerationSetup_6->text() + " " + ui->LineEditPhaseCurrentSetup_6->text() + " " + ui->LineEditAnalogOutputSetup_6->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_7_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 7...");

    if(ui->LineEditMaxVelSetup_7->text() != "" && ui->LineEditVelHomeSetup_7->text() != "" && ui->LineEditAccelerationSetup_7->text() != "" &&
            ui->LineEditDecelerationSetup_7->text() != "" && ui->LineEditPhaseCurrentSetup_7->text() != "" && ui->LineEditAnalogOutputSetup_7->text() != "")
    {
        QString tmp = "set_par 7 " + ui->LineEditMaxVelSetup_7->text() + " " + ui->LineEditVelHomeSetup_7->text() + " " + ui->LineEditAccelerationSetup_7->text() +
        " " + ui->LineEditDecelerationSetup_7->text() + " " + ui->LineEditPhaseCurrentSetup_7->text() + " " + ui->LineEditAnalogOutputSetup_7->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_8_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 8...");

    if(ui->LineEditMaxVelSetup_8->text() != "" && ui->LineEditVelHomeSetup_8->text() != "" && ui->LineEditAccelerationSetup_8->text() != "" &&
            ui->LineEditDecelerationSetup_8->text() != "" && ui->LineEditPhaseCurrentSetup_8->text() != "" && ui->LineEditAnalogOutputSetup_8->text() != "")
    {
        QString tmp = "set_par 8 " + ui->LineEditMaxVelSetup_8->text() + " " + ui->LineEditVelHomeSetup_8->text() + " " + ui->LineEditAccelerationSetup_8->text() +
        " " + ui->LineEditDecelerationSetup_8->text() + " " + ui->LineEditPhaseCurrentSetup_8->text() + " " + ui->LineEditAnalogOutputSetup_8->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_9_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 9...");

    if(ui->LineEditMaxVelSetup_9->text() != "" && ui->LineEditVelHomeSetup_9->text() != "" && ui->LineEditAccelerationSetup_9->text() != "" &&
            ui->LineEditDecelerationSetup_9->text() != "" && ui->LineEditPhaseCurrentSetup_9->text() != "" && ui->LineEditAnalogOutputSetup_9->text() != "")
    {
        QString tmp = "set_par 9 " + ui->LineEditMaxVelSetup_9->text() + " " + ui->LineEditVelHomeSetup_9->text() + " " + ui->LineEditAccelerationSetup_9->text() +
        " " + ui->LineEditDecelerationSetup_9->text() + " " + ui->LineEditPhaseCurrentSetup_9->text() + " " + ui->LineEditAnalogOutputSetup_9->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_10_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 10...");

    if(ui->LineEditMaxVelSetup_10->text() != "" && ui->LineEditVelHomeSetup_10->text() != "" && ui->LineEditAccelerationSetup_10->text() != "" &&
            ui->LineEditDecelerationSetup_10->text() != "" && ui->LineEditPhaseCurrentSetup_10->text() != "" && ui->LineEditAnalogOutputSetup_10->text() != "")
    {
        QString tmp = "set_par 10 " + ui->LineEditMaxVelSetup_10->text() + " " + ui->LineEditVelHomeSetup_10->text() + " " + ui->LineEditAccelerationSetup_10->text() +
        " " + ui->LineEditDecelerationSetup_10->text() + " " + ui->LineEditPhaseCurrentSetup_10->text() + " " + ui->LineEditAnalogOutputSetup_10->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_11_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 11...");

    if(ui->LineEditMaxVelSetup_11->text() != "" && ui->LineEditVelHomeSetup_11->text() != "" && ui->LineEditAccelerationSetup_11->text() != "" &&
            ui->LineEditDecelerationSetup_11->text() != "" && ui->LineEditPhaseCurrentSetup_11->text() != "" && ui->LineEditAnalogOutputSetup_11->text() != "")
    {
        QString tmp = "set_par 11 " + ui->LineEditMaxVelSetup_11->text() + " " + ui->LineEditVelHomeSetup_11->text() + " " + ui->LineEditAccelerationSetup_11->text() +
        " " + ui->LineEditDecelerationSetup_11->text() + " " + ui->LineEditPhaseCurrentSetup_11->text() + " " + ui->LineEditAnalogOutputSetup_11->text() + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_12_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 12...");

    if(ui->LineEditMaxVelSetup_12->text() != "" && ui->LineEditVelHomeSetup_12->text() != "" && ui->LineEditAccelerationSetup_12->text() != "" &&
            ui->LineEditDecelerationSetup_12->text() != "" && ui->LineEditPhaseCurrentSetup_12->text() != "" && ui->LineEditAnalogOutputSetup_12->text() != "")
    {
        QString tmp = "set_par 12 " + ui->LineEditMaxVelSetup_12->text() + " " + ui->LineEditVelHomeSetup_12->text() + " " + ui->LineEditAccelerationSetup_12->text() +
        " " + ui->LineEditDecelerationSetup_12->text() + " " + ui->LineEditPhaseCurrentSetup_12->text() + " " + ui->LineEditAnalogOutputSetup_12->text()  + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_13_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 13...");

    if(ui->LineEditMaxVelSetup_13->text() != "" && ui->LineEditVelHomeSetup_13->text() != "" && ui->LineEditAccelerationSetup_13->text() != "" &&
            ui->LineEditDecelerationSetup_13->text() != "" && ui->LineEditPhaseCurrentSetup_13->text() != "" && ui->LineEditAnalogOutputSetup_13->text() != "")
    {
        QString tmp = "set_par 13 " + ui->LineEditMaxVelSetup_13->text() + " " + ui->LineEditVelHomeSetup_13->text() + " " + ui->LineEditAccelerationSetup_13->text() +
        " " + ui->LineEditDecelerationSetup_13->text() + " " + ui->LineEditPhaseCurrentSetup_13->text() + " " + ui->LineEditAnalogOutputSetup_13->text()  + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_SetParameterDriver_14_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of the driver 14...");

    if(ui->LineEditMaxVelSetup_14->text() != "" && ui->LineEditVelHomeSetup_14->text() != "" && ui->LineEditAccelerationSetup_14->text() != "" &&
            ui->LineEditDecelerationSetup_14->text() != "" && ui->LineEditPhaseCurrentSetup_14->text() != "" && ui->LineEditAnalogOutputSetup_14->text() != "")
    {
        QString tmp = "set_par 14 " + ui->LineEditMaxVelSetup_14->text() + " " + ui->LineEditVelHomeSetup_14->text() + " " + ui->LineEditAccelerationSetup_14->text() +
        " " + ui->LineEditDecelerationSetup_14->text() + " " + ui->LineEditPhaseCurrentSetup_14->text() + " " + ui->LineEditAnalogOutputSetup_14->text()  + "\n";

        QByteArray data(tmp.toUtf8()); // <-- fill with data
        _pSocket->write( data );
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }
}

void MainWindow::on_Homing_1_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 1...");

    QString tmp = "homing 1\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_2_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 2...");

    QString tmp = "homing 2\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_3_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 3...");

    QString tmp = "homing 3\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_4_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 4...");

    QString tmp = "homing 4\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_5_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 5...");

    QString tmp = "homing 5\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_6_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 6...");

    QString tmp = "homing 6\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_7_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 7...");

    QString tmp = "homing 7\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_8_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 8...");

    QString tmp = "homing 8\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_9_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 9...");

    QString tmp = "homing 9\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_10_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 10...");

    QString tmp = "homing 10\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_11_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 11...");

    QString tmp = "homing 11\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_12_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 12...");

    QString tmp = "homing 12\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_13_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 13...");

    QString tmp = "homing 13\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_Homing_14_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing driver 14...");

    QString tmp = "homing 14\n";

    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_1_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 1...");

    QString tmp = "get_mov_par 1\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_2_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 2...");

    QString tmp = "get_mov_par 2\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_3_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 3...");

    QString tmp = "get_mov_par 3\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_4_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 4...");

    QString tmp = "get_mov_par 4\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_5_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 5...");

    QString tmp = "get_mov_par 5\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_6_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 6...");

    QString tmp = "get_mov_par 6\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_7_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 7...");

    QString tmp = "get_mov_par 7\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_8_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 8...");

    QString tmp = "get_mov_par 8\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_9_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 9...");

    QString tmp = "get_mov_par 9\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_10_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 10...");

    QString tmp = "get_mov_par 10\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_11_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 11...");

    QString tmp = "get_mov_par 11\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_12_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 12...");

    QString tmp = "get_mov_par 12\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_13_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 13...");

    QString tmp = "get_mov_par 13\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_GetParameterMov_14_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting movimentation parameters of driver 14...");

    QString tmp = "get_mov_par 14\n";
    QByteArray data(tmp.toUtf8()); // <-- fill with data
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_1_clicked()
{
    QString LocalTmpLog("Moving driver 1 to " + ui->LineEditMoveToValue_1->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 1 " + ui->LineEditMoveToValue_1->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_2_clicked()
{
    QString LocalTmpLog("Moving driver 2 to " + ui->LineEditMoveToValue_2->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 2 " + ui->LineEditMoveToValue_2->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_3_clicked()
{
    QString LocalTmpLog("Moving driver 3 to " + ui->LineEditMoveToValue_3->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 3 " + ui->LineEditMoveToValue_3->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_4_clicked()
{
    QString LocalTmpLog("Moving driver 4 to " + ui->LineEditMoveToValue_4->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 4 " + ui->LineEditMoveToValue_4->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_5_clicked()
{
    QString LocalTmpLog("Moving driver 5 to " + ui->LineEditMoveToValue_5->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 5 " + ui->LineEditMoveToValue_5->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_6_clicked()
{
    QString LocalTmpLog("Moving driver 6 to " + ui->LineEditMoveToValue_6->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 6 " + ui->LineEditMoveToValue_6->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_7_clicked()
{
    QString LocalTmpLog("Moving driver 7 to " + ui->LineEditMoveToValue_7->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 7 " + ui->LineEditMoveToValue_7->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_8_clicked()
{
    QString LocalTmpLog("Moving driver 8 to " + ui->LineEditMoveToValue_8->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 8 " + ui->LineEditMoveToValue_8->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_9_clicked()
{
    QString LocalTmpLog("Moving driver 9 to " + ui->LineEditMoveToValue_9->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 9 " + ui->LineEditMoveToValue_9->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_10_clicked()
{
    QString LocalTmpLog("Moving driver 10 to " + ui->LineEditMoveToValue_10->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 10 " + ui->LineEditMoveToValue_10->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_11_clicked()
{
    QString LocalTmpLog("Moving driver 11 to " + ui->LineEditMoveToValue_11->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 11 " + ui->LineEditMoveToValue_11->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_12_clicked()
{
    QString LocalTmpLog("Moving driver 12 to " + ui->LineEditMoveToValue_12->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 12 " + ui->LineEditMoveToValue_12->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_13_clicked()
{
    QString LocalTmpLog("Moving driver 13 to " + ui->LineEditMoveToValue_13->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 13 " + ui->LineEditMoveToValue_13->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_MoveTo_14_clicked()
{
    QString LocalTmpLog("Moving driver 14 to " + ui->LineEditMoveToValue_14->text());

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

    QString tmp = "move_to 14 " + ui->LineEditMoveToValue_14->text() + "\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_GetAllParameter_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Getting all parameters");

    QString tmp = "get_all_parameter\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

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

void MainWindow::on_Homing_all_clicked()
{

    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Homing all drivers...");

    QString tmp = "homing_mult ";

    if (ui->checkBox_1->checkState() ==  Qt::Checked)  tmp+="1 ";
    if (ui->checkBox_2->checkState() ==  Qt::Checked)  tmp+="2 " ;
    if (ui->checkBox_3->checkState() ==  Qt::Checked)  tmp+="3 " ;
    if (ui->checkBox_4->checkState() ==  Qt::Checked)  tmp+="4 " ;
    if (ui->checkBox_5->checkState() ==  Qt::Checked)  tmp+="5 " ;
    if (ui->checkBox_6->checkState() ==  Qt::Checked)  tmp+="6 " ;
    if (ui->checkBox_7->checkState() ==  Qt::Checked)  tmp+="7 " ;
    if (ui->checkBox_8->checkState() ==  Qt::Checked)  tmp+="8 " ;
    if (ui->checkBox_9->checkState() ==  Qt::Checked)  tmp+="9 " ;
    if (ui->checkBox_10->checkState() ==  Qt::Checked)  tmp+="10 " ;
    if (ui->checkBox_11->checkState() ==  Qt::Checked)  tmp+="11 " ;
    if (ui->checkBox_12->checkState() ==  Qt::Checked)  tmp+="12 " ;
    if (ui->checkBox_13->checkState() ==  Qt::Checked)  tmp+="13 " ;
    if (ui->checkBox_14->checkState() ==  Qt::Checked)  tmp+="14 " ;

    tmp+="\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );

}

void MainWindow::on_MoveTo_all_clicked()
{
    if (ui->LineEditMoveToValue_all->text() != "")
    {

        QString choose_parameter = "Target position: " + ui->LineEditMoveToValue_all->text() + "\n\n";

        choose_parameter += "Are you sure to move to this positon drivers: ";

        if (ui->checkBox_1->checkState() ==  Qt::Checked)  choose_parameter+="1 ";
        if (ui->checkBox_2->checkState() ==  Qt::Checked)  choose_parameter+="2 " ;
        if (ui->checkBox_3->checkState() ==  Qt::Checked)  choose_parameter+="3 " ;
        if (ui->checkBox_4->checkState() ==  Qt::Checked)  choose_parameter+="4 " ;
        if (ui->checkBox_5->checkState() ==  Qt::Checked)  choose_parameter+="5 " ;
        if (ui->checkBox_6->checkState() ==  Qt::Checked)  choose_parameter+="6 " ;
        if (ui->checkBox_7->checkState() ==  Qt::Checked)  choose_parameter+="7 " ;
        if (ui->checkBox_8->checkState() ==  Qt::Checked)  choose_parameter+="8 " ;
        if (ui->checkBox_9->checkState() ==  Qt::Checked)  choose_parameter+="9 " ;
        if (ui->checkBox_10->checkState() ==  Qt::Checked)  choose_parameter+="10 " ;
        if (ui->checkBox_11->checkState() ==  Qt::Checked)  choose_parameter+="11 " ;
        if (ui->checkBox_12->checkState() ==  Qt::Checked)  choose_parameter+="12 " ;
        if (ui->checkBox_13->checkState() ==  Qt::Checked)  choose_parameter+="13 " ;
        if (ui->checkBox_14->checkState() ==  Qt::Checked)  choose_parameter+="14 " ;

        choose_parameter += "?\n\n";

        AllDriverSettingsCheck* all_check = new AllDriverSettingsCheck(choose_parameter);

        all_check->setWindowModality(Qt::ApplicationModal);

        int accepted = 0;

        accepted = all_check->exec();

        if (accepted == 1)
        {
            QString LocalTmpLog("Moving all drivers to " + ui->LineEditMoveToValue_all->text());

            main_window_log_file->LogFileExpertGUI_WriteOnLogFile(LocalTmpLog.toUtf8().constData());

            QString tmp = "moveto_mult " + ui->LineEditMoveToValue_all->text() + " "; //Occhio allo sharp!!!!

            if (ui->checkBox_1->checkState() ==  Qt::Checked)  tmp+="1 ";
            if (ui->checkBox_2->checkState() ==  Qt::Checked)  tmp+="2 " ;
            if (ui->checkBox_3->checkState() ==  Qt::Checked)  tmp+="3 " ;
            if (ui->checkBox_4->checkState() ==  Qt::Checked)  tmp+="4 " ;
            if (ui->checkBox_5->checkState() ==  Qt::Checked)  tmp+="5 " ;
            if (ui->checkBox_6->checkState() ==  Qt::Checked)  tmp+="6 " ;
            if (ui->checkBox_7->checkState() ==  Qt::Checked)  tmp+="7 " ;
            if (ui->checkBox_8->checkState() ==  Qt::Checked)  tmp+="8 " ;
            if (ui->checkBox_9->checkState() ==  Qt::Checked)  tmp+="9 " ;
            if (ui->checkBox_10->checkState() ==  Qt::Checked)  tmp+="10 " ;
            if (ui->checkBox_11->checkState() ==  Qt::Checked)  tmp+="11 " ;
            if (ui->checkBox_12->checkState() ==  Qt::Checked)  tmp+="12 " ;
            if (ui->checkBox_13->checkState() ==  Qt::Checked)  tmp+="13 " ;
            if (ui->checkBox_14->checkState() ==  Qt::Checked)  tmp+="14 " ;

            tmp += "\n";

            QByteArray data(tmp.toUtf8());
            _pSocket->write( data );
        }
    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to specified a MoveTo value in the field above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }

}

void MainWindow::on_SetParameterDriver_all_clicked()
{
    if (ui->LineEditMaxVelSetup_all->text() != "" && ui->LineEditVelHomeSetup_all->text() != ""
            && ui->LineEditAccelerationSetup_all->text() != "" &&  ui->LineEditDecelerationSetup_all->text() != ""
            && ui->LineEditPhaseCurrentSetup_all->text() != "" && ui->LineEditAnalogOutputSetup_all->text() != "")
    {

        QString choose_parameter = "MaxVel: " + ui->LineEditMaxVelSetup_all->text() + "\n";
        choose_parameter += "VelHome: " + ui->LineEditVelHomeSetup_all->text() + "\n";
        choose_parameter += "Acceleration: " + ui->LineEditAccelerationSetup_all->text() + "\n";
        choose_parameter += "Deceleration: " + ui->LineEditDecelerationSetup_all->text() + "\n";
        choose_parameter += "PhaseCurrent: " + ui->LineEditPhaseCurrentSetup_all->text() + "\n";
        choose_parameter += "AnalogOutput(0): " + ui->LineEditAnalogOutputSetup_all->text() + "\n\n";

        choose_parameter += "Are you sure to set this parameter to driver: ";

        if (ui->checkBox_1->checkState() ==  Qt::Checked)  choose_parameter+="1 ";
        if (ui->checkBox_2->checkState() ==  Qt::Checked)  choose_parameter+="2 " ;
        if (ui->checkBox_3->checkState() ==  Qt::Checked)  choose_parameter+="3 " ;
        if (ui->checkBox_4->checkState() ==  Qt::Checked)  choose_parameter+="4 " ;
        if (ui->checkBox_5->checkState() ==  Qt::Checked)  choose_parameter+="5 " ;
        if (ui->checkBox_6->checkState() ==  Qt::Checked)  choose_parameter+="6 " ;
        if (ui->checkBox_7->checkState() ==  Qt::Checked)  choose_parameter+="7 " ;
        if (ui->checkBox_8->checkState() ==  Qt::Checked)  choose_parameter+="8 " ;
        if (ui->checkBox_9->checkState() ==  Qt::Checked)  choose_parameter+="9 " ;
        if (ui->checkBox_10->checkState() ==  Qt::Checked)  choose_parameter+="10 " ;
        if (ui->checkBox_11->checkState() ==  Qt::Checked)  choose_parameter+="11 " ;
        if (ui->checkBox_12->checkState() ==  Qt::Checked)  choose_parameter+="12 " ;
        if (ui->checkBox_13->checkState() ==  Qt::Checked)  choose_parameter+="13 " ;
        if (ui->checkBox_14->checkState() ==  Qt::Checked)  choose_parameter+="14 " ;

        choose_parameter += "?\n\n";


        AllDriverSettingsCheck* all_check = new AllDriverSettingsCheck(choose_parameter);

        all_check->setWindowModality(Qt::ApplicationModal);

        int accepted = 0;

        accepted = all_check->exec();

        if (accepted == 1)
        {

            main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Setting the parameters of all drivers...");

            QString tmp = "setmult_par " + ui->LineEditMaxVelSetup_all->text() + " " + ui->LineEditVelHomeSetup_all->text()  + " " + ui->LineEditAccelerationSetup_all->text() +
            " " + ui->LineEditDecelerationSetup_all->text() + " " + ui->LineEditPhaseCurrentSetup_all->text() + " " + ui->LineEditAnalogOutputSetup_all->text() + " "; //Occhio allo sharp!!!!

            if (ui->checkBox_1->checkState() ==  Qt::Checked)  tmp+="1 ";
            if (ui->checkBox_2->checkState() ==  Qt::Checked)  tmp+="2 " ;
            if (ui->checkBox_3->checkState() ==  Qt::Checked)  tmp+="3 " ;
            if (ui->checkBox_4->checkState() ==  Qt::Checked)  tmp+="4 " ;
            if (ui->checkBox_5->checkState() ==  Qt::Checked)  tmp+="5 " ;
            if (ui->checkBox_6->checkState() ==  Qt::Checked)  tmp+="6 " ;
            if (ui->checkBox_7->checkState() ==  Qt::Checked)  tmp+="7 " ;
            if (ui->checkBox_8->checkState() ==  Qt::Checked)  tmp+="8 " ;
            if (ui->checkBox_9->checkState() ==  Qt::Checked)  tmp+="9 " ;
            if (ui->checkBox_10->checkState() ==  Qt::Checked)  tmp+="10 " ;
            if (ui->checkBox_11->checkState() ==  Qt::Checked)  tmp+="11 " ;
            if (ui->checkBox_12->checkState() ==  Qt::Checked)  tmp+="12 " ;
            if (ui->checkBox_13->checkState() ==  Qt::Checked)  tmp+="13 " ;
            if (ui->checkBox_14->checkState() ==  Qt::Checked)  tmp+="14 " ;

            tmp += "\n";

            QByteArray data(tmp.toUtf8());
            _pSocket->write( data );

        }

    }
    else
    {
        QString choose_parameter = "Warning! In order to accomplished this operation you have to set all the parameters in the fields above!";
        About* tmp = new About();
        tmp->SetAboutLabel(choose_parameter);
        tmp->exec();
    }

}

void MainWindow::on_CheckEncoderAssoc_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking the new encoder values and comparing them with those in the EncoderLog.txt file...");

    QString tmp = "check_encode_assoc\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );

    AppendMessageToQtextEdit(ui->GeneralConnectionLog, "Checking driver/encoder values association\nwait a moment...\n");
}

void MainWindow::on_ReadEncoderLog_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Reading the EncoderLog.txt file...");

    QString tmp = "read_encoder_log\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );

    AppendMessageToQtextEdit(ui->GeneralConnectionLog, "Reading EncoderLog.txt\nwait a moment...\n");
}

void MainWindow::on_ReadGeneralLog_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Reading the GeneralLog.txt file...");

    QString tmp = "read_log\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );

    AppendMessageToQtextEdit(ui->GeneralConnectionLog, "Reading GeneralLog.txt\nwait a moment...\n");

}

void MainWindow::on_CheckInternalStatus_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking the internal status of the server...");

    QString tmp = "check_internal_status\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );

    AppendMessageToQtextEdit(ui->GeneralConnectionLog, "Checking internal server status\nwait a moment...\n");

}

void MainWindow::on_LoadEncoderFromFileButton_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Loading the encoder values from EncoderLog.txt...");

    QString tmp = "load_encoder_from_file\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );

    AppendMessageToQtextEdit(ui->GeneralConnectionLog, "Loading encoder from file command sent....\n");
}

void MainWindow::on_ReadActualEncoderValue_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Reading the encoder values actually used by the server...");

    QString tmp = "Read_actual_encoder_values\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );

    AppendMessageToQtextEdit(ui->GeneralConnectionLog, "Reading actual encoder values command sent....\n");
}

void MainWindow::on_CheckPositionButton_1_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 1...");

    QString tmp = "check_position 1\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_2_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 2...");

    QString tmp = "check_position 2\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_3_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 3...");

    QString tmp = "check_position 3\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_4_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 4...");

    QString tmp = "check_position 4\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_5_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 5...");

    QString tmp = "check_position 5\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_6_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 6...");

    QString tmp = "check_position 6\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_7_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 7...");

    QString tmp = "check_position 7\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_8_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 8...");

    QString tmp = "check_position 8\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_9_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 9...");

    QString tmp = "check_position 9\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_10_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 10...");

    QString tmp = "check_position 10\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_11_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 11...");

    QString tmp = "check_position 11\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_12_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 12...");

    QString tmp = "check_position 12\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_13_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 13...");

    QString tmp = "check_position 13\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_CheckPositionButton_14_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking position of driver 14...");

    QString tmp = "check_position 14\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_DeviceListButton_clicked()
{
    main_window_log_file->LogFileExpertGUI_WriteOnLogFile("Checking device list...");

    QString tmp = "device_list\n";

    QByteArray data(tmp.toUtf8());
    _pSocket->write( data );
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    About* about = new About();
    about->exec();
}

void MainWindow::CheckGeneralStatus()
{
    if (ui->label_status1->text() == "OK" && ui->label_status2->text() == "OK" && ui->label_status3->text() == "OK")
    {
        ui->label_statusAll->setText("OK");
        ui->label_statusAll->setStyleSheet("QLabel { color : green; }");
    }
    else
    {
        ui->label_statusAll->setText("FAILED");
        ui->label_statusAll->setStyleSheet("QLabel { color : red; }");
    }
}

void MainWindow::AppendMessageToQtextEdit(QTextEdit* MexArea, QString Message)
{
    //Message += "\n---------------------";

    //QString PromptMessage("> ");

    MexArea->append("> " + Message);
    QTextCursor c2 =  MexArea->textCursor();
    c2.movePosition(QTextCursor::End);
    MexArea->setTextCursor(c2);
    MexArea->ensureCursorVisible();
}
