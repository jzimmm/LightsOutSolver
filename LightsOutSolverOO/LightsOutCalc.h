#pragma once

#include "LightsOutSolverOO.h"

class LightsOutCalc {
private:
	const int m_n;
	const int m_m;
	const int m_size = m_n * m_m;
	bool m_isAlwaysSolvable = true;
	const int* const m_buttonRepMat;
	int** m_solutionMat;

public:
	LightsOutCalc(int n, int m);
	~LightsOutCalc();
	int* solve(int* startVect);

private:
	int** createSolutionMat(int* startVect);
	const int* const createButtonRepMat();
	void invertVect(int* vect);
	int** gaussJordanElim(int* startVect = nullptr);
	int findRowWith1(int** mat, int col);
	void swapRows(int** mat, int currentRow, int rowToSwap);
	void addRows(int** mat, int rowSize, int currentRow, int rowToAdd);
	int mod2Add(int value1, int value2);
	int* createSolutionVect();
	void checkIsAlwaysSolvable();
	bool checkIsSolvable();
	void deleteSolutionMat();
};

