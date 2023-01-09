import numpy as np
from dataclasses import dataclass, field
from spectrum_component import SpectrumComponent
import spectrum_component
from scipy.optimize import minimize

# The Background class is a subclass of the SpectrumComponent class
# that is used to model and interpolate background spectral data
@dataclass
class Background(SpectrumComponent):

    # The following instance variables are used to store the results of the interpolation process
    # They are initialized to empty arrays and are not intended to be set directly by the user
    __beta_inter: np.ndarray = field(init=False)  # The fitting function parameters for the interpolation
    __yhat_inter: np.ndarray = field(init=False)  # The predicted y-values for the interpolation

    # This method finds the fitting function parameters that best fit the data over a specified set of indices
    # and stores the results in the __beta_inter and __yhat_inter instance variables
    def interpolate(self, x_data, y_data, idx):
        """Find the coefficients beta that best fit the function over the set of indexes in idx

        Parameters
        ----------
        x_data: np.ndarray
            The x-values of the data points
        y_data: np.ndarray
            The y-values of the data points
        idx: np.ndarray
            The indices of the data points to use in the fit

        Returns
        -------
        yhat_inter: np.ndarray
            The predicted y-values of the fit
        """
        # Use the Nelder-Mead optimization algorithm to find the best fit
        result = minimize(self.loss_fit_beta, self.get_beta_init(), args=(x_data[idx], y_data[idx]),
                          method='Nelder-Mead', tol=1e-12)

        # Store the results of the optimization in the __beta_inter and __yhat_inter instance variables
        self.__beta_inter = result.x
        self.__loss_inter = result.fun
        self.__yhat_inter = self.fit_fun(x_data, self.__beta_inter)
        return self.__yhat_inter

    # These methods are getters that return the values of the corresponding instance variables
    def get_beta_init(self):
        return super().get_beta_init()

    def get_beta_size(self):
        return super().get_beta_size()

    def get_beta_inter(self):
        return self.__beta_inter

    def get_loss_inter(self):
        return self.__loss_inter

    def get_yhat_inter(self):
        return self.__yhat_inter

    # This method resets the instance variables to their default values
    def reset(self):
        self.__beta_inter = np.zeros(1)
        self.__yhat_inter = np.zeros(1)
