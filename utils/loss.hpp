#ifndef LOSS_HPP_INCLUDED
#define LOSS_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <math.h>

class Loss
{
    public:
        // Basic constructors.
        Loss() = default; // default constructor.
        Loss(Loss const & ); // copy constructor.
        Loss(Loss       &&); // move constructor.
        Loss & operator=(Loss const & ); // copy assignment operator.
        Loss & operator=(Loss       &&); // move assignment operator.
        ~Loss() = default; // default destruct

        double PositiveMse(std::vector<double> &y_pred, std::vector<double> &y_true);
        double PositiveMseLoss(std::vector<double> &beta, std::vector<double> &x_data, std::vector<double> &y_data);
        double L2ReluLoss(std::vector<double> &y_pred, std::vector<double> &y_true, double nu);
        double PeakMseLoss(std::vector<double> &y_pred, std::vector<double> &y_true);
        double MseLoss(std::vector<double> &y_pred, std::vector<double> &y_true);
};

#endif // LOSS_HPP_INCLUDED
