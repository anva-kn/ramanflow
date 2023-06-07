#ifndef SPECTRUM_COMPONENT_HPP
#define SPECTRUM_COMPONENT_HPP

#include <vector>

class SpectrumComponent {
public:
    SpectrumComponent() {}

    virtual ~SpectrumComponent() {}

    virtual std::vector<double> fit_fun(std::vector<double> x, std::vector<double> beta) = 0;

    virtual std::vector<double> init_fit_fun(std::vector<double> x, std::vector<double> y, std::vector<double> idx) = 0;

    virtual double loss_fun(std::vector<double> y, std::vector<double> yhat);

    virtual std::vector<double> get_beta_init() = 0;

    virtual int get_beta_size() = 0;
};

#endif // SPECTRUM_COMPONENT_HPP