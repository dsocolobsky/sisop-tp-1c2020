#!/usr/bin/env python3
import numpy
import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.ticker as ticker

files = ['mediciones_files/dylan_50f_50t_res_files.csv', 'mediciones_files/lartu_20f_20t_res_files.csv']

df_dy = pd.read_csv('mediciones_files/dylan_50f_50t_res_files.csv')
plt.figure()
plt.xticks(range(0, 50))
plt.scatter(df_dy['threads_files'], df_dy['time'], c='g', alpha=0.5)
plt.savefig('mediciones_files/dylan_50f50t.png')

df_lartu = pd.read_csv('mediciones_files/lartu_20f_20t_res_files.csv')
plt.figure()
plt.xticks(range(0, 20))
plt.scatter(df_lartu['threads_files'], df_lartu['time'], c='g', alpha=0.5)
plt.savefig('mediciones_files/lartu_20f20t.png')
