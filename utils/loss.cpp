#include "loss.hpp"
#include "poly1d.cpp"
// #include <pybind11.h>
// #include <stl.h>
// #include <operators.h>
// #include <numpy.h>
#include <numeric>

// namespace py = pybind11;

double Loss :: PositiveMse (std::vector<double> &y_pred, std::vector<double> &y_true)
{
    double loss = 0.0;
    std::vector<double> result(y_pred.size(), 0.0);
    std::vector<double> y_pred_copy;
    std::vector<double> ones(y_pred.size(), 1.0);
    std::vector<double> dot_prod_secnd_arg(y_pred.size(), 0.0), dot_prod_frst_arg(y_pred.size(), 0.0);
    // for(auto i : y_true) 
    // {
    //     y_pred_copy.push_back(y_true[i]);
    // }
    // for(std::size_t i = 0; i < y_pred_copy.size(); i++)
    // {
    //     if(y_pred[i] - y_true[i] > 0)
    //     {
    //         dot_prod_frst_arg[i] = std::pow(10.0, 8.0);
    //     }
    //     dot_prod_secnd_arg[i] = std::pow(y_pred[i] - y_true[i], 2.0);
    // }
    // loss = std::inner_product(dot_prod_frst_arg.begin(), dot_prod_frst_arg.end(), dot_prod_secnd_arg.begin(), 0.0);
    return 0.0;//loss / double(y_true.size());
}
double Loss :: PositiveMseLoss(std::vector<double> &beta, std::vector<double> &x_data, std::vector<double> &y_data)
{
    double result;
    double size = double(y_data.size());
    std::vector<double> temp;
    Poly1d poly;
    poly.init(beta);
    std::vector<double> eval = poly.evaluate(x_data);
    std::cout << "Problem in PositiveMseLoss?  " << std::endl;
    for (std::size_t i = 0; i < x_data.size(); i++)
    {
        temp.push_back(PositiveMse(eval, y_data));
    }
    std::cout << "Not in the loop..." << std::endl;
    result = std::accumulate(temp.begin(), temp.end(), decltype(temp)::value_type(0));
    std::cout << "Not in the sum transpose...";
    return result / size;
}

double Loss :: L2ReluLoss(std::vector<double> &y_pred, std::vector<double> &y_true, double nu)
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

double Loss :: PeakMseLoss(std::vector<double> &y_pred, std::vector<double> &y_true)
{
    std::vector<double>::iterator max_element = std::max_element(y_true.begin(), y_true.end());
    std::vector<double>::iterator min_element = std::min_element(y_true.begin(), y_true.end());
    double al = *max_element - *min_element;
    std::vector<double> diff(y_true.size(), 0.0);
    std::cout << "Problem in the PeakMseLoss?  " << std::endl;
    std::transform(y_pred.begin(), y_pred.end(), y_true.begin(), diff.begin(),std::minus<double>());
    std::cout << "Not in diff transpose" << std::endl;
    for(auto i : diff)
    {
        std::cout << i << " ";
    }
    std::cout << "not in accessing diff vector..." << std::endl;
    return al;
}

// double Loss :: MseLoss(std::vector<double> y_pred, std::vector<double> y_true)
// {
//
// }

int main(void)
{
    std::cout << "Problem here?" << std::endl;
    Loss loss;
    std::cout << "Nope" << std::endl;
    std::vector<double> y1{1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<double> y2{2.0, 3.0, 4.0, 5.0, 7.0};
    std::cout << "Nope " << std::endl;
    loss.PeakMseLoss(y1, y2);
    return 0;

}

// PYBIND11_MODULE(loss, myLoss) {
//     py::class_<Loss>(myLoss, "Loss")
//     .def(py::init<>())
//     .def("positive_mse_cpp", &Loss::PositiveMse)
//     .def("positive_mse_loss_cpp", &Loss::PositiveMseLoss)
//     .def("l2_relu_loss_cpp", &Loss::L2ReluLoss)
//     .def("peak_mse_loss_cpp", &Loss::PeakMseLoss)
//     .def("mse_loss_cpp", &Loss::MseLoss)
// }
//
