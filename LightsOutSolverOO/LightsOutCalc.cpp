#include "LightsOutCalc.h"

LightsOutCalc::LightsOutCalc(const int n, const int m) : m_n(n), m_m(m), m_buttonRepMat(createButtonRepMat()) {
	m_solutionMat = nullptr;
	checkIsAlwaysSolvable();
}

LightsOutCalc::~LightsOutCalc() {
	delete[] m_buttonRepMat;
	deleteSolutionMat();
	return;
}

int* LightsOutCalc::solve(int* startVect) {
	int* solution = nullptr;

	m_solutionMat = gaussJordanElim(startVect);
	if (checkIsSolvable())
		solution = createSolutionVect();

	delete[] startVect;
	deleteSolutionMat();
	m_solutionMat = nullptr;
	return solution;
}

int** LightsOutCalc::createSolutionMat(int* startVect) {
	int** tempMat = new int* [m_size];

	if (startVect) {
		for (int i = 0; i < m_size; i++) {
			tempMat[i] = (int*)malloc((m_size + 1) * sizeof(int));
			memcpy_s(tempMat[i], (m_size + 1) * sizeof(int), m_buttonRepMat + (i * m_size), (m_size) * sizeof(int));
			tempMat[i][m_size] = startVect[i];
		}
	}
	else {
		for (int i = 0; i < m_size; i++) {
			tempMat[i] = (int*)malloc(m_size * sizeof(int));
			memcpy_s(tempMat[i], m_size * sizeof(int), m_buttonRepMat + (i * m_size), m_size * sizeof(int));
		}
	}
	return tempMat;
}

const int* const LightsOutCalc::createButtonRepMat() {
	int moveVect[5] = { 0, -1, -m_m, 1, m_m };
	int* resultMat = new int[(int)pow(m_size, 2)]();
	for (int i = 0; i < m_size; i++) {
		for (int j = 0; j < 5; j++) {
			int moveVal = i + moveVect[j];
			if (moveVal < 0 || moveVal >= m_size)
				continue;
			//Invalid moves
			if ((i % m_m == 0 && j == 1) || ((i + 1) % m_m == 0 && j == 3))
				continue;
			int rowCalc = i * (m_size)+moveVal;
			resultMat[rowCalc] = 1;
		}
	}
	return resultMat;
}

void LightsOutCalc::invertVect(int* vect) {
	for (int i = 0; i < m_size; i++) {
		if (vect[i] == 0) {
			vect[i] = 1;
		}
		else {
			vect[i] = 0;
		}
	}
	return;
}

int** LightsOutCalc::gaussJordanElim(int* startVect) {
	int** tempMat = createSolutionMat(startVect);
	int rowSize = startVect ? m_size + 1 : m_size;

	// Column by column
	for (int i = 0; i < m_size; i++) {
		if (tempMat[i][i] != 1) {
			int rowWith1 = findRowWith1(tempMat, i);
			if (rowWith1 < 0)
				continue;
			swapRows(tempMat, i, rowWith1);
		}
		// Row by row
		for (int j = 0; j < m_size; j++) {
			if (i == j || tempMat[j][i] == 0)
				continue;
			addRows(tempMat, rowSize, i, j);
		}
	}
	return tempMat;
}

int LightsOutCalc::findRowWith1(int** mat, int col) {
	for (int i = col + 1; i < m_size; i++) {
		if (mat[i][col] == 1)
			return i;
	}
	return -1;
}

void LightsOutCalc::swapRows(int** mat, int currentRow, int rowToSwap) {
	int* tempRow = mat[currentRow];
	mat[currentRow] = mat[rowToSwap];
	mat[rowToSwap] = tempRow;
	return;
}

void LightsOutCalc::addRows(int** mat, int rowSize, int currentRow, int rowToAdd) {
	for (int i = 0; i < rowSize; i++) {
		mat[rowToAdd][i] = mod2Add(mat[rowToAdd][i], mat[currentRow][i]);
	}
	return;
}

int LightsOutCalc::mod2Add(int value1, int value2) {
	int result = (value1 + value2) % 2;
	return result;
}

int* LightsOutCalc::createSolutionVect() {
	int* solutionVect = new int[m_size];
	for (int i = 0; i < m_size; i++)
		solutionVect[i] = m_solutionMat[i][m_size];
	return solutionVect;
}

void LightsOutCalc::checkIsAlwaysSolvable() {
	int** tempMat = gaussJordanElim();

	for (int i = 0; i < m_size; i++)
	{
		int total = 0;
		for (int j = 0; j < m_size; j++)
		{ 
			total += tempMat[i][j];
		}
		if (!total)
		{
			m_isAlwaysSolvable = false;
			delete[] tempMat;
			return;
		}
	}
	delete[] tempMat;
	return;
}

bool LightsOutCalc::checkIsSolvable() 
{
	if (m_isAlwaysSolvable)
		return true;

	for (int i = 0; i < m_size; i++) 
	{
		int total = 0;
		for (int j = 0; j < m_size; j++)
		{
			total += m_solutionMat[i][j];
		}
		if (!total && m_solutionMat[i][m_size] == 1)
		{
			return false;
		}
	}
	return true;
}

void LightsOutCalc::deleteSolutionMat() {
	if (m_solutionMat) {
		for (int i = 0; i < m_size; i++) {
			delete[] m_solutionMat[i];
		}
		delete[] m_solutionMat;
	}
}
