from scipy.optimize import minimize, curve_fit
from dataclasses import dataclass
from typing import Any, Callable
from fit_functions import Fit

# The SpectrumComponent class is a base class for modeling and fitting spectral data
@dataclass
class SpectrumComponent:
    """
        # models should contain a
        # -fitting function and a loss after fitting
        # -an interpolation function
    """
    # The fit_fun field is a function that takes as input the x-values and fitting function parameters
    # and returns the predicted y-values
    fit_fun: Callable
    # The init_fit_fun field is a function that initializes the fitting function parameters
    init_fit_fun: Callable
    # The loss_fun field is a function that calculates the loss of a fit given the predicted and true y-values
    loss_fun: Callable

    # This method initializes the fitting function parameters with default values
    def init(self, x_data, y_data, range_of_indices=None):
        """Initialize the fitting function parameters with default values

        Parameters
        ----------
        x_data: np.ndarray
            The x-values of the data points
        y_data: np.ndarray
            The y-values of the data points
        range_of_indices: tuple, optional
            A tuple (start, end) indicating the range of indices to use in the initialization process.
            If not provided, all data points will be used.
        """
        # Initialize the fitting function parameters
        if range_of_indices is not None:
            # Use only the specified range of indices
            peak_pos = list(range(range_of_indices[0], range_of_indices[1], 1))
            self.__beta_init = self.init_fit_fun(x_data[peak_pos], y_data[peak_pos])
        else:
            # Use all data points
            self.__beta_init = self.init_fit_fun(x_data, y_data)

    # This method calculates the loss of a fit given the true and predicted y-values
    def loss_fit(self, y_data, yh_data):
        """Calculate the loss of a fit

        Parameters
        ----------
        y_data: np.ndarray
            The true y-values
        yh_data: np.ndarray
            The predicted y-values

        Returns
        -------
        loss: float
            The loss of the fit
        """
        return self.loss_fun(yh_data, y_data)

    # This method calculates the loss of a fit given the fitting function parameters and data points
    def loss_fit_beta(self, beta, x_data, y_data):
        """Calculate the loss of a fit given the fitting function parameters and data points

        Parameters
        ----------
        beta: np.ndarray
            The fitting function parameters
        x_data: np.ndarray
            The x-values of the data points
        y_data: np.ndarray
            The y-values of the data points

        Returns
        -------
        loss: float
            The loss"""
        return self.loss_fun(self.fit_fun(x_data, beta), y_data)

    def get_beta_size(self):
        fit_function = self.fit_fun.__name__
        beta_params = Fit._get_beta_params()
        return beta_params[fit_function]

    def get_beta_init(self):
        return self.__beta_init

    def get_loss_fit(self):
        return self.__loss_fit