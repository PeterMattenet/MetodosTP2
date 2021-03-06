#include <vector>
#include <algorithm>
#include "Image.cpp"
#include "matrix.h"
#include <math.h>
#include <algorithm>
#include <map>

using namespace std;

class ImagePredictor
{
public:

	vector<int> photoIds;
	vector<Image*> imageDataSet; //MATRIZ X (inicial)
	vector<string> labels;
	
	//(n x m )
	vector<vector<double> > basicImagePixelMatrix; // cada vector<double> es una imagen del archivo de entrada
	//(m x alfa)
	vector<vector<double> > basisChangeMatrix; 
	//(n x alfa)
	vector<vector<double> > pcaMatrix; //X CAMBIADO DE BASE.

	int currentAlfa;

	int n; //Cantidad de imagenes
	int m; //pixeles de una imagen en total
	int alfa;
  
  	// crea la basicImagePixelMatrix, que es la matriz X (inicial) en formato de vector<vector<double>>
	ImagePredictor(){
		
	}; 

	~ImagePredictor(){
		for (int i = 0; i < imageDataSet.size(); ++i)
		{
			delete(imageDataSet[i]);
		}
		cout << "Destruyendo predictor" << endl;
		
	};

	// llena el vector imageDataSet con todas las imágenes del archivo de entrada (cuyo nombre es el primer elemento de imageFileDataSet, el parámetro de entrada)
	void loadImagesFromFileDataSet(vector<string>& imageFileDataSet) {
		imageDataSet = vector<Image*>();
		m = 0; 

		n = imageFileDataSet.size();

		for (int i = 0; i < n; ++i)
		{
			Image* img = new Image(imageFileDataSet[i]);
			m = img->width * img->height; 
			imageDataSet.push_back(img);
			labels.push_back(img->label);
		}

		createImageMatrix();
	}

	//  
	
	//PRE: Asume que las imagenes ya fueron levantadas con loadImagesFromFileDataSet
	// y que ya fue creada la matriz de cambio de base (y PCA) con SVD
	//PARAMS: Imagen. K: vecinos. Alfa: columnas/parametros relevantes. Inter: Cantidad iteraciones para Metodo de la potencia.
	string clasificarImagen(string image, int k){
		//Levanto la imagen, y la pongo en un vector de doubles
		//Z = vector de (mx1)
		Image img = Image(image);
		vector<double> imagePixels;
		obtainImageInVectorDoubles(img,imagePixels);	

		//Ahora tengo que cambiar de base Z.
		//z1 = V * z  (alfa*1)
		vector<double> z1(alfa, 0.0);
		multiplicarVectorMatrizDouble(imagePixels, basisChangeMatrix, z1);

		//Finalmente, llamo a kNN con las z y X ya cambiados de base
		return resolverKnn(pcaMatrix, imageDataSet, z1, k);
	}

	// Asume que las imágenes fueron levantadas con loadImagesFromFileDataSet
	string clasificarImagenSinPca(string image, int k){
		//Levanto la imagen, y la pongo en un vector de doubles
		//Z = vector de (mx1)
		Image img = Image(image);
		vector<double> imagePixels;
		obtainImageInVectorDoubles(img,imagePixels);	

		//Finalmente, llamo a kNN con las z y X ya cambiados de base
		return resolverKnn(basicImagePixelMatrix, imageDataSet, imagePixels, k);
	}

	// usa Knn devolviendo la primer moda que se genera
	string clasificarImagenGrego(string image, int k){
		//Levanto la imagen, y la pongo en un vector de doubles
		//Z = vector de (mx1)
		Image img = Image(image);
		vector<double> imagePixels;
		obtainImageInVectorDoubles(img,imagePixels);	

		//Ahora tengo que cambiar de base Z.
		//z1 = V * z  (alfa*1)
		vector<double> z1(alfa, 0.0);
		multiplicarVectorMatrizDouble(imagePixels, basisChangeMatrix, z1);

		//Finalmente, llamo a kNN con las z y X ya cambiados de base
		return resolverKnnGrego(pcaMatrix, imageDataSet, z1, k);
	}

	// usa Knn devolviendo la primer moda que se genera
	string clasificarImagenSinPcaGrego(string image, int k){
		//Levanto la imagen, y la pongo en un vector de doubles
		//Z = vector de (mx1)
		Image img = Image(image);
		vector<double> imagePixels;
		obtainImageInVectorDoubles(img,imagePixels);	

		//Finalmente, llamo a kNN con las z y X ya cambiados de base
		return resolverKnnGrego(basicImagePixelMatrix, imageDataSet, imagePixels, k);
	}

	// usa K-puntos para clasificar
	string clasificarImagenPuntos(string image, int k){
		//Levanto la imagen, y la pongo en un vector de doubles
		//Z = vector de (mx1)
		Image img = Image(image);
		vector<double> imagePixels;
		obtainImageInVectorDoubles(img,imagePixels);	

		//Ahora tengo que cambiar de base Z.
		//z1 = V * z  (alfa*1)
		vector<double> z1(alfa, 0.0);
		multiplicarVectorMatrizDouble(imagePixels, basisChangeMatrix, z1);

		//Finalmente, llamo a kNN con las z y X ya cambiados de base
		return knnPorPuntos(pcaMatrix, imageDataSet, z1, k);
	}

	// usa K-puntos para clasificar
	string clasificarImagenSinPcaPuntos(string image, int k){
		//Levanto la imagen, y la pongo en un vector de doubles
		//Z = vector de (mx1)
		Image img = Image(image);
		vector<double> imagePixels;
		obtainImageInVectorDoubles(img,imagePixels);	

		//Finalmente, llamo a kNN con las z y X ya cambiados de base
		return knnPorPuntos(basicImagePixelMatrix, imageDataSet, imagePixels, k);
	}	

	void generateBasisChangeMatrixWithSVD(int alfa, int niter){
		currentAlfa = alfa;

		vector<double> vectorEsperanza(m, 0.0);
 		for (int i = 0; i < m; i++) {
			vectorEsperanza[i] = 0.0;
			for (int j = 0; j < n; j++) {
				vectorEsperanza[i] += basicImagePixelMatrix[j][i];
			}
			vectorEsperanza[i] = vectorEsperanza[i] / n;
			//En U[i] tengo el promedio del pixel i
		}

		
		//Creo la Matriz X donde X_i = (X_i - u ) / sqrt(n-1)
    	vector<vector<double> > matrizRestadoEsperanza(n, vector<double>(m, 0.0));
    	for (int i = 0 ;  i < m; i++){ //itero columnas
        	for (int j = 0; j < n; j++) { //itero filas
				matrizRestadoEsperanza[j][i] = (basicImagePixelMatrix[j][i] - vectorEsperanza[i]) / sqrt(n-1) ;
			}
    	}
    	//(X^t)
    	vector<vector<double> > matrizRestadoEsperanzaTraspuesta(m, vector<double>(n, 0.0));
		trasponerMatriz(matrizRestadoEsperanza, matrizRestadoEsperanzaTraspuesta);

		//Creo M matriz U de nxn. M = [ (X)*(X^t) ] 
		
		vector<vector<double> > matrizU(n, vector<double>(n, 0.0));
		multiplicarMatricesDouble(matrizRestadoEsperanza,matrizRestadoEsperanzaTraspuesta, matrizU );
		
		vector<vector<double> > matrizAutovectoresU(n, vector<double>(alfa, 0.0));
		vector<double> alfaAutovalores(alfa, 0.0);
    	obtenerAutovectoresDeflacion(matrizU, matrizAutovectoresU, alfaAutovalores,alfa, niter);

    	vector<vector<double> > matrizAutovectoresV(m, vector<double>(alfa, 0.0));
    	for (int i = 0; i < alfa; ++i)
    	{
    		vector<double> Ui(n, 0.0);
    		for (int j = 0; j < n; ++j)
    		{
    			Ui[j]= matrizAutovectoresU[j][i];
    		}

    		vector<double> Vi(m, 0.0);
    		multiplicarMatrizVectorDouble(matrizRestadoEsperanzaTraspuesta, Ui, Vi);
    		// mostrarVector(Vi);
    		for (int j = 0; j < m; ++j)
    		{
    			matrizAutovectoresV[j][i] = Vi[j] / sqrt(alfaAutovalores[i]);
    		}

    	}

    	basisChangeMatrix = matrizAutovectoresV;

    	// vamos a hacer testing. Muestro el primer autovector
    	/*
    	cout << "["; 
    	for(int i = 0; i < matrizAutovectoresV.size(); i++){
    		cout << matrizAutovectoresV[i][0];
    		cout << ",";
    		if(i == matrizAutovectoresV.size() - 1){
    			cout << "]" << endl;
    		}
    	}
    	*/

		//pca matrix. (n x alfa); cambio de base (m x alfa)
    	pcaMatrix = vector<vector<double> > (n, vector<double>(alfa, 0.0));
    	multiplicarMatricesDouble(basicImagePixelMatrix, basisChangeMatrix, pcaMatrix);

	}

	//CAMBIO DE BASE
   	void generateBasisChangeMatrix(int alfa, int niter) {
   		currentAlfa = alfa;
		// genero matriz X con pixeles en doubles
    	
    	// generar vector U de medianas

		vector<double> vectorEsperanza(m, 0.0);
 		for (int i = 0; i < m; i++) {
			vectorEsperanza[i] = 0.0;
			for (int j = 0; j < n; j++) {
				vectorEsperanza[i] += basicImagePixelMatrix[j][i];
			}
			vectorEsperanza[i] = vectorEsperanza[i] / n;
			//En U[i] tengo el promedio del pixel i
		}

		
		//Creo la Matriz X donde X_i = (X_i - u ) / sqrt(n-1)
    	vector<vector<double> > matrizRestadoEsperanza(n, vector<double>(m, 0.0));
    	for (int i = 0 ;  i < m; i++){ //itero columnas
        	for (int j = 0; j < n; j++) { //itero filas
				matrizRestadoEsperanza[j][i] = (basicImagePixelMatrix[j][i] - vectorEsperanza[i]) / sqrt(n-1) ;
			}
    	}
    	
		//(X^t)
    	vector<vector<double> > matrizRestadoEsperanzaTraspuesta(m, vector<double>(n, 0.0));
		trasponerMatriz(matrizRestadoEsperanza, matrizRestadoEsperanzaTraspuesta);

		//Creo M matriz de covarianzas. M = [ (X^t)*(X) ] 
		
		vector<vector<double> > matrizCovarianza(m, vector<double>(m, 0.0));
		multiplicarMatricesDouble(matrizRestadoEsperanzaTraspuesta, matrizRestadoEsperanza, matrizCovarianza );
		


		// M * (1/(n-1)) ----> ¡NO! ya multiplicamos por sqrt(n-1)
		//multiplicarEscalarPorMatriz(1/(n-1), matrizCovarianza);
	
    
		//Ahora hay que diagonalizar M mediante el MetodoDeLaPotencia.
    	//Luego de obtener el primer autovalor y autovector, continuamos haciendo Deflacion para obtener el resto de autoval y autovec.
    	//Hacer ALFA deflaciones, donde ALFA es la cantidad de columnas relevantes que vamos a dejar en nuestro cambio de base.
		//Finalmente, la matriz de cambio de base no son mas que los ALFA autovectores mas relevantes de la matriz M.
     	
     	vector<vector<double> > matrizCambioBase(m, vector<double>(alfa, 0.0));
     	vector<double> autovalores(alfa, 0.0);
    	obtenerAutovectoresDeflacion(matrizCovarianza, matrizCambioBase, autovalores, alfa, niter);
    	
    	basisChangeMatrix = matrizCambioBase;

    	//pca matrix. (n x alfa); cambio de base (m x alfa)
    	pcaMatrix = vector<vector<double> > (n, vector<double>(alfa, 0.0));
    	multiplicarMatricesDouble(basicImagePixelMatrix, matrizCambioBase, pcaMatrix);

	}
	
	
	//Para hacerlo testeable se modifico m por el tamaño de la matriz parametro, se pasa por copia la matriz original
	//Recordar revertir cambios una vez que sepamos bien que funciona
	void obtenerAutovectoresDeflacion(vector<vector<double> > matrizOriginal, vector<vector<double> >& matrizResultado, vector<double>& autovalores, int alfa, int niter){
		
		//ESCENCIAL PARA RECORDAR EL ALFA DE LA PCA ACTUAL ya que las matrices son todas de n x m, no n x alfa
		this->alfa = alfa;
		//borrar despues de test
		int mPrima = matrizOriginal.size();
		//



    	for(int i=0; i< alfa ; i++){
      		double Ai; //Autovector de la iteracion i
			vector<double> Vi(mPrima, 0.0);//Autovector 1. Debe ser uno aleatorio para empezar el metodo de la potencia
    		for(int j=0;j<mPrima;j++){
        		double f = (double)rand() / (double)RAND_MAX;
          		Vi[j] = f*255;
        	}

      		Ai = metodoDeLaPotencia(matrizOriginal, Vi, niter);
      		// cout << Ai << endl;
      		autovalores[i] = Ai;
      		//Vi ahora tiene el iesimo autovector de M. Esta sera nuestra primera columna de la matriz 
  			for(int j=0; j<mPrima; j++){
      			matrizResultado[j][i] = Vi[j];
    		}
      		vector<vector<double> > matrizProductoAutovectores(mPrima, vector<double>(mPrima,0.0));
      		generarMatrizDeVectores(Vi, Vi, matrizProductoAutovectores); //Vi*Vi^t
      		multiplicarEscalarPorMatriz(Ai, matrizProductoAutovectores); // Ai * (Vi*Vi^t)
      		
    		restarMatrices(matrizOriginal, matrizProductoAutovectores);

    	}
    }


	double metodoDeLaPotencia(vector<vector<double> >& matriz, vector<double>& vec, int niter){
  		
  		if (matriz.size() == 0 || matriz[0].size() != vec.size())
  		{
  			cout << "Parametros para metodo de la potencia erroneos" << endl;
			return 0;
		}

		double lambda1; 
    	
    	for(int i = 0; i< niter; i++){
      		vector<double> tempVector(vec.size(), 0.0);
      		
      		multiplicarMatrizVectorDouble(matriz, vec, tempVector);

      		vec = tempVector;
      		normalizar(vec);
 	  		//En cada iteracion, perfecciono el autovector que devuelvo
    	}

    	
    	vector<double> matrizXVec(vec.size(), 0.0);

    	multiplicarMatrizVectorDouble(matriz, vec, matrizXVec);

    	double vtBvv = productoInternoVectores(vec, matrizXVec);
    	double vtv = productoInternoVectores(vec, vec);
    	lambda1 = vtBvv / vtv;
    	
    	return lambda1;
	}  

	//imagePixels es el vector imagen y K la cantidad de vecinos a considerar
	string resolverKnn(vector<vector<double> >& MatrizDatos, vector<Image*>& imagenes, vector<double> z, int k){
		
		//Knn no debe modificar el tamaño de la imagen. Asume que sus dimensiones coinciden con las de X con base cambiada(pcaMatrix)
		
		/*Hecho por grego. Se cambio por un multimap dado que si tienen la misma distancia(double) no se podria agregar dos elementos
		*	
		*
		*/

		// calculo distancias de z a cada una de la base y las inserto en un multimap
		// el significado será el número de la imagen (es necesario pues se ordenan todas en el multimap)
		multimap<double,int> distancias;
 		for(int i = 0; i < n; i ++){
	  		distancias.insert(pair<double,int>(distanceBetweenVector(z,MatrizDatos[i]), i));
		}

		multimap<double, int>::iterator it = distancias.begin();

		// armo vector de votaciones sólo considerando los k más cercanos
		// votaciones[i] es la cantidad de votos que tuvo la imagen i
		// en realidad votaciones[i] es 0 y 1, ya que o está entre las k más cercanas o no lo está.
		int vecinos = 0;
	 	vector<int> votaciones (n,0);
	 	while(it != distancias.end() && vecinos < k ){
	  		votaciones[it->second] += 1;
  			vecinos++;
	  		it++;
	 	}

	 	map<string,int> votacionDeLabel;

	 	//hago diccionario  key nombre de la imagen, value cantidad de votos
	 	for(int i = 0 ; i < votaciones.size(); i ++){
	 		if(votaciones[i] != 0 ){
	 			// si entramos a este caso, significa que la imagen i está entre las k más cercanas
	 			if(votacionDeLabel.find(imagenes[i]->label) == votacionDeLabel.end()){
	 				votacionDeLabel.insert(pair<string,int> (imagenes[i]->label, 1));	
	 			}
	 			else{
	 				map<string,int>::iterator currentIt = votacionDeLabel.find(imagenes[i]->label);
	 				currentIt -> second += 1;
	 			}	
	 		}
	 	}

	 	// recorro votacionDeLabel para quedarme con el de mayor repeticiones dentro de los k + cercanos
	 	// en otras palabras, me quedo con la moda del conjunto
	 	map<string,int>::iterator itRes = votacionDeLabel.begin();
	 	for(map<string,int>::iterator it = votacionDeLabel.begin(); it != votacionDeLabel.end() ; it ++){
	 		if(it -> second > itRes -> second){
	 			itRes = it;
	 		}
	 	}

	 	return itRes->first;
	}

	//imagePixels es el vector imagen y K la cantidad de vecinos a considerar
	string resolverKnnGrego(vector<vector<double> >& MatrizDatos, vector<Image*>& imagenes, vector<double> z, int k){
		
		//Knn no debe modificar el tamaño de la imagen. Asume que sus dimensiones coinciden con las de X con base cambiada(pcaMatrix)
		
		/*Hecho por grego. Se cambio por un multimap dado que si tienen la misma distancia(double) no se podria agregar dos elementos
		*	
		*
		*/

		// calculo distancias de z a cada una de la base y las inserto en un multimap
		// el significado será el número de la imagen (es necesario pues se ordenan todas en el multimap)
		multimap<double,int> distancias;
 		for(int i = 0; i < n; i ++){
	  		distancias.insert(pair<double,int>(distanceBetweenVector(z,MatrizDatos[i]), i));
		}

		multimap<double, int>::iterator it = distancias.begin();

		// armo vector de votaciones sólo considerando los k más cercanos
		// votaciones[i] es la cantidad de votos que tuvo la imagen i
		// en realidad votaciones[i] es 0 y 1, ya que o está entre las k más cercanas o no lo está.


	 	map<string,int> votacionDeLabel;
	 	int vecinos = 0;
	 	for(int i = 0 ; i < labels.size(); i ++){
	 		if(votacionDeLabel.find(labels[i]) == votacionDeLabel.end()){
	 			votacionDeLabel.insert(pair<string,int> (labels[i], 0));		
	 		}
	 	}

	 	map<string,int>::iterator labelMasVotado = votacionDeLabel.begin();
	 	while(it != distancias.end() && vecinos < k ){
	 		map<string,int>::iterator currentLabel = votacionDeLabel.find(labels[it->second]);
	 		currentLabel -> second += 1;

	 		if(currentLabel -> second > labelMasVotado -> second){
	 			labelMasVotado = currentLabel;
	 		}

  			vecinos++;
	  		it++;
	 	}

	 	return labelMasVotado->first;
	}

	string knnPorPuntos(vector<vector<double> >& MatrizDatos, vector<Image*>& imagenes, vector<double> z, int k){
		multimap<double,int> distancias;
 		for(int i = 0; i < n; i ++){
	  		distancias.insert(pair<double,int>(distanceBetweenVector(z,MatrizDatos[i]), i));
		}

		multimap<double, int>::iterator it = distancias.begin();

		map<string,int> votacionDeLabel;
	 	int vecinos = 0;
	 	for(int i = 0 ; i < labels.size(); i ++){
	 		if(votacionDeLabel.find(labels[i]) == votacionDeLabel.end()){
	 			votacionDeLabel.insert(pair<string,int> (labels[i], 0));		
	 		}
	 	}

	 	map<string,int>::iterator labelMasVotado = votacionDeLabel.begin();
	 	while(it != distancias.end() && vecinos < k ){
	 		map<string,int>::iterator currentLabel = votacionDeLabel.find(labels[it->second]);
	 		currentLabel -> second += (k - vecinos);

	 		if(currentLabel -> second > labelMasVotado -> second){
	 			labelMasVotado = currentLabel;
	 		}

  			vecinos++;
	  		it++;
	 	}

	 	return labelMasVotado->first;
	}
	

private:
	double obtainVectorNorm(vector<double>& vec, unsigned int imageLength){
		double norma2 = 0.0;

		for(int i = 0; i < vec.size(); i++){
			norma2 += vec[i]*vec[i];
		}

		norma2 = sqrt(norma2);

		return norma2;
	}

	void normalizar(vector<double>& vec){
		double sumaDouble = 0.0;

		for (int i = 0; i < vec.size(); ++i)
		{
			sumaDouble += vec[i]*vec[i];
		}

		sumaDouble = sqrt(sumaDouble);

		for (int i = 0; i < vec.size(); ++i)
		{
			vec[i] = vec[i]/sumaDouble; 
		}
	}


	void createImageMatrix(){
    	vector<vector<double> > matrix(imageDataSet.size(), vector<double>(m, 0.0));
		for (int i = 0; i < n; ++i)
		{		
        	for( int j = 0; j < m; j++){
      			matrix[i][j] = imageDataSet[i]->obtainPixelValue(j); 
    		}
		}

		basicImagePixelMatrix = matrix;	
	}

	int maximo (vector<int>& v){
		int res = v[0];
		int n = v.size();
		for(int i = 0 ; i < n; i ++){
			res = res > v[i] ? res : v[i];
		}
 		return res;
	}

	void obtainImageInVectorDoubles(Image& image, vector<double>& z){
		for(int i = 0 ; i < m ; i ++){
			double pixel = image.obtainPixelValue(i);
			z.push_back(pixel);
		}
	} 

	void applyBasisChangeToImagePixelsVector(std::vector<double>& imagePixels, vector<double>& imageAfterBasisChange){
		multiplicarMatrizVectorDouble(pcaMatrix,imagePixels,imageAfterBasisChange);
	}

	vector<double> restaEntreVectores(vector<double> v1, vector<double> v2 ){
		vector<double> res ;
		for(int i = 0 ; i < v1.size(); i ++){
			res.push_back(v1[i] - v2[i]);
		}
		return res;
	}

	double distanceBetweenVector(vector<double> v1, vector<double> v2 ){
		std::vector<double> v3 = restaEntreVectores(v1,v2);
		sort(v3.begin(), v3.end());
		
		double ac = 0 ;
		for(int i = 0 ; i < v3.size(); i ++){
			ac += v3[i]*v3[i];
		}
		return ac;
	}
};