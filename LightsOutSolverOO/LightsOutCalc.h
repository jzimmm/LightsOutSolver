#pragma once

#include "LightsOutSolverOO.h"

class LightsOutCalc {
private:
	const int m_n;
	const int m_m;
	const int m_size;
	bool m_isAlwaysSolvable;
	const int* const m_buttonMapVect;
	int** m_solutionMat;

public:
	LightsOutCalc(int n, int m);
	~LightsOutCalc();
	int* solve(int* startVect);

private:
	void initSolutionMat();
	void createSolutionMat(int* startVect);
	int** createSolutionMat();
	const int* const createButtonMapVect();
	void gaussJordanElim(int** resultMat, int rowSize);
	int findRowWith1(int** mat, int col);
	void swapRows(int** mat, int currentRow, int rowToSwap);
	void addRows(int** mat, int rowSize, int currentRow, int rowToAdd);
	int mod2Add(int value1, int value2);
	int* createSolutionVect();
	bool checkIsAlwaysSolvable();
	bool checkIsSolvable();
	void deleteSolutionMat();
};

