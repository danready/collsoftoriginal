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

#include <assert.h>
#include <math.h>
#include "Linear.h"

//! Given a set of points, this class calculates the linear regression parameters and evaluates the regression line at arbitrary abscissas.


Maths::Regression::Linear::Linear(int n, double *x, double *y)
{

            // calculate the averages of arrays x and y
            double xa = 0, ya = 0;
            for (int i = 0; i < n; i++) {
                xa += x[i];
                ya += y[i];
            }
            xa /= n;
            ya /= n;

            // calculate auxiliary sums
            double xx = 0, yy = 0, xy = 0;
            for (int i = 0; i < n; i++) {
                double tmpx = x[i] - xa, tmpy = y[i] - ya;
                xx += tmpx * tmpx;
                yy += tmpy * tmpy;
                xy += tmpx * tmpy;
            }

            // calculate regression line parameters

            // make sure slope is not infinite
            assert(fabs(xx) != 0);

                m_b = xy / xx;
                m_a = ya - m_b * xa;
            m_coeff = (fabs(yy) == 0) ? 1 : xy / sqrt(xx * yy);

}

Maths::Regression::Linear::Linear(double m_b_arg, double m_a_arg, double m_coeff_arg)
{
	//! Returns the slope of the regression line

	setSlope(m_b_arg);

	//! Returns the intercept on the Y axis of the regression line

	setIntercept(m_a_arg);

	//! Returns the linear regression coefficient

	setCoefficient(m_coeff_arg);
}

//! Evaluates the linear regression function at the given abscissa.

double Maths::Regression::Linear::getValue(double x)
{
    return m_a + m_b * x;
}

//! Returns the slope of the regression line

double Maths::Regression::Linear::getSlope()
{
   return m_b;
}

//! Returns the intercept on the Y axis of the regression line

double Maths::Regression::Linear::getIntercept()
{
  return m_a;
}

//! Returns the linear regression coefficient

double Maths::Regression::Linear::getCoefficient()
{
  return m_coeff;
}

//! Returns the slope of the regression line

double Maths::Regression::Linear::setSlope(double m_b_arg)
{
   m_b = m_b_arg;
   return m_b;
}

//! Returns the intercept on the Y axis of the regression line

double Maths::Regression::Linear::setIntercept(double m_a_arg)
{
  m_a = m_a_arg;
  return m_a;
}

//! Returns the linear regression coefficient

double Maths::Regression::Linear::setCoefficient(double m_coeff_arg)
{
  m_coeff = m_coeff_arg;
  return m_coeff;
}
