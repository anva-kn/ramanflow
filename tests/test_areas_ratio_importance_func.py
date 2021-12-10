import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from importance_functions import Importance

indices_1 = [155, 495]
indices_2 = [504, 639]
indices_3 = [990, 1023]

xls = pd.ExcelFile("/Users/anvarkunanbaev/PycharmProjects/SLA/ICASSP/xlsx/clean_spectrum.xlsx")

f_sup = np.array(pd.read_excel(xls, 'f_sup')).squeeze()
dict_data = xls.sheet_names
data = {}
for i in dict_data:
    data[i] = np.array(pd.read_excel(xls, i))
data_m = {}

for i in dict_data:
    data_m[i] = np.mean(np.array(pd.read_excel(xls, i)), axis=0)
g = data_m["gly"] - data_m["water"]

y_data = g

# normalize to 0/1
y_rec = (y_data - np.min(y_data)) / (np.max(y_data) - np.min(y_data))
x_rec = f_sup

x_rec_2 = np.array([1252.92123373, 1254.60927397, 1256.29715042, 1257.98486309,
                    1259.67241198, 1261.35979709, 1263.04701842, 1264.73407597,
                    1266.42096974, 1268.0947255, 1269.78129297, 1271.46769665,
                    1273.15393655, 1274.82704348, 1276.51295708, 1278.18574021,
                    1279.87132751, 1281.54378685, 1283.22904785, 1284.9011834,
                    1286.58611809, 1288.25792984, 1289.92958033, 1291.6140262,
                    1293.28535289, 1294.95651831, 1296.62752247, 1298.29836535,
                    1299.96904696, 1301.63956729, 1303.30992636, 1304.98012415,
                    1306.65016068, 1308.32003593, 1309.98974991, 1311.65930262,
                    1313.31575366, 1314.98498508, 1316.65405523, 1318.31002744,
                    1319.9787763, 1321.64736388, 1323.30285728, 1324.97112357,
                    1326.62629816, 1328.28131397, 1329.94909895, 1331.60379596,
                    1333.25833419, 1334.92563785, 1336.57985728, 1338.23391793,
                    1339.8878198, 1341.54156288, 1343.19514719, 1344.84857271,
                    1346.50183946, 1348.15494742, 1349.80789661, 1351.46068701,
                    1353.11331863, 1354.75288214, 1356.40519744, 1358.05735397,
                    1359.69644609, 1361.34828629, 1362.99996771, 1364.63858844,
                    1366.28995354, 1367.92826042, 1369.566411, 1371.21730224,
                    1372.85513896, 1374.49281938, 1376.13034348, 1377.78060332,
                    1379.41781358, 1381.05486752, 1382.69176516, 1384.32850649,
                    1385.96509151, 1387.60152022, 1389.23779262, 1390.87390871,
                    1392.49698753, 1394.13279223, 1395.76844063, 1397.40393271,
                    1399.02639244, 1400.66157313, 1402.28372392, 1403.91859323,
                    1405.54043509, 1407.17499301, 1408.79652593, 1410.43077246,
                    1412.05199644, 1413.67306656, 1415.29398283, 1416.92760782,
                    1418.54821515, 1420.16866862, 1421.78896824, 1423.409114,
                    1425.0291059, 1426.64894394, 1428.26862813, 1429.88815846,
                    1431.49468334, 1433.11390717, 1434.73297715, 1436.33904533,
                    1437.95780882, 1439.57641844, 1441.18202993, 1442.80033306,
                    1444.40564049, 1446.02363712, 1447.62864049, 1449.23349243,
                    1450.85102993, 1452.45557781, 1454.05997426, 1455.66421929,
                    1457.26831289, 1458.87225507, 1460.47604582, 1462.07968515,
                    1463.68317305, 1465.28650953, 1466.88969458, 1468.49272821,
                    1470.08278796, 1471.68551995, 1473.28810051])

y_rec_2 = np.array([0.10377512, 0.10771494, 0.11789459, 0.12526682, 0.12841472,
                    0.12883452, 0.12991211, 0.14514542, 0.15023021, 0.14903793,
                    0.1546673, 0.15907051, 0.1616733, 0.16284823, 0.15796715,
                    0.15662769, 0.15048872, 0.14718762, 0.1410988, 0.14291417,
                    0.14789316, 0.14630034, 0.14264004, 0.14463333, 0.14263516,
                    0.1446257, 0.13903028, 0.13813521, 0.14174163, 0.14303548,
                    0.14559435, 0.15081682, 0.16108624, 0.17565631, 0.18075724,
                    0.19567117, 0.201949, 0.21755365, 0.22531787, 0.23932074,
                    0.25860138, 0.28398761, 0.29959994, 0.32341711, 0.34985283,
                    0.37731352, 0.41125978, 0.44600625, 0.47277743, 0.50055923,
                    0.52547782, 0.54233561, 0.55261269, 0.55306451, 0.53362448,
                    0.50694316, 0.46639439, 0.42019417, 0.36837111, 0.31781941,
                    0.27220833, 0.23860362, 0.21422664, 0.19173495, 0.17002051,
                    0.158443, 0.15017557, 0.14338025, 0.14168052, 0.14163987,
                    0.13176795, 0.12700988, 0.12573958, 0.12679932, 0.13202436,
                    0.12527942, 0.11510978, 0.11439527, 0.11653196, 0.12005372,
                    0.12172731, 0.1251235, 0.14370803, 0.13979302, 0.14946565,
                    0.15328828, 0.15772094, 0.16693048, 0.17928383, 0.19497512,
                    0.2135397, 0.2331026, 0.25561804, 0.28126716, 0.30884806,
                    0.34057773, 0.36186322, 0.38654957, 0.42067048, 0.44608269,
                    0.4710598, 0.49080732, 0.50386752, 0.51338143, 0.5092672,
                    0.49357912, 0.46851101, 0.4422309, 0.41990078, 0.40318257,
                    0.37462176, 0.36241264, 0.36442691, 0.37097869, 0.38604953,
                    0.4092933, 0.43252983, 0.45269765, 0.45975296, 0.46102079,
                    0.45862878, 0.44765749, 0.42821682, 0.40300349, 0.36686361,
                    0.32303737, 0.27976334, 0.24660988, 0.21411711, 0.19128439,
                    0.18026192, 0.1698267, 0.15031181, 0.13672749, 0.12939672])

peak_importance_fun = Importance.areas_ratio


def find_peak_supp(int_idx, y_data, peak_importance_fun, discount=0.5):
    """This function find the support for peaks

    Parameters
    ----------
    int_idx -> left and right edges
    y_data -> spectral data
    peak_importance_fun -> spectral data
    discount -> how generous we are in getting the peaks to continue

    Returns
    -------

    """
    # SR: debug

    # int_idx=idx_lr_tmp

    # define the steepness an the mse of the diff of the

    # expand left and right until the steepness in
    res = len(y_data)
    # TODO: checking for the boundaries "should" be unnecessary
    l_pos_temp = int(np.max([int_idx[0], 0]))
    r_pos_temp = int(np.min([int_idx[1], res - 1]))

    # extend the peak until the decrease from the max importance you have seen in the window is discount*max
    #
    max_impo = peak_importance_fun(y_data[range(int_idx[0], int_idx[1])])

    # expand the interval until is decreasing by a factor discount, discount account for the randomness in the error
    # the discount starts from the median of the vector

    expand_left = True
    expand_right = True

    while expand_left == True and expand_right == True:

        # check boundaries

        if l_pos_temp == 0:
            expand_left = False

        if r_pos_temp == res - 1:
            expand_right = False

        if expand_left == True:
            impo_left = peak_importance_fun(y_data[range(l_pos_temp - 1, r_pos_temp)])

            if impo_left < discount * max_impo:
                expand_left = False
            else:
                l_pos_temp = l_pos_temp - 1

        if expand_right == True:
            impo_right = peak_importance_fun(y_data[range(l_pos_temp, r_pos_temp + 1)])

            if impo_right < discount * max_impo:
                expand_left = False
            else:
                r_pos_temp = r_pos_temp + 1

        # update max importance
        # TODO we can do this better?
        tmp_impo = peak_importance_fun(y_data[range(l_pos_temp, r_pos_temp)])

        if tmp_impo > max_impo:
            max_impo = tmp_impo

    return [l_pos_temp, r_pos_temp]


peak_support_indices_1 = find_peak_supp(indices_1, y_rec, peak_importance_fun, 0.9)

peak_support_indices_2 = find_peak_supp(indices_2, y_rec, peak_importance_fun, 0.9)

peak_support_indices_3 = find_peak_supp(indices_3, y_rec, peak_importance_fun, 0.9)

peak_support_indices_4 = find_peak_supp([30, 70], y_rec_2, peak_importance_fun, 0.9)

peak_support_1 = np.arange(peak_support_indices_1[0], peak_support_indices_1[1])

peak_support_2 = np.arange(peak_support_indices_2[0], peak_support_indices_2[1])

peak_support_3 = np.arange(peak_support_indices_3[0], peak_support_indices_3[1])

peak_support_4 = np.arange(peak_support_indices_4[0], peak_support_indices_4[1])

plt.figure("testing areas ratio importance function 1")
plt.plot(x_rec, y_rec)
plt.plot(x_rec[peak_support_1], y_rec[peak_support_1], 'r-')
plt.vlines(x_rec[indices_1[0]], ymin=0, ymax=1)
plt.vlines(x_rec[indices_1[1]], ymin=0, ymax=1)
plt.show()

plt.figure("testing areas ratio importance function 2")
plt.plot(x_rec, y_rec)
plt.plot(x_rec[peak_support_2], y_rec[peak_support_2], 'r-')
plt.vlines(x_rec[indices_2[0]], ymin=0, ymax=1)
plt.vlines(x_rec[indices_2[1]], ymin=0, ymax=1)
plt.show()

plt.figure("testing areas ratio importance function 3")
plt.plot(x_rec, y_rec)
plt.plot(x_rec[peak_support_3], y_rec[peak_support_3], 'r-')
plt.vlines(x_rec[indices_3[0]], ymin=0, ymax=1)
plt.vlines(x_rec[indices_3[1]], ymin=0, ymax=1)
plt.show()

plt.figure("testing areas ratio importance function 4")
plt.plot(x_rec_2, y_rec_2)
plt.plot(x_rec_2[peak_support_4], y_rec_2[peak_support_4], 'r-')
plt.vlines(x_rec_2[30], ymin=0, ymax=1)
plt.vlines(x_rec_2[70], ymin=0, ymax=1)
plt.show()
