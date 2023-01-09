#pragma once

#include <functional>
#include <tuple>
#include <vector>

using Callable = std::function<std::tuple<Any>(std::vector<float>&, std::vector<float>&)>;

// The SpectrumComponent class is a base class for modeling and fitting spectral data
class SpectrumComponent {
public:
  // The fit_fun field is a function that takes as input the x-values and fitting function parameters
  // and returns the predicted y-values
  Callable fit_fun;
  // The init_fit_fun field is a function that initializes the fitting function parameters
  Callable init_fit_fun;
  // The loss_fun field is a function that calculates the loss of a fit given the predicted and true y-values
  Callable loss_fun;

  // This method initializes the fitting function parameters with default values
  void init(std::vector<float>& x_data, std::vector<float>& y_data,
            std::tuple<size_t, size_t> range_of_indices = std::tuple<size_t, size_t>());

  // This method calculates the loss of a fit given the true and predicted y-values
  Any loss_fit(std::vector<float>& y_data, std::vector<float>& yh_data);

  // This method calculates the loss of a fit given the fitting function parameters and data points
  Any loss_fit_beta(std::vector<float>& beta, std::vector<float>& x_data, std::vector<float>& y_data);

  size_t get_beta_size();

  std::vector<float> get_beta_init();

  Any get_loss_fit();
};
