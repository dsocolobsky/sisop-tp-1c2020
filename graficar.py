#!/usr/bin/env python3
import numpy
import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.ticker as ticker

tams = [100, 500, 1000]

for tam in tams:
    df = pd.read_csv(f'mediciones_lartu/avg_{tam}.csv')
    plt.figure()
    plt.xticks(range(0, 30))
    plt.scatter(df['threads_maximo'], df['time'], c='g', alpha=0.5)
    plt.savefig(f'mediciones_lartu/graf_{tam}.png')