#pragma once

#include "LightsOutSolverOO.h"

class LightsOutGrid
{
public:
	const HWND m_hwnd;
	const int m_n;
	const int m_size;
	const int m_buttonSize;
	const int m_xPos;
	const int m_yPos;
	const int m_spacing;
	int* m_buttonStateVect;
	float const * const * m_buttonPosMat;

public:
	LightsOutGrid(HWND hwnd, int n, int buttonSize, int spacing, int xPos, int yPos);
	~LightsOutGrid();
	void clearGrid();
	int* getButtonStateVectVal();
	int* getButtonStateVectValInvert();
	void setButtonStateVect(int* vect);

private:
	int* initButtonStateVect();
	float const* const* initButtonPosMat();
};

