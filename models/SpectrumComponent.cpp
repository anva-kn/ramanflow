#include "SpectrumComponent.hpp"

// This method initializes the fitting function parameters with default values
void SpectrumComponent::init(std::vector<float>& x_data, std::vector<float>& y_data,
                             std::tuple<size_t, size_t> range_of_indices) {
  // Initialize the fitting function parameters
  if (range_of_indices) {
    // Use only the specified range of indices
    std::vector<float> peak_pos;
    for (size_t i = std::get<0>(range_of_indices); i < std::get<1>(range_of_indices); ++i) {
      peak_pos.push_back(i);
    }
    beta_init_ = init_fit_fun(x_data, y_data, peak_pos);
  } else {
    // Use all data points
    beta_init_ = init_fit_fun(x_data, y_data);
  }
}

// This method calculates the loss of a fit given the true and predicted y-values
Any SpectrumComponent::loss_fit(std::vector<float>& y_data, std::vector<float>& yh_data) {
  // Calculate the loss of a fit
  return loss_fun(yh_data, y_data);
}

// This method calculates the loss of a fit given the fitting function parameters and data points
Any SpectrumComponent::loss_fit_beta(std::vector<float>& beta, std::vector<float>& x_data,
                                     std::vector<float>& y_data) {
  // Calculate the loss of a fit given the fitting function parameters and data points
  return loss_fun(fit_fun(x_data, beta), y_data);
}

size_t SpectrumComponent::get_beta_size() {
  // Get the size of the beta parameter vector
  return Fit::GetBetaParams()[fit_fun.__name__];
}

std::vector<float> SpectrumComponent::get_beta_init() { return beta_init_; }

Any SpectrumComponent::get_loss_fit() { return loss_fit_; }
