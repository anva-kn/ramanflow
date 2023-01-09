#include <iostream>
#include <nlopt.h>

// Class that represents a function to be fit
class FunctionToFit
{
public:
    // Initialize the function with the given coefficients
    FunctionToFit(double* beta) : __beta(beta) {}

    // Fit function
    double fit_fun(double x)
    {
        // The fit function is a simple linear function: y = beta[0] + beta[1] * x
        return __beta[0] + __beta[1] * x;
    }

    // Loss function for the fit
    static double loss_fit_beta(unsigned n, const double* beta, double* grad, void* data)
    {
        // Unpack the data passed to the loss function
        double* x_data = static_cast<double*>(data);
        FunctionToFit* fit = static_cast<FunctionToFit*>(data);
        int n_data = static_cast<int*>(data)[n];

        // Initialize the loss to 0
        double loss = 0;

        // Iterate over the data points and compute the loss
        for (int i = 0; i < n_data; i++)
        {
            double x = x_data[i];
            double y = x_data[n_data + i];
            double yhat = fit->fit_fun(x);
            loss += (yhat - y) * (yhat - y);
        }

        return loss;
    }

    // Initial guess for the optimization
    double* get_beta_init()
    {
      static double beta_init[2] = {0, 0};
      return beta_init;
    }

    // Instance variables for the fit
    double* __beta;
    double __loss_inter;
    double __yhat_inter;
};

int main()
{
  // Set up the data for the fit
  int n = 10;
  double x_data[n] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  double y_data[n] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  int idx[n] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

  // Initialize the function to be fit
  double beta[2] = {0, 1};
  FunctionToFit fit(beta);

  // Set up the optimization parameters
  nlopt_opt opt;
  opt = nlopt_create(NLOPT_LN_NELDERMEAD, 2);
  nlopt_set_lower_bounds(opt, get_beta_init());
  nlopt_set_upper_bounds(opt, get_beta_init());
  nlopt_set_min_objective(opt, FunctionToFit::loss_fit_beta, &fit);

  // Optimize the loss function
  double minf;
  nlopt_optimize(opt, fit.get_beta_init(), &minf);

  // Print the results of the optimization
  std::cout << "Minimum loss: " << minf << std::endl;
  std::cout << "Optimized coefficients: " << beta[0] << " " << beta[1] << std::endl;

  return 0;

