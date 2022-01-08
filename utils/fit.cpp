#include "fit.hpp"
#include "polynomial.hpp"
#include <string>
// #include "../extern/pybind11/include/pybind11/pybind11.h"
#include <math.h>
#include <algorithm>
#include <iterator>
// #include <Python.h>
#include <pybind11.h>
#include <stl.h>
#include <operators.h>
#include <numpy.h>

namespace py = pybind11;

using namespace std;

void Fit :: PolyBiquadratic(std::vector<double> x_data, double *beta, int order, std::vector<double> eval_pts)
{
    int poly_size = int(x_data.size());
    int ordr = order;
    int exp[poly_size];
    double x_data_arr[x_data.size()];
    for (std::size_t i = 0; i < poly_size; i++)
    {
        exp[i] = poly_size - i;
        x_data_arr[i] = x_data[i];
    }
//  polynomial_print(1, poly_size, beta);
//    double c[11];
    double c1[6] = { 7.0, - 5.0, 9.0, 11.0, 0.0, - 13.0 };
    double c2[5] = { 2.0, 3.0, -8.0, 4.0, 9.0 };
    int m = 1;
    int e[11];
    int e1[6] = { 1, 2, 4, 5, 12, 33 };
    int e2[5] = { 1, 3, 4, 30, 33 };
    int o;
    int o1 = 6;
    int o2 = 5;
    string title = "  P1(X) + P2(X) =";
    string title1 = "  P1(X) =";
    string title2 = "  P2(X) =";
    cout << "\n";
    cout << "ordr: " << ordr << endl;
    cout << "poly_size: " << poly_size << endl;
    cout << "x_data: ";
    for (auto i : x_data)
    {
        cout << x_data[i] << " ";
    }
    cout << "exp: ";
    for (int i = 0; i < sizeof(exp) / sizeof(exp[0]); i++)
    {
        cout << exp[i] << " ";
    }
    cout << endl;
    cout << "m: " << m << endl;
    cout << "o1: " << o1 << endl;
    cout << "c1: ";
    for (int i = 0; i < sizeof(c1) / sizeof(c1[0]); i++)
    {
        cout << c1[i] << " ";
    }
    cout << "e1: ";
    for (int i = 0; i < sizeof(e1) / sizeof(e1[0]); i++)
    {
        cout << e1[i] << " ";
    }
    double *c, *p;
    double points[eval_pts.size()];
    double x[3*2] = {
                    1.0, 2.0, 3.0,
                    -2.0, 4.0, 1.0 };
    polynomial_print(ordr, poly_size, x_data_arr, exp, title1);
    c = polynomial_value(ordr, poly_size, x_data_arr, exp, 1, points);
    cout << endl;
    polynomial_print ( m, o1, c1, e1, title1 );
    p = polynomial_value ( m, o, c, e, 2, x );

    cout << "\n";
    for (int j = 0; j < 2; j++ )
    {
        cout << "  P(" << x[0+j*m]
        << "," << x[1+j*m]
        << "," << x[2+j*m]
        << ") = " << p[j] << "\n";
    }

    delete [] p;
    //  p = np.poly1d(beta)
    cout << "\n";
    cout << "c: "
    for (int j = 0; j < 4; j++ )
    {
        cout << c[j] << " ";
    }

    delete [] c;
//  return p(x_data)
}


//std::vector<double> Fit :: InitPolyBiquadratic(std::vector<double> x_data, std::vector<double> y_data)
//{
//  beta_init = np.polyfit(x_data, y_data, 4)
//  return beta_init
//}

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
//  .def("get_beta_params_cpp", &Fit::GetBetaParams)
    .def("poly4_cpp", &Fit::PolyBiquadratic)
    //.def("init_poly_cpp", &Fit::InitPolyBiquadratic)
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
