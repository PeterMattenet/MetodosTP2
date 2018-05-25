# -*- coding: utf-8 -*-

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import random
import math
import numpy as np


def exp50():
	df100 = pd.read_csv('niter100.csv')

	df300 = pd.read_csv('niter300.csv')


	df500 = pd.read_csv('niter500.csv')


	df700 = pd.read_csv('niter700.csv')

	# Dens2 = df1[df1['densidad'] == 0.2]
	# Dens5 = df1[df1['densidad'] == 0.5]

	xdata100 = df100['Autovector']
	ydata100 = df100['Error']
	
	xdata300 = df300['Autovector']
	ydata300 = df300['Error']
	
	xdata500 = df500['Autovector']
	ydata500 = df500['Error']
	
	xdata700 = df700['Autovector']
	ydata700 = df700['Error']
	

	plt.plot(xdata100, ydata100, "c", alpha = 0.5, label='niter = 100')
	plt.plot(xdata300, ydata300, "r", alpha = 0.5, label='niter = 300')
	plt.plot(xdata500, ydata500, "g", alpha = 0.5, label='niter = 500')
	plt.plot(xdata700, ydata700, "b", alpha = 0.5, label='niter = 700')


	plt.xlabel('Autovectores')
	plt.ylabel('Error')
	plt.yscale('log')
	plt.title('Error relativo |Ax-Lx| segun niter ')

	plt.legend()
	plt.show()


exp50()