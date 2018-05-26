# -*- coding: utf-8 -*-

import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import random
import math
import numpy as np

def expKnnAccurReduced():
	df1 = pd.read_csv('accurVariandoKSinPCAK2Reduced.csv')
	df2 = pd.read_csv('accurVariandoKSinPCAK5Reduced.csv')
	df3 = pd.read_csv('accurVariandoKSinPCAK10Reduced.csv')

	# Dens2 = df1[df1['densidad'] == 0.2]
	# Dens5 = df1[df1['densidad'] == 0.5]

	xdata  = df1['k']

	ydataK2 = df1['accuracyProm'] * 100
	ydataK5 = df2['accuracyProm'] * 100
	ydataK10 = df3['accuracyProm'] * 100

	plt.plot(xdata, ydataK2, "r.", alpha = 0.5, label='K = 2 (KFold)')
	plt.plot(xdata, ydataK5, "g.", alpha = 0.5, label='K = 5 (KFold)')
	plt.plot(xdata, ydataK10, "b.", alpha = 0.5, label='K = 10 (KFold)')

	plt.xlabel('k (kNN)')
	plt.ylabel('% Accuracy promedio')
	plt.title('Imagenes reducidas')

	plt.legend()
	plt.show()

def expKnnAccurK5():
	df1 = pd.read_csv('accurVariandoKSinPCAK5.csv')

	# Dens2 = df1[df1['densidad'] == 0.2]
	# Dens5 = df1[df1['densidad'] == 0.5]

	xdata  = df1['k']

	ydata = df1['accuracyProm'] * 100

	plt.plot(xdata, ydata, "c", alpha = 0.5, label='K = 5')

	plt.xlabel('k')
	plt.ylabel('% Accuracy promedio')
	plt.title('Imagenes sin reducir')

	plt.legend()
	plt.show()

def exp1vs2():
	df1 = pd.read_csv('accurVariandoKSinPCAK5.csv')
	df2 = pd.read_csv('accurVariandoKSinPCAK5Reduced.csv')

	xdata  = df1['k']

	ydata1 = df1['accuracyProm'] * 100
	ydata2 = df2['accuracyProm'] * 100

	plt.plot(xdata, ydata1, "c", alpha = 0.5, label='Originales')
	plt.plot(xdata, ydata2, "k", alpha = 0.5, label='Reducidas')

	plt.xlabel('k')
	plt.ylabel('% Accuracy promedio')
	plt.title('Reducidas vs Originales K = 5')

	plt.legend()
	plt.show()	

def expKnnTimeReduced():
	df1 = pd.read_csv('accurVariandoKSinPCAK2Reduced.csv')
	df2 = pd.read_csv('accurVariandoKSinPCAK5Reduced.csv')
	df3 = pd.read_csv('accurVariandoKSinPCAK10Reduced.csv')

	# Dens2 = df1[df1['densidad'] == 0.2]
	# Dens5 = df1[df1['densidad'] == 0.5]

	xdata  = df1['k']

	ydataK2 = df1['Tiempo']
	ydataK5 = df2['Tiempo']
	ydataK10 = df3['Tiempo']

	plt.plot(xdata, ydataK2, "r", alpha = 0.5, label='K = 2 (KFold)')
	plt.plot(xdata, ydataK5, "g", alpha = 0.5, label='K = 5 (KFold)')
	plt.plot(xdata, ydataK10, "b", alpha = 0.5, label='K = 10 (KFold)')

	plt.xlabel('k (kNN)')
	plt.ylabel('Tiempo en milisegundos')
	plt.title('Imagenes reducidas')

	plt.legend()
	plt.show()

def expKnnTimeK5():
	df1 = pd.read_csv('accurVariandoKSinPCAK5.csv')

	# Dens2 = df1[df1['densidad'] == 0.2]
	# Dens5 = df1[df1['densidad'] == 0.5]

	xdata  = df1['k']

	ydata = df1['Tiempo']

	plt.plot(xdata, ydata, "c", alpha = 0.5, label='K = 5')
	plt.plot(50, 1400, "r", alpha=0.0)
	plt.plot(50, 800, "r", alpha=0.0)

	plt.xlabel('k')
	plt.ylabel('Tiempo en milisegundos')
	plt.title('Imagenes sin reducir')

	plt.legend()
	plt.show()	

def expFinalK2Accuracy():
	df1 = pd.read_csv('expFinalK2.csv')

	g = sns.barplot(x='algoritmo', y='accuracy', data=df1)

	x1,x2,y1,y2 = plt.axis()

	for index, row in df1.iterrows():
		g.text(row.name, row.accuracy, row.accuracy, color='black', ha="center")

	plt.ylabel('Accuracy promedio (0 a 1)')
	plt.title('K = 2, accuracy')

	plt.legend()

	plt.show()

def expFinalK2Tiempo():
	df1 = pd.read_csv('expFinalK2.csv')

	g = sns.barplot(x='algoritmo', y='tiempo', data=df1)

	x1,x2,y1,y2 = plt.axis()

	for index, row in df1.iterrows():
		g.text(row.name, row.tiempo, row.tiempo, color='black', ha="center")

	plt.ylabel('Tiempo en segundos')
	plt.title('K = 2, tiempo')

	plt.legend()

	plt.show()

def expFinalK5Accuracy():
	df1 = pd.read_csv('expFinalK5.csv')

	g = sns.barplot(x='algoritmo', y='accuracy', data=df1)

	x1,x2,y1,y2 = plt.axis()

	for index, row in df1.iterrows():
		g.text(row.name, row.accuracy, row.accuracy, color='black', ha="center")

	plt.ylabel('Accuracy promedio (0 a 1)')
	plt.title('K = 5, accuracy')

	plt.legend()

	plt.show()

def expFinalK5Tiempo():
	df1 = pd.read_csv('expFinalK5.csv')

	g = sns.barplot(x='algoritmo', y='tiempo', data=df1)

	x1,x2,y1,y2 = plt.axis()

	for index, row in df1.iterrows():
		g.text(row.name, row.tiempo, row.tiempo, color='black', ha="center")

	plt.ylabel('Tiempo en segundos')
	plt.title('K = 5, tiempo')

	plt.legend()

	plt.show()

# expKnnAccurReduced()
# expKnnAccurK5()
# exp1vs2()
# expKnnTimeReduced()
# expKnnTimeK5()
expFinalK5Accuracy()
expFinalK5Tiempo()