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

#ifndef GENARALDIALOG_H
#define GENERALDIALOG_H

#include <QDialog>

//The GeneralDialog class is used to highlight information to the user.

namespace Ui {
class GeneralDialog;
}

class GeneralDialog : public QDialog
{

friend class MainWindow;

    Q_OBJECT

public:
    explicit GeneralDialog(QString message = "", QWidget *parent = 0);
    ~GeneralDialog();

private slots:
void on_OkButton_GeneralDialog_clicked();

private:
    Ui::GeneralDialog *ui;
};

#endif // GENERALDIALOG_H
