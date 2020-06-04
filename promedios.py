#!/usr/bin/env python3
import pandas as pd

tams = [100, 500, 1000]

empty_df = pd.read_csv('mediciones_lartu/empty.csv')

for tam in tams:
    # Pongo en 0 el df vacio que lleva la suma
    empty_df['time'].values[:] = 0

    for n in range(0, 100):
        filename = f'30_test_{tam}_{n}_testResults.csv'
        df = pd.read_csv(f'mediciones_lartu/{filename}')
        empty_df['time'] += df['time']

    # Cuando terminamos de 0 a 100, calculo el promedio
    empty_df['time'] /= 100
    print(f'Tamaño: {tam}')
    print(empty_df)
    df.to_csv(f'mediciones_lartu/avg_{tam}.csv')
