#include "SpectrumComponent.hpp"
#include <cmath>

double SpectrumComponent::loss_fun(std::vector<double> y, std::vector<double> yhat) {
    double loss = 0.0;
    for (int i = 0; i < y.size(); i++) {
        loss += pow(y[i] - yhat[i], 2);
    }
    return loss;
}