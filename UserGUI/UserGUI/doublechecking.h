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

#ifndef DOUBLECHECKING_H
#define DOUBLECHECKING_H

#include <QDialog>

//The DoubleChecking class is used to propose a "Are you sure? okay/cancel" panel to the user.

namespace Ui {
class DoubleChecking;
}

class DoubleChecking : public QDialog
{
    friend class MainWindow;
    Q_OBJECT

public:
    explicit DoubleChecking(QWidget *parent = 0);
    ~DoubleChecking();

private:
    Ui::DoubleChecking *ui;
};

#endif // DOUBLECHECKING_H
