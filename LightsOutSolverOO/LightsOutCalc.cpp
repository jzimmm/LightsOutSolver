#include "LightsOutCalc.h"

LightsOutCalc::LightsOutCalc(const int n, const int m) : 
	m_n(n), 
	m_m(m), 
	m_size(n * m),
	m_buttonMapVect(createButtonMapVect()),
	m_solutionMat(initSolutionMat())
{
	m_isAlwaysSolvable = checkIsAlwaysSolvable();
}

LightsOutCalc::~LightsOutCalc() {
	free((int*)m_buttonMapVect);
	deleteSolutionMat();
	return;
}

int* LightsOutCalc::solve(int* startVect) {
	createSolutionMat(startVect);
	gaussJordanElim(m_solutionMat, m_size + 1);
	return checkIsSolvable() ? createSolutionVect() : nullptr;
}

int** LightsOutCalc::initSolutionMat() {
	int** tempMat = (int**)malloc(m_size * sizeof(int*));
	for (int i = 0; i < m_size; i++) {
		tempMat[i] = (int*)malloc((m_size + 1) * sizeof(int));
	}
	return tempMat;
}

void LightsOutCalc::createSolutionMat(int* startVect) {
	for (int i = 0; i < m_size; i++) {
		memcpy_s(m_solutionMat[i], (m_size + 1) * sizeof(int), m_buttonMapVect + (i * m_size), (m_size) * sizeof(int));
		m_solutionMat[i][m_size] = startVect[i];
	}
}

int** LightsOutCalc::createSolutionMat() {
	int** tempMat = (int**)malloc(m_size * sizeof(int*));
	for (int i = 0; i < m_size; i++) {
		tempMat[i] = (int*)malloc(m_size * sizeof(int));
		memcpy_s(tempMat[i], m_size * sizeof(int), m_buttonMapVect + (i * m_size), m_size * sizeof(int));
	}
	return tempMat;
}

const int* const LightsOutCalc::createButtonMapVect() {
	int moveVect[5] = { 0, -1, -m_m, 1, m_m };
	int* resultMat = (int*)calloc(m_size * m_size, sizeof(int));
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

void LightsOutCalc::gaussJordanElim(int** destMat, int rowSize) {
	// Column by column
	for (int i = 0; i < m_size; i++) {
		if (destMat[i][i] != 1) {
			int rowWith1 = findRowWith1(destMat, i);
			if (rowWith1 < 0)
				continue;
			swapRows(destMat, i, rowWith1);
		}
		// Row by row
		for (int j = 0; j < m_size; j++) {
			if (i == j || destMat[j][i] == 0)
				continue;
			addRows(destMat, rowSize, i, j);
		}
	}
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
	int* solutionVect = (int*)malloc(m_size * sizeof(int));
	for (int i = 0; i < m_size; i++)
		solutionVect[i] = m_solutionMat[i][m_size];
	return solutionVect;
}

bool LightsOutCalc::checkIsAlwaysSolvable() {
	int** tempMat = createSolutionMat();
	gaussJordanElim(tempMat, m_size);
	for (int i = 0; i < m_size; i++)
	{
		int total = 0;
		for (int j = 0; j < m_size; j++)
		{ 
			total += tempMat[i][j];
		}
		if (!total)
		{
			free(tempMat);
			return false;
		}
	}
	free(tempMat);
	return true;
}

bool LightsOutCalc::checkIsSolvable() {
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
			free(m_solutionMat[i]);
		}
		free(m_solutionMat);
	}
}
