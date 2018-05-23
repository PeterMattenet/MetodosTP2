# -*- coding: utf-8 -*-

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import random
import math
import numpy as np


def exp50():
	df1 = pd.read_csv('niter50.csv')

	# Dens2 = df1[df1['densidad'] == 0.2]
	# Dens5 = df1[df1['densidad'] == 0.5]

	xdata  = df1['Autovector']

	ydata = df1['Error']
	plt.yscale('log')

	plt.plot(xdata, ydata, "c", alpha = 0.5, label='K = 5')

	plt.xlabel('Autovectores')
	plt.ylabel('Error')
	plt.title('Error relativo |Ax-Lx| para niter= 50 ')

	plt.legend()
	plt.show()


exp50()