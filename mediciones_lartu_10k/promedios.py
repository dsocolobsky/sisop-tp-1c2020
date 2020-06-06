#!/usr/bin/env python3
import pandas as pd

tams = [100, 500, 1000, 5000, 10000, 25000]

empty_df = pd.read_csv('empty.csv')

for tam in tams:
    # Pongo en 0 el df vacio que lleva la suma
    empty_df['time'].values[:] = 0

    for n in range(0, 5):
        df = pd.read_csv(f'30_test_{tam}_{n}_testResults.csv')
        empty_df['time'] += df['time']

    # Promedio de las 5 mediciones
    empty_df['time'] /= 5
    print(f'Tamaño: {tam}')
    print(empty_df)
    df.to_csv(f'avg_{tam}.csv', index=False)
