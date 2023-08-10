import numpy as np
from scipy.optimize import minimize
import math


class Loss:
    """
        # some functions that we need here. Better integrate with more functions
    """

    @staticmethod
    def positive_mse(y_pred, y_true):
        loss = np.dot((10 ** 8 * (y_pred - y_true > 0) + np.ones(y_true.size)).T, (y_pred - y_true) ** 2)
        return np.sum(loss) / y_true.size

    def positive_mse_loss(self, beta, x_data, y_data):
        p = np.poly1d(beta)
        return (
            sum(
                self.positive_mse(p(x_data[i]), y_data[i])
                for i in range(x_data.size)
            )
            / y_data.size
        )

    @staticmethod
    def l2_relu_loss(y_pred, y_true, nu):
        # keep mu between 0 and 1
        # loss=mu*np.dot((mu(y_pred-y_true>0)+np.ones(y_true.size)).T,(y_pred-y_true)**2)
        loss = (1 - nu) * np.dot((y_pred - y_true > 0), np.abs((y_pred - y_true))) + nu * np.linalg.norm(
            y_pred - y_true)
        return loss / y_true.size

    @staticmethod
    def peak_mse_loss(y_pred, y_true):
        # an mse loss where the loss is weighted by the high of the peak itself, so we git th height better
        # scaling of the max vs  the min of the true data
        al = np.max(y_true) - np.min(y_true)
        loss = np.multiply(y_true, (y_pred - y_true) ** 2) / al
        return sum(loss) / y_true.size

    @staticmethod
    def mse_loss(y_pred, y_true):
        # an mse loss where the loss is weighted by the high of the peak itself, so we git th hight better
        # scaling of the max vs  the min of the true data
        return np.linalg.norm(y_pred - y_true)
