# include "matrixInversion_20190320.h"
# include <iostream>

using namespace std;

vector<vector<double>> matrixInversion(vector<vector<double>> matrix) {

	double pivot, d = 1.0;

	for (int p = 0; p < matrix.size(); p++)
	{
		pivot = matrix[p][p];
		d = d * pivot;

		if (abs(pivot) <= 1e-15) {
			cout << "ERROR: matrixInversion.cpp (singular matrix!)" << "\n";
			break;
		}
		for (int i = 0; i < matrix.size(); i++)
			matrix[i][p] = -matrix[i][p] / pivot;

		for (int i = 0; i < matrix.size(); i++)
			if (i != p)
				for (int j = 0; j < matrix.size(); j++)
					if (j != p) matrix[i][j] = matrix[i][j] + matrix[p][j] * matrix[i][p];
		for (int j = 0; j < matrix.size(); j++)
			matrix[p][j] = matrix[p][j] / pivot;
		matrix[p][p] = 1 / pivot;
	}

	return matrix;
}