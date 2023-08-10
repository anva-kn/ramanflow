#!/usr/bin/env python3


import sys
sys.path.insert(0, '../utils')
sys.path.insert(0, '../models')
sys.path.insert(0, '../test_data')
from loss_functions import Loss
from fit_functions import Fit
from background import Background

import scipy.interpolate as si
import scipy.signal as sci

import pandas as pd
import numpy as np

import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
from math import pi
import math

#xls = pd.ExcelFile("/home/dtos/Coding/SLA/ICASSP/xlsx/clean_spectrum.xlsx")
# xls = pd.ExcelFile("clean_spectrum.xlsx")
test_data = np.load('../test_data/batch1.npy')
#f_sup = np.array(pd.read_excel(xls, 'f_sup')).squeeze()
f_sup = np.arange(0, 2400, 2)
#dict_data = xls.sheet_names
#data = {}
#for i in dict_data:
#    data[i] = np.array(pd.read_excel(xls, i))
#data_m = {}

#for i in dict_data:
#    data_m[i] = np.mean(np.array(pd.read_excel(xls, i)), axis=0)
#g = data_m["gly"] - data_m["water"]

y_data = test_data[1]
# normalize to 0/1
y_data = (y_data - np.min(y_data)) / (np.max(y_data) - np.min(y_data))
x_data = f_sup
nu = 0.01
slide_win = 10
loss = Loss()
fit = Fit()

# res = 1024
# interpol_pos = [0, 100, 250, 500, 750, res-1]
# loss = Loss()

background_model = Background(fit.poly4, fit.init_poly4, lambda y_pred, y_true: loss.l2_relu_loss(y_pred, y_true, 0.0001))


res = x_data.shape[-1]

# copy data, don't want to overwrite the original spectrum
y = y_data.copy()
x = x_data.copy()

# initialize the background
background_model.init(x, y)

interpol_err = [1000, 1000]

interpol_pos = [0, res - 1]

slide_win = 18
min_pos = 0
while (min_pos + 1) - res < -1:

    # -------------------- change this to start at curr_pos, reinitialize whenever we encounter a min
    tmp_interpol_pos = list(range(min_pos + 1, min((min_pos + 1) + slide_win, res - 1)))

    # debug
    # plt.plot(x_data, y_data)
    # plt.plot(x_data, yhat_inter)

    # this are the positions that we are testing for this position of the sliding window

    #  TODO change to min_err=background_model.max_loss
    min_err = 1000

    # gosh my indexing is awful
    # pick on position for every window where you "pin" the interpolation
    for tmp_interpol_po in tmp_interpol_pos:

        tmp_pos = np.concatenate((interpol_pos, [tmp_interpol_po]))

        y_hat_tmp = background_model.interpolate(x, y, tmp_pos)

        # weighted jumps:
        # including log seems to be a bit better
        tmp_err = background_model.get_loss_inter() / np.log(
            tmp_interpol_po - min_pos + 0.5
        )

        # update the minimum
        if tmp_err < min_err:
            min_pos = tmp_interpol_po
            min_err = tmp_err

    background_model.init(x[tmp_pos], y[tmp_pos])
    interpol_pos.append(min_pos)
    interpol_err.append(min_err)

    # sort points
sort_pos = np.argsort(interpol_pos)
interpol_pos = list(np.array(interpol_pos)[sort_pos.astype(int)])
interpol_err = list(np.array(interpol_err)[sort_pos.astype(int)])

# finally fitting
y_hat = background_model.interpolate(x, y, interpol_pos)

# spline interpolation just to see the peak support better
# y_bsp = si.interp1d(x_data[interpol_pos], y_data[interpol_pos], kind='cubic')(x_data)
y_bsp = si.interp1d(x[interpol_pos], y[interpol_pos], kind='linear')(x)

plt.figure("fitting positions + spline interpolation", figsize=(12, 10), dpi=80)
plt.title("fitting positions + spline interpolation")
plt.plot(x, y)
plt.plot(x, y_hat)
plt.plot(x_data, y_bsp,'.')
plt.legend(["Original Curve", "Y_hat", "Y_bsp"])
plt.show()
