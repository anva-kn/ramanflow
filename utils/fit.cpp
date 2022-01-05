#include "fit.hpp"
// #include "../extern/pybind11/include/pybind11/pybind11.h"
#include <math.h>
#include <algorithm>
#include <iterator>

// namespace py = pybind11;


void PrintMap(std::map<std::string, int>& m)
{
    std::cout << "[ ";
    for (auto &item : m)
    {
        std::cout << item.first << ":" << item.second << " ";
    }
    std::cout << "]\n";
}

// std::vector<double> Fit :: PolyBiquadratic(std::vector<double> x_data, std::array<double, 4> beta)
// {
//
// }
//
//
// std::vector<double> Fit :: InitPolyBiquadratic(std::vector<double> x_data, std::vector<double> y_data)
// {
//
// }

std::vector<double> Fit :: PseudoVoight(std::vector<double> x_data, std::vector<double> beta)
{
    double max = beta[0];
    double center_pos = beta[1];
    double fwhm = beta[2];
    double nu = beta[3];
    double sigma, sgs;
    std::vector<double> result(x_data.size(), 0.0);

    sigma = fwhm / (2.0 * std::sqrt(2.0 * std::log(2.0)));
    sgs = std::pow(sigma, 2.0);

    for(std::size_t i = 0; i < x_data.size(); i++)
    {
        result[i] = max * (nu * std::exp(std::pow(-(x_data[i] - center_pos), 2.0) / (2.0 * std::abs(sgs))) + (1.0 - nu) * (1.0 / (1.0 + std::pow(std::abs((x_data[i] - center_pos) / fwhm), 2.0))));
    }

    return result;
}

std::array<double, 4> Fit :: InitPseudoVoight(std::vector<double> x_data, std::vector<double> y_data)
{
    std::array<double, 4> beta_init_lor = {};
    std::vector<double>::iterator max_element = std::max_element(y_data.begin(), y_data.end());
    int argmaxVal = std::distance(y_data.begin(), max_element);
    beta_init_lor[0] = *std::max_element(y_data.begin(), y_data.end());
    beta_init_lor[1] = x_data[*max_element];
    beta_init_lor[2] = (x_data.end() - x_data.begin()) / 16.0;
    beta_init_lor[3] = 0.0;
    return beta_init_lor;
}

std::vector<double> Fit :: GaussianAmplitude(std::vector<double> x_data, std::vector<double> beta)
{
    double max = beta[0];
    double center_pos = beta[1];
    double sgs = beta[2];
    std::vector<double> result(x_data.size(), 0.0);
    for(std::size_t i = 0; i < x_data.size(); i++)
    {
        result[i] = max * std::exp(std::pow(-(x_data[i] - center_pos), 2.0) / (2.0 * std::abs(sgs)));
    }
    return result;
}

std::vector<double> Fit :: GeneralizedLorentz(std::vector<double> x_data, std::vector<double> beta)
{
    double max = beta[0];
    double center_pos = beta[1];
    double sgs = beta[2];
    double d = beta[3];
    std::vector<double> result(x_data.size(), 0.0);

    for(std::size_t i = 0; i < x_data.size(); i++)
    {
        result[i] = max / (1 + std::pow(std::abs((x_data[i] - center_pos) / sgs), d));
    }

    return result;
}

std::vector<double> Fit :: LorentzAmplitude(std::vector<double> x_data, std::vector<double> beta)
{
    double max = beta[0];
    double center_pos = beta[1];
    double sgs = beta[2];
    double d = 2.0; 
    std::vector<double> result(x_data.size(), 0.0);

    for(std::size_t i = 0; i < x_data.size(); i++)
    {
        result[i] = max / (1 + std::pow(std::abs((x_data[i] - center_pos) / sgs), d));
    }

    return result;
}

// PYBIND11_MODULE(_fit, myFit) {
//   myfit.def("poly4_cpp", &PolyBiquadratic);
//   myfit.def("init_poly_cpp", &InitPolyBiquadratic);
//   myfit.def("pseudo_voight_cpp", &PseudoVoight);
//   myfit.def("init_pseudo_voight_cpp", &InitPseudoVoight);
//   myfit.def("gauss_amp_cpp" &GaussianAmplitude);
//   myfit.def("gen_lor_amp_cpp", &GeneralizedLorentz);
//   myfit.def("lor_amp_cpp", &LorentzAmplitude)
//   // py::class_<Fit>(myfit, "Fit_cpp")
//   //     .def("get_beta_params_cpp", &GetBetaParams);
// }


//
// int main(void)
// {
//     Fit *myfit = new Fit();
//     std::map<std::string, int> betaParameters = myfit->GetBetaParams();
//     std::cout << "__beta_params map : ";
//     myfit->PrintMap(betaParameters);
//     std::cout <<std::endl;
//     std::cout << betaParameters.size() << std::endl;
//     // std::string key = std::string("poly4");
//     std::cout << betaParameters[std::string("poly4")] << std::endl;
//
//     return EXIT_SUCCESS;
// }
