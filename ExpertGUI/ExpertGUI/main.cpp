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
#include <QApplication>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    //Creating the application
    QApplication a(argc, argv);

    //It means that the ExpertGUI is lauched with the username
    //./ExpertGUI Robert
    //The UserGUI launchs the ExpertGUI with this modality
    if (argc == 2)
    {
        //Passing argv[1] to the MainWindow.
        MainWindow w(argv[1]);

        //w.setWindowTitle("CollSoft Client GUI");
        w.show();

        QObject::connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(closing()));

        a.setStyleSheet("QToolTip { color: black; background-color: white; border: black; }");

        return a.exec();
    }
    else
    {
        MainWindow w;
        //w.setWindowTitle("CollSoft Client GUI");
        w.show();

        QObject::connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(closing()));

        a.setStyleSheet("QToolTip { color: black; background-color: white; border: black; }");

        return a.exec();
    }

}
