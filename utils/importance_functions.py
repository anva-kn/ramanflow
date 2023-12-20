import numpy as np
from scipy.optimize import minimize
from scipy.signal import chirp, find_peaks, peak_widths
import math


class Importance:
    """
        # some functions that we need here. Better integrate with more functions
        # TODO: Anvar: I propose that every function returns score to keep the class methods consistent
    """

    @staticmethod
    def area(y_data):
        return np.sum(y_data)

    @staticmethod
    def high(y_data):
        return np.max(y_data)

    @staticmethod
    def mean_level(y_data):
        return np.sum(y_data) / y_data.size

    @staticmethod
    def box(y_data):
        return np.max(y_data) * y_data.size

    @staticmethod
    def sharpness(y_data):
        return np.sum(np.diff(y_data) ** 2) / y_data.size

    @staticmethod
    def fwhm(y_data):
        # TODO: complete full_width_at_half_max
        # TODO: double check this function
        peak = np.argmax(y_data)
        results = peak_widths(y_data, np.array([peak]), rel_height=0.5, prominence_data=None, wlen=None)
        return results[0]

    @staticmethod
    def trapezoid(self):
        # TODO: complete trapezoid
        pass

    @staticmethod
    def asymmetry(x_data, y_data):
        """This function is based on https://www.chromatographyonline.com/view/peak-shapes-and-their-measurements-need-and-concept-behind-total-peak-shape-analysis-0
        The idea is to compute the ratio b / a
        where a is distance from leading edge of the peak to the peak maxima at 10% peak height
        and b is distance from peak maxima to the trailing edge at 10% peak height

        1 - perfect symmetry
        < 1 - net fronting
        > 1 - net tailing
        Parameters
        -------
        x_data -> frequency support in the estimated peak interval
        y_data -> data points in the estimated peak interval without background

        Returns
        -------
        score -> the score of peak importance
        """
        peak = np.argmax(y_data)
        results_tenth = peak_widths(y_data, np.array([peak]), rel_height=0.9, prominence_data=None)
        left_edge, right_edge = round(results_tenth[2][0]), round(results_tenth[3][0])
        a = x_data[peak] - x_data[left_edge]
        b = np.abs(x_data[peak] - x_data[right_edge])
        return b / a

    @staticmethod
    def gaussian_peak_area(y_data):
        peak = np.argmax(y_data)
        fwhm = peak_widths(y_data, np.array([peak]), rel_height=0.5, prominence_data=None)
        peak_height = y_data[peak]
        gaussian_area = (peak_height * fwhm) / (2.35 * 0.3989)

    @staticmethod
    def areas_ratio(y_data):
        """This function takes into the account the ratio between the area under FWHM line and the whole area under the
        curve

        Parameters
        -------
        y_data -> data points in the estimated peak interval without background
        Returns
        -------
        score -> the score of peak importance
        """
        peak = np.argmax(y_data)
        results_half = peak_widths(y_data, np.array([peak]), rel_height=0.5, prominence_data=None)
        left_edge, right_edge = round(results_half[2][0]), round(results_half[3][0])
        fwhm_height = results_half[1][0]
        total_area = np.sum(y_data)
        fwhm_area = fwhm_height * (right_edge - left_edge)
        return fwhm_area / total_area

    @staticmethod
    def house_fit(y_data):
        # approxiate the peak as triangle on top of a rectangle
        # the importance is the ratio between the area and this approximation
        peak = np.argmax(y_data)
        results = peak_widths(y_data, np.array([peak]), rel_height=0.5, prominence_data=None, wlen=None)
        triangle_high = y_data[peak]
        box_high = results[1]
        base = results[0]
        house_area = (triangle_high*base/2)+(base*box_high)
        total_area = np.sum(y_data)
        return house_area/total_area/len(y_data)

    # TODO: Think of more functions that will determine importance of the peak ...
