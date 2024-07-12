#pragma once

#include "LightsOutSolverOO.h"

class LightsOutGrid
{
public:
	const HWND m_hwnd;
	const int m_n;
	const int m_size = pow(m_n, 2);
	const int m_buttonSize = 50;
	const int m_xPos;
	const int m_yPos;
	const int m_spacing = 5;
	int* m_buttonOnArr = nullptr;
	float const * const * m_buttonPosArr = nullptr;

public:
	LightsOutGrid(HWND hWnd, int n, int xPos, int yPos);
	~LightsOutGrid();
	void clearGrid();
	int* getButtonOnArrByVal();
	int* getButtonOnArrByValInver();
	void setButtonOnArr(int* vect);

private:
	void initializeButtonOnArr();
	void initializeButtonPosArr();
};

