import numpy as np
import math


class Fit:
    """
    
    """
    __beta_params = {'poly4': 4, 'pseudo_voig': 4, 'gauss_amp': 3, 'gen_lor_amp': 4, 'lor_amp': 3}

    @classmethod
    def _get_beta_params(cls):
        return cls.__beta_params

    @staticmethod
    def poly4(x_data, beta):
        p = np.poly1d(beta)
        return p(x_data)

    @staticmethod
    def init_poly4(x_data, y_data):
        beta_int = np.polyfit(x_data, y_data, 4)
        return beta_int

    @staticmethod
    def pseudo_voig(x_data, beta):
        max = beta[0]
        center_pos = beta[1]
        fwhf = beta[2]
        nu = beta[3]

        sigma = fwhf / (2 * np.sqrt(2 * np.log(2)))
        sgs = sigma ** 2

        # G = [ M*math.exp(-(x_data[i]-c)**2/ (2*abs(sgs))) for i in range(x_data.size)]
        # L = [ M/(1+(abs((x_data[i]-c)/sgs))**2) for i in range(x_data.size)]

        # G=1/(sigma*np.sqrt(2*pi))*np.exp(-(x_data-c)**2/(2*sigma**2))
        # L=1/pi*(FWHM /2)/((x_data-c)**2+ (FWHM /2)**2)

        # return [ M*(nu*math.exp(-(x_data[i]-c)**2/ (2*abs(sgs)))+(1-nu)*(1/(1+(abs((x_data[i]-c)/sgs))**2))) for i in range(x_data.size)]
        return [max * (nu * math.exp(-(x_data[i] - center_pos) ** 2 / (2 * abs(sgs))) + (1 - nu) * (
                1 / (1 + (abs((x_data[i] - center_pos) / fwhf)) ** 2))) for i in range(x_data.size)]

    # ------------------------------------------------------------------------------

    @staticmethod
    def init_pseudo_voig(x_data, y_data):
        beta_init_lor = np.zeros(4)
        # beta_init_lor = {}
        beta_init_lor[0] = np.max(y_data)
        # center
        beta_init_lor[1] = x_data[np.argmax(y_data)]
        # gamma
        beta_init_lor[2] = (x_data[-1] - x_data[0]) / 16
        #
        beta_init_lor[3] = 0

        return beta_init_lor

    # generalize gaussian with any amplitude
    # for the bias, we will remove it from the fitting
    @staticmethod
    def gauss_amp(x_data, beta):
        # beta:
        # a
        # c
        # sgs
        M = beta[0]
        c = beta[1]
        sgs = beta[2]
        return [M * math.exp(-(x_data[i] - c) ** 2 / (2 * abs(sgs))) for i in range(x_data.size)]

    # TODO: ADD THE BIAS HERE!

    @staticmethod
    def gen_lor_amp(x_data, beta):
        # beta:
        # a
        # c
        # sgs
        M = beta[0]
        c = beta[1]
        sgs = beta[2]
        d = beta[3]
        return [M / (1 + (abs((x_data[i] - c) / sgs)) ** d) for i in range(x_data.size)]

    @staticmethod
    def lor_amp(x_data, beta):
        # beta:
        # a
        # c
        # sgs
        M = beta[0]
        c = beta[1]
        sgs = beta[2]
        d = 2
        return [M / (1 + (abs((x_data[i] - c) / sgs)) ** d) for i in range(x_data.size)]
