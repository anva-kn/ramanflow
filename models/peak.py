import numpy as np
from dataclasses import dataclass, field
from spectrum_component import SpectrumComponent
from scipy.optimize import minimize
from typing import List, Optional


@dataclass
class Peak(SpectrumComponent):

    __beta_fit: np.ndarray = field(init=False)
    __yhat_fit: np.ndarray = field(init=False)

    def fit(self, x_data, y_data, range_of_indices=None):
        """

        Parameters
        ----------
        x_data
        y_data

        Returns
        -------

        """
        # find the coefficients beta that best fit the function
        # TODO: add verbose option
        if range_of_indices is not None:
            peak_pos = list(range(range_of_indices[0], range_of_indices[1], 1))
            result = minimize(self.loss_fit_beta, self.get_beta_init(), args=(x_data[peak_pos], y_data[peak_pos]),
                          method='Nelder-Mead',
                          tol=1e-12)
        else:
            result = minimize(self.loss_fit_beta, self.get_beta_init(), args=(x_data, y_data),
                              method='Nelder-Mead',
                              tol=1e-12)
        # params, params_covariance = curve_fit(self.fit_fun, x_data, y_rec, p0=self.__beta_init)
        self.__beta_fit = result.x
        self.__loss_fit = result.fun
        self.__yhat_fit = self.fit_fun(x_data, self.__beta_fit)
        return self.__yhat_fit

    def get_beta_init(self):
        return super().get_beta_init()

    def get_beta_size(self):
        return super().get_beta_size()

    def get_beta_fit(self):
        return self.__beta_fit

    def get_loss_fit(self):
        return self.__loss_fit

    def get_yhat_fit(self):
        return self.__yhat_fit

    def reset(self):
        self.__beta_fit = np.zeros(1)
        self.__yhat_fit = np.zeros(1)