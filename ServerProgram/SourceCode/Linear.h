// GNU General Public License Agreement
// Copyright (C) 2004-2010 CodeCogs, Zyba Ltd, Broadwood, Holford, TA5 1DU, England.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by CodeCogs. 
// You must retain a copy of this licence in all copies. 
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// ---------------------------------------------------------------------------------
//! Calculates the linear regression parameters and evaluates the regression line at arbitrary abscissas

#ifndef MATHS_REGRESSION_LINEAR_H
#define MATHS_REGRESSION_LINEAR_H

#include <assert.h>
#include <math.h>

namespace Maths
{

namespace Regression
{

//! Given a set of points, this class calculates the linear regression parameters and evaluates the regression line at arbitrary abscissas.

class Linear
{
public:

//! Class constructor

Linear(int n, double *x, double *y);

Linear(double m_b_arg, double m_a_arg, double m_coeff_arg);

//! Evaluates the linear regression function at the given abscissa.

double getValue(double x);

//! Returns the slope of the regression line

double getSlope();

//! Returns the intercept on the Y axis of the regression line

double getIntercept();

//! Returns the linear regression coefficient

double getCoefficient();

//! Returns the slope of the regression line

double setSlope(double m_b_arg);

//! Returns the intercept on the Y axis of the regression line

double setIntercept(double m_a_arg);

//! Returns the linear regression coefficient

double setCoefficient(double m_coeff_arg);

private:

double m_a, m_b, m_coeff;
};


}

}

#endif

