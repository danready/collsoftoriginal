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

#include "doublechecking.h"
#include "ui_doublechecking.h"

//The DoubleChecking class is used to propose a "Are you sure? okay/cancel" panel to the user.

DoubleChecking::DoubleChecking(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoubleChecking)
{
    ui->setupUi(this);
}

DoubleChecking::~DoubleChecking()
{
    delete ui;
}
