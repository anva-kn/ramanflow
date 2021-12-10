import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
from importance_functions import Importance as im
from Synthetic_Signal import Synthetic_Signal
from scipy import signal

synthetic_sig = Synthetic_Signal()

synthetic_sig.generate_random_walk(1000, 1, mu=0.01 ,sigma=20, reverse=False, seed=42)

synthetic_sig.r_add_peak(fwhmG=None, fwhmL=None, amplitudeL=1)
synthetic_sig.smooth_data(20)

f_sup = np.arange(0, 1000, 1)
data = synthetic_sig.data[0]

asymmetry = im.asymmetry(f_sup[150:300], data[150:300])
print('b/a is: {}'.format(asymmetry))


window = signal.gaussian(51, std=7)
f_sup_win = np.arange(0, 51, 1)

peak = np.argmax(window)
results_tenth = signal.peak_widths(window, np.array([peak]), rel_height=0.9, prominence_data=None)
left_edge, right_edge = round(results_tenth[2][0]), round(results_tenth[3][0])
height = results_tenth[1][0]

asymmetry_win = im.asymmetry(f_sup_win, window)
print(asymmetry_win)
plt.plot(f_sup_win, window)
plt.plot(f_sup_win[left_edge], window[left_edge], 'r*')
plt.plot(f_sup_win[right_edge], window[right_edge], 'r*')
plt.show()
