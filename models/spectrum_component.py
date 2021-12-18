from scipy.optimize import minimize, curve_fit
from dataclasses import dataclass
from typing import Any, Callable
from fit_functions import Fit


@dataclass
class SpectrumComponent:
    """
        # models should contain a
        # -fitting function and a loss after fitting
        # -an interpolation function
    """
    fit_fun: Callable
    init_fit_fun: Callable
    loss_fun: Callable

    def init(self, x_data, y_data, range_of_indices=None):
        """

        Parameters
        ----------
        x_data
        y_data

        Returns
        -------

        """
        # initialize the function parameters in beta
        # note that you don't need to initialize all times you want to do fitting to new data

        # TODO: # add in verbose option
        if range_of_indices is not None:
            peak_pos = list(range(range_of_indices[0], range_of_indices[1], 1))
            self.__beta_init = self.init_fit_fun(x_data[peak_pos], y_data[peak_pos])
        else:
            self.__beta_init = self.init_fit_fun(x_data, y_data)


    def loss_fit(self, y_data, yh_data):
        """

        Parameters
        ----------
        y_data
        yh_data

        Returns
        -------

        """
        return self.loss_fun(yh_data, y_data)

    # loss function given a beta
    def loss_fit_beta(self, beta, x_data, y_data):
        """

        Parameters
        ----------
        beta
        x_data
        y_data

        Returns
        -------

        """
        return self.loss_fun(self.fit_fun(x_data, beta), y_data)

    def get_beta_size(self):
        fit_function = self.fit_fun.__name__
        beta_params = Fit._get_beta_params()
        return beta_params[fit_function]

    def get_beta_init(self):
        return self.__beta_init

    def get_loss_fit(self):
        return self.__loss_fit
