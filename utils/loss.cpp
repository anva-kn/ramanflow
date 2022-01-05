#include "loss.hpp"


double PositiveMse(std::vector<double> y_pred, std::vector<double> y_true)
{
    double loss = 0.0;
    std::vector<double> result(y_pred.size(), 0.0);
    std::vector<double> y_pred_copy;
    std::vector<double> ones(y_pred.size(), 1.0);
    std::vector<double> dot_prod_secnd_arg(y_pred.size(), 0.0), dot_prod_frst_arg(y_pred.size(), 0.0);
    for(auto i : y_true) y_pred_copy.push_back(y_true[i]);
    for(std::size_t i = 0; i < y_pred_copy.size(); i++)
    {
        if(y_pred[i] - y_true[i] > 0)
        {
            dot_prod_frst_arg[i] = std::pow(10.0, 8.0);
        }
        dot_prod_secnd_arg[i] = std::pow(y_pred[i] - y_true[i], 2.0);
    }
    loss = std::inner_product(dot_prod_frst_arg.begin(), dot_prod_frst_arg.end(), dot_prod_secnd_arg.begin(), 0.0);
    return loss / double(y_true.size());
}
// double PositiveMseLoss(std::vector<double> y_pred, std::vector<double> y_true)
// {
//
// }

double L2ReluLoss(std::vector<double> y_pred, std::vector<double> y_true, double nu)
{
    double loss = 0.0;
    std::vector<double> result(y_pred.size(), 0.0);
    std::vector<double> y_pred_copy;
    std::vector<double> ones(y_pred.size(), 1.0);
    std::vector<double> dot_prod_secnd_arg(y_pred.size(), 0.0), dot_prod_frst_arg(y_pred.size(), 0.0);
    for(auto i : y_true) y_pred_copy.push_back(y_true[i]);
    for(std::size_t i = 0; i < y_pred_copy.size(); i++)
    {
        if(y_pred[i] - y_true[i] > 0)
        {
            dot_prod_frst_arg[i] = std::pow(10.0, 8.0);
        }
        dot_prod_secnd_arg[i] = std::abs(y_pred[i] - y_true[i]);
    }
    loss = std::inner_product(dot_prod_frst_arg.begin(), dot_prod_frst_arg.end(), dot_prod_secnd_arg.begin(), 0.0);
    return loss / double(y_true.size());

}

double PeakMseLoss(std::vector<double> y_pred, std::vector<double> y_true)
{

}

double MseLoss(std::vector<double> y_pred, std::vector<double> y_true)
{

}

