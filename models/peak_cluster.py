from scipy.optimize import minimize
from dataclasses import dataclass, field
from peak import Peak
import numpy as np
import scipy.signal as sci


@dataclass
class PeakCluster(Peak):
    """This is a class to fit N peaks into  a subset of data
    # TODO: For the Spectrum class, we need to be able to compose N functions and store the fitting
    Parameters:
        for init function: x_data, y_data, size(num of peaks)
        for fit function: x_data, y_data
    """
    __size: int = 0
    __beta_init: np.ndarray = field(init=False)

    def init(self, x_data, y_data):
        # self.__size = size
        num_of_peaks = self.__size
        step = int(y_data.shape[-1] / num_of_peaks)
        self.__beta_init = np.zeros(num_of_peaks * self.get_beta_size())
        
        peaks, properties = sci.find_peaks(y_data, prominence=0.01) #should prominence be a parameter of init?
        prominences = properties['prominences']    
        peak_width = properties['right_bases'] - properties['left_bases']
        l_ips = properties['left_bases'].astype(int)
        r_ips = properties['right_bases'].astype(int)
        
        if len(y_data) < self.get_beta_size() * num_of_peaks:
            # in this case, we can not do the fitting properly
            raise Exception("Not enough data to fit the given number of overall parameters")

        for i in range(num_of_peaks):
            temp_component = Peak(self.fit_fun, self.init_fit_fun, self.loss_fun)
            # temp_component.init(x_data[i * step:(i + 1) * step], y_data[i * step:(i + 1) * step])
            temp_component.init(x_data, y_data, [i * step, (i + 1) * step])
            self.__beta_init[i * self.get_beta_size(): (i + 1) * self.get_beta_size()] = temp_component.get_beta_init()

    def fit(self, x_data, y_data):
        """

        Parameters
        ----------
        x_data
        y_data

        Returns
        -------

        """
        result = minimize(self.loss_fit_beta_N, self.__beta_init, args=(x_data, y_data),
                          method='Nelder-Mead',
                          tol=1e-12)

        # TODO: add verbose option
        print("fit minimize", result.x)
        self.__beta_fit = result.x
        self.__loss_fit = result.fun
        self.__yhat_fit = self.fit_fun_N(x_data, self.__beta_fit)

        return self.__yhat_fit

    def fit_fun_N(self, x_data, beta):
        """Here we fit each peak by stacking the function

        Parameters
        ----------
        x_data
        beta

        Returns
        -------
        Fitted data
        """
        y = np.zeros(x_data.shape[0])

        for i in range(self.__size):
            y += self.fit_fun(x_data, beta[i * self.get_beta_size():(i + 1) * self.get_beta_size()])

        return y

    def loss_fit_beta_N(self, beta, x_data, y_data):
        """

        Parameters
        ----------
        beta
        x_data
        y_data

        Returns
        -------

        """
        return self.loss_fun(self.fit_fun_N(x_data, beta), y_data)

    def get_beta_init(self):
        """Utility function

        Returns
        -------
        __beta_init
        """
        return self.__beta_init
        