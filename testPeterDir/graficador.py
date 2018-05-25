import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
# sphinx_gallery_thumbnail_number = 2

def plotConfusion():

	#Aca el archvio que quieran plotuear pongan, que tenga tres columnas nada mas.
	r = pd.read_csv("matrizConfusionBrutosK10alfa65knn5.csv")


	#aca un renombre de las columnas para que se muestren las lleyendas.
	colums = pd.DataFrame({"Imagen Obtenida" : r['etiquetaObtenida'], "Imagen Esperada": r['etiquetaEsperada'], "cantidad": r['cantidad']})
	combinaciones = colums.groupby(["Imagen Obtenida","Imagen Esperada"]).mean()
	combinaciones = combinaciones.reset_index()


	plt.figure(figsize=(10,10))
	pivot_table = combinaciones.pivot("Imagen Obtenida","Imagen Esperada",'cantidad')

	plt.xlabel('Imagen Obtenida', size = 15)
	plt.ylabel('Imagen Esperada', size = 15)
	plt.title('Matriz de Confusion', size = 15)
	#a = plt.axes([.65, .6, .2, .2], facecolor='y')
	sns.heatmap(pivot_table, annot=False, fmt=".1f", linewidths=.5, square = True, cmap = 'viridis', yticklabels=5,xticklabels=5);
	plt.show()


def plotFiscore():
	#Aca el archvio que quieran plotuear pongan, que tenga tres columnas nada mas.
	r = pd.read_csv("f1ScoreAlfakK10.csv")


	#aca un renombre de las columnas para que se muestren las lleyendas.
	colums = pd.DataFrame({"k vecinos" : r['k'], "alfa autovectores": r['alfa'], "f1score": r['f1score']})
	combinaciones = colums.groupby(["k vecinos","alfa autovectores"]).mean()
	combinaciones = combinaciones.reset_index()


	plt.figure(figsize=(12,12))
	pivot_table = combinaciones.pivot("k vecinos","alfa autovectores",'f1score')

	plt.xlabel('K vecinos', size = 15)
	plt.ylabel('alfa autovectores', size = 15)
	plt.title('F1Score segun combinacion K - Alfa', size = 15)
	#a = plt.axes([.65, .6, .2, .2], facecolor='y')
	sns.heatmap(pivot_table, annot=False, fmt=".1f", linewidths=.5, square = True, cmap = 'summer_r', cbar_kws={'label': 'segundos'});
	plt.show()






plotConfusion()





#Accent, Accent_r, Blues, Blues_r, BrBG, BrBG_r, BuGn, BuGn_r, BuPu, BuPu_r, CMRmap, CMRmap_r, Dark2, Dark2_r, GnBu, GnBu_r, Greens, Greens_r, Greys, Greys_r, OrRd, OrRd_r, Oranges, Oranges_r, PRGn, PRGn_r, Paired, Paired_r, Pastel1, Pastel1_r, Pastel2, Pastel2_r, PiYG, PiYG_r, PuBu, PuBuGn, PuBuGn_r, PuBu_r, PuOr, PuOr_r, PuRd, PuRd_r, Purples, Purples_r, RdBu, RdBu_r, RdGy, RdGy_r, RdPu, RdPu_r, RdYlBu, RdYlBu_r, RdYlGn, RdYlGn_r, Reds, Reds_r, Set1, Set1_r, Set2, Set2_r, Set3, Set3_r, Spectral, Spectral_r, Wistia, Wistia_r, YlGn, YlGnBu, YlGnBu_r, YlGn_r, YlOrBr, YlOrBr_r, YlOrRd, YlOrRd_r, afmhot, afmhot_r, autumn, autumn_r, binary, binary_r, bone, bone_r, brg, brg_r, bwr, bwr_r, cividis, cividis_r, cool, cool_r, coolwarm, coolwarm_r, copper, copper_r, cubehelix, cubehelix_r, flag, flag_r, gist_earth, gist_earth_r, gist_gray, gist_gray_r, gist_heat, gist_heat_r, gist_ncar, gist_ncar_r, gist_rainbow, gist_rainbow_r, gist_stern, gist_stern_r, gist_yarg, gist_yarg_r, gnuplot, gnuplot2, gnuplot2_r, gnuplot_r, gray, gray_r, hot, hot_r, hsv, hsv_r, icefire, icefire_r, inferno, inferno_r, jet, jet_r, magma, magma_r, mako, mako_r, nipy_spectral, nipy_spectral_r, ocean, ocean_r, pink, pink_r, plasma, plasma_r, prism, prism_r, rainbow, rainbow_r, rocket, rocket_r, seismic, seismic_r, spring, spring_r, summer, summer_r, tab10, tab10_r, tab20, tab20_r, tab20b, tab20b_r, tab20c, tab20c_r, terrain, terrain_r, viridis, viridis_r, vlag, vlag_r, winter, winter_r
