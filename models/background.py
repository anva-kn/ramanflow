import numpy as np
from dataclasses import dataclass, field
from spectrum_component import SpectrumComponent
from scipy.optimize import minimize


@dataclass
class Background(SpectrumComponent):

    __beta_inter: np.ndarray = field(init=False)
    __yhat_inter: np.ndarray = field(init=False)

    def interpolate(self, x_data, y_data, idx):
        """Find the coefficients beta that best fit the function over the set of indexes in idx

        Parameters
        ----------
        x_data
        y_data
        idx

        Returns
        -------
        yhat_inter
        """
        result = minimize(self.loss_fit_beta, self.get_beta_init(), args=(x_data[idx], y_data[idx]),
                          method='Nelder-Mead', tol=1e-12)

        # print("inter minimize", result.x)
        self.__beta_inter = result.x
        self.__loss_inter = result.fun
        self.__yhat_inter = self.fit_fun(x_data, self.__beta_inter)
        return self.__yhat_inter


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

    def reset(self):
        self.__beta_inter = np.zeros(1)
        self.__yhat_inter = np.zeros(1)
