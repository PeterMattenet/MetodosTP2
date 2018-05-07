#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <vector>

class Matrix<class T>
{
public:
	Matrix(int n, int m);
	~Matrix();

	T& operator[][](int i, int j);

	int n;
	int m;
private:
	vector<vector<T> > privateMatrix;

	
};

#endif //__IMAGE_H__