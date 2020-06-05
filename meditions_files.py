#!/usr/bin/env python3
import random
import sys
import subprocess
from datetime import datetime
import argparse
import os


graficosError = False
try:
    import numpy
    import matplotlib.pyplot as plt
    import pandas as pd
    import matplotlib.ticker as ticker
except ImportError:
    print('Not using graphics')
    graficosError = True

def mean(l):
    return sum(l)/len(l)

def run_cmd(cmd):
    pipe = subprocess.Popen(cmd,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    pipe.wait()
    out, _ = pipe.communicate()
    result = out.decode()
    return result

def run():
    filename = f'{folder}_files.csv'

    # Test run
    cmd = f'./build/ContarPalabras 1 1 {tests_s}'
    print(f'Voy a correr {cmd}')
    testres, _, testtime = run_cmd(cmd).split(',')
    print(f'Test run: {testres}, {testtime}')

    with open(filename, 'w') as f:
        f.write('threads_files,time\n')
        for t in range(1,args.files+1):
            print(f'Testing {t} threads for files')
            cmd = f'./build/ContarPalabras {t} 1 {tests_s}'
            print(args.testfile)
            times = []
            for sample in range(args.samples):
                cmd_result = run_cmd(cmd)
                print(cmd_result)
                try:
                    res, _, time = cmd_result.split(',')
                except BaseException as e:
                    print(f"{e}")
                    continue

                if res == testres:
                    sys.stdout.write('\x1b[1;32m OK \x1b[0m')
                else:
                    sys.stderr.write('\x1b[1;31mFAIL\x1b[0m')

                print('sample {} in {}'.format(sample, time))
                times.append(float(time))

            m = round(mean(times), args.samples)
            print(f'mean: {m}')
            f.write(f'{t},{m}\n')
    
    print('DONE: Resultados en {}'.format(folder))
    return filename



def graficar(filename):
    print(f'Cargando csv {filename}')
    df = pd.read_csv(filename)
    plt.figure()
    plt.xticks(range(0, args.files+1))
    plt.scatter(df['threads_files'], df['time'], c='g', alpha=0.5)
    plt.savefig(f'{filename}.png')


parser = argparse.ArgumentParser(description='Genera casos de tests, testea y hace mediciones del TP')

parser.add_argument('-f', '--files', type=int, help='Threads para buscar files', default=1, nargs='?')
parser.add_argument('-s', '--samples', type=int, default=2, help='Numero de muestras que tomar para cada testeo (default 2)')
parser.add_argument('-o', '--folder', help='Nombre de la carpeta de la medicion SIN ESPACIOS (por default es la hora)')
parser.add_argument('-t', '--testfile', help='Test files, al menos una', nargs='+')
parser.add_argument('-g', '--graficos', action='store_true', help='Escupe graficos en matplotlib')

args = parser.parse_args()

if not args.testfile:
    print('Te falta el testfile')
    sys.exit(1)

if not os.path.exists('mediciones'):
    os.mkdir('mediciones')

if args.folder is None:
    fname = args.testfile[0].split("/")[1]
    folder = f'mediciones/files_{len(args.testfile)}f_{args.files}t_res'
else:
    folder = f'mediciones/{args.folder}'


tests_s = ''
for t in args.testfile:
    tests_s += (t + ' ')

csvs = []
filename = run()

if args.graficos and (not graficosError):
    graficar(filename)
