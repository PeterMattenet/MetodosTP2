all:
	g++ -std=c++11 main.cpp -o main

testMatriz:
	g++ -std=c++11 tests.cpp -o testMatriz

testDeflacion:
	g++ -std=c++11 testDeflacion.cpp -o testDeflacion

testPredictor:
	g++ -std=c++11 testPredictor.cpp -o testPredictor

clean:
	rm testPredictor -f
	rm testDeflacion -f
	rm testMatriz -f
	rm main -f