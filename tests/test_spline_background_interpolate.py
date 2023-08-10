from loss_functions import Loss
from fit_functions import Fit
from importance_functions import Importance
# TODO: merge once done
import pandas as pd
import numpy as np
import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
from math import pi

from background import Background
from peak import Peak

# from playground_v6 import spline_remove_est_fluorescence
# from playground_v6 import find_peak_supp

# def spline_remove_est_fluorescence(x_data, y_data, background_model, peak_model, max_peak_width, min_peak_width=18):
#     # obtain length of x_data
#     res = x_data.shape[-1]

#     # copy data, don't want to overwrite the original spectrum
#     y = y_data.copy()
#     x = x_data.copy()

#     # initialize the background
#     background_model.init(x, y)

#     # TODO change to  background_model.max_loss*2*np.ones(2)
#     interpol_err = [1000, 1000]

#     interpol_pos = [0, res - 1]

#     # this value should get smaller as time goes by
#     slide_win = min_peak_width

#     # ----------------------------------------------
#     # slide_win  is the largest peak we can detect, so we need wider windows for wider peaks. 
#     # we can choose a min window and max window to select the span of the peaks we want to detect
#     #    

#     # TODO: ADD A LOOP OVER THE MIN PEAK WIDTH NO MAX PEAK WIDTH

#     # find the points that minimizes the variance of the data minus spline interpolation
#     # scan all points, who cares

#     # indices that are in the background, if you are not in the background, then we are in a peak

#     # first position, minimum of the data, largest value
#     # are my initial pins for the interpolation

#     # what is this loop?
#     # this loop iterates over the position in the x axis

#     # TODO: decide a minim width of a peak to be detected

#     # step decreases numbers in tmp_interpol_pos; excludes possible mins; \
#     # consecutive min_pos problem persists
#     # step = 10
#     # position in which loss in minimized
#     min_pos = 0
#     while (min_pos + 1) - res < -1:

#         # -------------------- change this to start at curr_pos, reinitialize whenever we encounter a min
#         tmp_interpol_pos = list(range(min_pos + 1, min((min_pos + 1) + slide_win, res - 1)))

#         # debug
#         # plt.plot(x_data, y_data)
#         # plt.plot(x_data, yhat_inter)

#         # this are the positions that we are testing for this position of the sliding window

#         #  TODO change to min_err=background_model.max_loss
#         min_err = 1000

#         # gosh my indexing is awful
#         # pick on position for every window where you "pin" the interpolation
#         for k in range(len(tmp_interpol_pos)):

#             tmp_pos = np.concatenate((interpol_pos, [tmp_interpol_pos[k]]))

#             y_hat_tmp = background_model.interpolate(x, y, tmp_pos)

#             # weighted jumps:
#             # including log seems to be a bit better
#             tmp_err = background_model.get_loss_inter() / np.log((tmp_interpol_pos[k] - min_pos) + 0.5)

#             # update the minimum
#             if tmp_err < min_err:
#                 min_pos = tmp_interpol_pos[k]
#                 min_err = tmp_err

#         interpol_pos.append(min_pos)
#         interpol_err.append(min_err)

#         # sort points
#         sort_pos = np.argsort(interpol_pos)
#         interpol_pos = list(np.array(interpol_pos)[sort_pos.astype(int)])
#         interpol_err = list(np.array(interpol_err)[sort_pos.astype(int)])

#         # 

#     # finally fitting
#     y_hat = background_model.interpolate(x, y, interpol_pos)

#     # spline interpolation just to see the peak support better
#     # y_bsp = si.interp1d(x_data[interpol_pos], y_data[interpol_pos], kind='cubic')(x_data)
#     y_bsp = si.interp1d(x[interpol_pos], y[interpol_pos], kind='linear')(x)

#     # TODO: put in verbose mod
#     plt.figure("fitting positions + spline interpolation", figsize=(12, 10), dpi=80)
#     plt.title("fitting positions + spline interpolation")
#     plt.plot(x, y)
#     plt.plot(x, y_hat)
#     plt.plot(x_data, y_bsp,'.')
#     plt.legend(["Original Curve", "Y_hat", "Y_bsp"])
#     plt.show()

#     #   find when you are over the mean level

#     # choose the threshold that is the square root of the variance between the 
#     # original data and the spline interpolation
#     #

#     th = np.sqrt(np.var(y_data - y_bsp)) / 2
#     pos = np.array(np.where((y_data - y_bsp) < th)[0])

#     #   merge the points
#     # pos[n+1] - pos[n] - 1; first and last element excluded from second mapping to prevent out of bounds

#     # why -1?
#     diff_pos = pos[1:] - pos[:-1] - 1
#     jumps = np.where(diff_pos > 0)[0]

#     # if the final element is res, the add a jump an the end
#     if pos[-1] == res:
#         jumps = np.append(jumps, pos.shape[0] - 1)

#     # -------------------------------------------------------------------------
#     # create a list where you put the edges of the fluorescence as you understand them at this stage

#     # Note: I should check this indexing more carefully
#     final_lb = []
#     final_rb = []

#     if jumps.size == 0:
#         final_lb.append(pos[0])
#         final_rb.append(pos[-1])

#     else:
#         final_lb.append(pos[0])
#         final_rb.append(pos[jumps[0]])
#         k = 0

#         while k < jumps.shape[0] - 1:
#             final_lb.append(pos[jumps[k] + 1])
#             # go to the next gap
#             k = k + 1
#             final_rb.append(pos[jumps[k]])

#     # add the first and the last intervals
#     idx_lr = np.zeros([2, len(final_rb)])
#     idx_lr[0] = np.array(final_lb)
#     idx_lr[1] = np.array(final_rb)
#     idx_lr.astype(int)
#     idx_lr = idx_lr.T

#     # merge intervals
#     # remove the one intervals
#     # TODO 
#     # the inte
#     idx_lr = idx_lr[np.where(idx_lr[:, 1] - idx_lr[:, 0] > 2)[0], :]
#     # Looks like it does nothing here, as they are ordered already. unless something else was meant
#     merged = recursive_merge(idx_lr.tolist())
#     idx_lr = np.array(merged).astype(int)

#     plt.plot(x_data, y_data - y_hat)
#     plt.show()
#     y_base = (y_data - y_hat).copy()

#     # TODO
#     # fix this
#     plt.figure("background vs peaks")
#     plt.plot(x_data, y_data)
#     plt.plot(x_data, y_bsp)
#     plt.plot(x_data[pos], y_bsp[pos], '*')
#     plt.legend(["Original Curve", "y_bsp", "y_bsp[pos]"])
#     plt.show()
#     # undid copy with missing parts

#     # at this point we can maybe return values

#     # --------------------------------------------------------------------------
#     # NOW FIT THE PEAKS IN BETWEEN THE INTERVALS 
#     # complement the positions, fit the peaks next 
#     # --------------------------------------------------------------------------

#     # TODO clean up here, there must be a more efficient structure

#     # first obtain the complement of the intervals

#     if idx_lr.size == 0:
#         # idx_lr_comp=np.array(range(i,i+int(slide_win)-1))
#         idx_lr_comp = np.array([0, slide_win - 1])
#         idx_lr_comp = idx_lr_comp.reshape([1, 2])
#     else:
#         idx_lr_comp = []

#         # include the first interval?
#         if idx_lr[0, 0] > 0:
#             idx_lr_comp.append([0, idx_lr[0, 0]])

#         for k in range(1, idx_lr.shape[0]):
#             idx_lr_comp.append([idx_lr[k - 1, 1], idx_lr[k, 0]])

#         # include the last interval
#         if idx_lr[-1, 1] < int(res) - 1:
#             idx_lr_comp.append([idx_lr[-1, 1], res])

#     idx_lr_comp = np.array(idx_lr_comp).reshape(int(np.size(idx_lr_comp) / 2), 2)
#     peak_importance_fun = Importance.area_ratio
#     # remove the intervals that are shorter than the number of parameters, 
#     for k in range(idx_lr_comp.shape[0]):
#         # expand the fitting window
#         # idx_lr_comp[k] = find_mul_peak_sym_supp(idx_lr_comp[k], y_data, mean_level, peak_tol)
#         # idx_lr_comp[k] = find_peak_supp(idx_lr_comp[k], y_data, mean_level, peak_tol)
#         # idx_lr_comp[k] = find_peak_supp(idx_lr_comp[k], y_base, areas_ratio, 0.4)
#         print(find_peak_supp(idx_lr_comp[k], y_base, Importance.areas_ratio, 0.7))

#     idx_lr_comp = np.array(recursive_merge(list(idx_lr_comp)))

#     # remove below min peak width

#     idx_lr_comp = np.array(idx_lr_comp[np.where(idx_lr_comp[:, 1] - idx_lr_comp[:, 0] > min_peak_width)[0], :])
#     idx_lr_comp.astype(int)

#     while False:
#         peak_pos = []

#         for i in range(len(idx_lr_comp)):
#             peak_pos = peak_pos + list(range(idx_lr_comp[i][0], idx_lr_comp[i][1]))

#         plt.plot(x_data, y_data)
#         plt.plot(x_data[peak_pos], y_bsp[peak_pos], '*')
#         plt.show()

#     # this variable is updated at each threshold window
#     mse_comp = []
#     range_comp = []
#     beta_comp = []

#     # fit the peaks recursively
#     idx_lr_comp = list(idx_lr_comp)

#     # idx_lr_comp_bck=idx_lr_comp.copy()

#     # --------------------------------------------------------------------------
#     # JUNE 19th edits
#     # 
#     # --------------------------------------------------------------------------

#     # needs to be a property in the peak object
#     peak_importance_fun = Importance.sharpness

#     # while len(idx_lr_comp)>0:
#     for i in range(len(idx_lr_comp)):

#         # TODO 
#         # no need to pop?

#         idx_lr_tmp = idx_lr_comp[i]

#         print('Now processing the range', idx_lr_tmp[0], '--', idx_lr_tmp[1])

#         # peak_tol must be set dynamically, based on the value of the max, but how?        
#         # idx_comp_lr=find_mul_peak_sym_supp(idx_lr_tmp,y_data,mean_level,peak_tol)

#         # TODO
#         # put in input the peak model which has the peak importance as a function
#         # find_peak_supp(int_idx, y_data peak_importance_fun, discount=0.85):
#         peak_lr_win = find_peak_supp(idx_lr_tmp, y_data, peak_importance_fun)

#         idx_comp_lr = idx_lr_tmp
#         # idx_comp = np.array(range(idx_comp_lr[0], idx_comp_lr[1]))
#         idx_comp = np.arange(idx_lr_comp[0], idx_lr_comp[1])

#         x_rec = x_data[idx_comp]
#         y_rec = y_data[idx_comp]
#         mean_rec = mean_level[idx_comp]

#         y_up_rec = y_rec - mean_rec

#         peak_pos = idx_comp_lr[0] + np.argmax(y_up_rec)

#         # peak_lr_win = find_peak_supp(peak_pos, int(idx_comp_lr[0]), int(idx_comp_lr[1]), y_data, peak_tol)
#         peak_lr_win = find_peak_supp(peak_pos, y_data[int(idx_comp_lr[0]): int(idx_comp_lr[1])], peak_importance_fun)

#         while False:
#             # check peaks
#             plt.plot(x_data, y_data)
#             plt.plot(x_rec, y_rec, '-*')
#             plt.plot(x_rec, mean_rec)
#             plt.plot(x_data[range(peak_lr_win[0], peak_lr_win[1])], y_data[range(peak_lr_win[0], peak_lr_win[1])], '*')

#         y_up_rec = y_rec - mean_rec

#         # fitting the peak        
#         y_data_temp = y_data[np.array(range(peak_lr_win[0], peak_lr_win[1]))]
#         x_data_temp = x_data[np.array(range(peak_lr_win[0], peak_lr_win[1]))]
#         mean_level_temp = mean_level[np.array(range(peak_lr_win[0], peak_lr_win[1]))]

#         # from ste_model_spectrum_v5 import *        
#         # init_fit_fun = init_pseudo_voig
#         # loss_peak = mse_loss
#         # fit_fun = pseudo_voig

#         beta_init = init_fit_fun(x_data_temp, y_data_temp - mean_level_temp)

#         result = minimize(loss_peak, beta_init, args=(x_data_temp, y_data_temp - mean_level_temp, fit_fun), tol=1e-12)
#         beta_hat_fit = result.x
#         mse_fit = result.fun

#         if False:
#             plt.plot(x_data_temp, y_data_temp)
#             plt.plot(x_data_temp, mean_level_temp)
#             # plt.plot(x_data_temp,fit_fun(x_data_temp,beta_init)+mean_level_temp)
#             plt.plot(x_data_temp, fit_fun(x_data_temp, beta_hat_fit) + mean_level_temp, '*-')

#         # now add head and tail to the list

#         # include the first interval?
#         if peak_lr_win[0] - idx_comp_lr[0] > min_peak_width and peak_lr_win[0] < idx_comp_lr[1]:
#             idx_lr_comp.append(np.array([idx_comp_lr[0], peak_lr_win[0] - 1]))
#         # includ the last interval?
#         if idx_comp_lr[1] - peak_lr_win[1] > min_peak_width and peak_lr_win[1] > idx_comp_lr[0]:
#             idx_lr_comp.append(np.array([peak_lr_win[1] + 1, idx_comp_lr[1]]))

#         # # include the first interval?
#         # if  peak_lr_win[0]-idx_lr_tmp[0]>min_peak_width and peak_lr_win[0]<idx_lr_tmp[1]:
#         #     idx_lr_comp.append(np.array([idx_lr_tmp[0],peak_lr_win[0]-1]))
#         # # includ the last interval?
#         # if idx_lr_tmp[1]-peak_lr_win[1]>min_peak_width and peak_lr_win[1]>idx_lr_tmp[0]:
#         #     idx_lr_comp.append(np.array([peak_lr_win[1]+1,idx_lr_tmp[1]]))

#         # ------------------------------------------
#         # update lists
#         if len(range_comp) == 0:
#             mse_comp.append(result.fun)
#             # range_up.append(peak_l_r_win)
#             range_comp.append(np.array(peak_lr_win))
#             beta_comp.append(np.array(beta_hat_fit))
#         elif not ((range_comp[-1] == peak_lr_win).all()):
#             mse_comp.append(result.fun)
#             # range_up.append(peak_l_r_win)
#             range_comp.append(np.array(peak_lr_win))
#             beta_comp.append(np.array(beta_hat_fit))

#     # ------------------------------------------
#     # storing bit

#     if len(range_comp) == 0:
#         print('Storing Th', j, ': No peak detected')
#         fitting_data[j] = dict(idx_lr_poly=idx_lr_poly, mean_level=mean_level, range_peak=0)
#     else:
#         print('Storing Th', j, ': ', len(range_comp), ' peaks detected')
#         fitting_data[j] = dict(idx_lr_poly=idx_lr_poly, mean_level=mean_level, mse_peak=mse_comp, range_peak=range_comp,
#                                beta_peak=beta_comp)

#     # -------------------------------------------
#     # recap plot
#     # title="iteration %i" % num_th
#     plt.figure("iteration %i" % j)
#     plt.plot(x_data, y_data)
#     plt.plot(x_data, mean_level)
#     for i in range(len(range_comp)):
#         x_data_temp = x_data[range_comp[i][0]:range_comp[i][1]]
#         mean_level_temp = mean_level[range_comp[i][0]:range_comp[i][1]]
#         plt.plot(x_data_temp, fit_fun(x_data_temp, beta_comp[i]) + mean_level_temp, 'r--')

#     # -------------------------------------------
#     # update the poly fitting list

#     peak_pos = []

#     for i in range(len(range_comp)):
#         peak_pos = peak_pos + list(range(range_comp[i][0], range_comp[i][1]))

#     return y_hat

def spline_remove_est_fluorescence(x_data, y_data, background_model, peak_model, max_peak_width, min_peak_width=18):
    # obtain length of x_data
    res = x_data.shape[-1]

    # copy data, don't want to overwrite the original spectrum
    y = y_data.copy()
    x = x_data.copy()

    # initialize the background
    background_model.init(x, y)

    # TODO change to  background_model.max_loss*2*np.ones(2)
    interpol_err = [1000, 1000]

    interpol_pos = [0, res - 1]

    # this value should get smaller as time goes by
    slide_win = min_peak_width

    # ----------------------------------------------
    # slide_win  is the largest peak we can detect, so we need wider windows for wider peaks. 
    # we can choose a min window and max window to select the span of the peaks we want to detect
    #    

    # TODO: ADD A LOOP OVER THE MIN PEAK WIDTH NO MAX PEAK WIDTH

    # find the points that minimizes the variance of the data minus spline interpolation
    # scan all points, who cares

    # indices that are in the background, if you are not in the background, then we are in a peak

    # first position, minimum of the data, largest value
    # are my initial pins for the interpolation

    # what is this loop?
    # this loop iterates over the position in the x axis

    # TODO: decide a minim width of a peak to be detected

    # step decreases numbers in tmp_interpol_pos; excludes possible mins; \
    # consecutive min_pos problem persists
    # step = 10
    # position in which loss in minimized
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

        interpol_pos.append(min_pos)
        interpol_err.append(min_err)

        # sort points
        sort_pos = np.argsort(interpol_pos)
        interpol_pos = list(np.array(interpol_pos)[sort_pos.astype(int)])
        interpol_err = list(np.array(interpol_err)[sort_pos.astype(int)])

            # 

    # finally fitting
    y_hat = background_model.interpolate(x, y, interpol_pos)

    # spline interpolation just to see the peak support better
    # y_bsp = si.interp1d(x_data[interpol_pos], y_data[interpol_pos], kind='cubic')(x_data)
    y_bsp = si.interp1d(x[interpol_pos], y[interpol_pos], kind='linear')(x)

    # TODO: put in verbose mod
    plt.figure("fitting positions + spline interpolation", figsize=(12, 10), dpi=80)
    plt.title("fitting positions + spline interpolation")
    plt.plot(x, y)
    plt.plot(x, y_hat)
    plt.plot(x_data, y_bsp,'.')
    plt.legend(["Original Curve", "Y_hat", "Y_bsp"])
    plt.show()

xls = pd.ExcelFile("/Users/anvarkunanbaev/PycharmProjects/SLA/ICASSP/xlsx/clean_spectrum.xlsx")

f_sup = np.array(pd.read_excel(xls, 'f_sup')).squeeze()
dict_data = ["dark", "laser", "quartz", "water", "gly", "leu", "phe", "trp"]
data = {i: np.array(pd.read_excel(xls, i)) for i in dict_data}
data_m = {
    i: np.mean(np.array(pd.read_excel(xls, i)), axis=0) for i in dict_data
}
g = data_m["gly"] - data_m["water"]

y_data = g

# normalize to 0/1
y_data =(y_data-np.min(y_data))/(np.max(y_data)-np.min(y_data))
x_data = f_sup
nu = 0.01
slide_win = 10
loss = Loss()
fit = Fit()

background_model = Background(fit.poly4, fit.init_poly4, lambda y_pred, y_true: loss.l2_relu_loss(y_pred, y_true, 0.0001))
peak_model = Peak(fit.pseudo_voig, fit.init_pseudo_voig, loss.peak_mse_loss)

spline_remove_est_fluorescence(f_sup,y_data, background_model, peak_model, max_peak_width = 25)