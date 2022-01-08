#include "fit.hpp"
#include <string>
#include "poly1d.cpp"
#include <math.h>
#include <algorithm>
#include <iterator>
#include <pybind11.h>
#include <stl.h>
#include <operators.h>
#include <numpy.h>

namespace py = pybind11;

std::vector<double> Fit :: PolyBiquadratic(std::vector<double> x_data, std::vector<double> beta)
{
    Poly1d poly;
    poly.init(beta);
    std::vector<double> fit_result(x_data.size(), 0.0);
    fit_result = poly.evaluate(x_data);
    return fit_result;
}

std::vector<double> Fit :: InitPolyBiquadratic(std::vector<double> x_data, std::vector<double> y_data, int order)
{
    std::vector<double> beta_init;
    std::vector<double> coefficients;
    double coeff_0 = 0.0;
    Poly1d poly;
    std::cout << "Starting fitting... " << std::endl;
    poly.fit(x_data, y_data, coefficients, order);
    std::cout << "Finished fitting. " << std::endl;
    std::cout << "Calculating beta_init... " << std::endl;
    for(auto check : coefficients)
    {
        std::cout << check << " ";
    }
    for (int i = 0; i < x_data.size(); i++)
    {
        std::cout << "Problem at coeff[0]? " << std::endl;
        double fit_val = coefficients[0];
        std::cout << "Nope..." << std::endl; 
        for (int j = 1; j < coefficients.size(); j++)
        {
            std::cout << "How about coeff[j]? " << std::endl;
            std::cout << coefficients[j] << " ";
           fit_val += coefficients[j] * std::pow(x_data.at(i), j);
        }
        //fit_val += coeff_0;
        beta_init.push_back(fit_val);
    }
    std::cout << "Finished calculating beta_init." << std::endl;
    //beta_init = np.polyfit(x_data, y_data, 4)
    return beta_init;
}

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

std::vector<double> Fit :: InitPseudoVoight(std::vector<double> &x_data, std::vector<double> &y_data)
{
    std::vector<double> beta_init_lor = {};
    std::vector<double>::iterator max_element = std::max_element(y_data.begin(), y_data.end());
    int argmaxVal = std::distance(y_data.begin(), max_element);
    beta_init_lor[0] = *std::max_element(y_data.begin(), y_data.end());
    beta_init_lor[1] = x_data[argmaxVal];
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

PYBIND11_MODULE(fit, myFit) {
    py::class_<Fit>(myFit, "Fit")
    .def(py::init<>())
    .def("poly4_cpp", &Fit::PolyBiquadratic)
    .def("init_poly_cpp", &Fit::InitPolyBiquadratic)
    .def("pseudo_voight_cpp", &Fit::PseudoVoight)
    .def("init_pseudo_voight_cpp", &Fit::InitPseudoVoight)
    //.def("gauss_amp_cpp" &Fit::GaussianAmplitude)
    .def("gen_lor_amp_cpp", &Fit::GeneralizedLorentz)
    .def("lor_amp_cpp", &Fit::LorentzAmplitude);

}



//int main(void)
//{
//  Fit *myfit = new Fit();
//  std::map<std::string, int> betaParameters = myfit->GetBetaParams();
//  std::cout << "__beta_params map : ";
//  myfit->PrintMap(betaParameters);
//  std::cout <<std::endl;
//  std::cout << betaParameters.size() << std::endl;
//  // std::string key = std::string("poly4");
//  std::cout << betaParameters[std::string("poly4")] << std::endl;
//
//  return EXIT_SUCCESS;
//}
