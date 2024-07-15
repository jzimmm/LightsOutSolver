#include "LightsOutGrid.h"

LightsOutGrid::LightsOutGrid(HWND hwnd, int n, int buttonSize, int spacing, int xPos, int yPos) :
	m_hwnd(hwnd),
	m_n(n),
	m_size(n * n),
	m_buttonSize(buttonSize),
	m_spacing(spacing),
	m_xPos(xPos),
	m_yPos(yPos),
	m_buttonStateVect(initButtonStateVect()),
	m_buttonPosMat(initButtonPosMat())
{}

LightsOutGrid::~LightsOutGrid() {
	free(m_buttonStateVect);
	for (int i = 0; i < m_size; i++) {
		free((float*)m_buttonPosMat[i]);
	}
	free((float**)m_buttonPosMat);
}

int* LightsOutGrid::initButtonStateVect() {
	return (int*)calloc(m_size, sizeof(int));
}

float const* const* LightsOutGrid::initButtonPosMat() {

	const float rowTopStart = m_yPos - (((m_buttonSize * m_n) + (m_spacing * (m_n - 1))) / 2);
	const float columnLeftStart = m_xPos - (((m_buttonSize * m_n) + (m_spacing * (m_n - 1))) / 2);

	float rowTop = rowTopStart;
	float columnLeft = columnLeftStart;

	float** tempArr = (float**)malloc(m_size * sizeof(float*));
	for (int i = 0; i < m_n; i++) {
		for (int j = 0; j < m_n; j++) {
			int arrIndex = (i * m_n) + j;
			tempArr[arrIndex] = (float*)malloc(4 * sizeof(float));
			float buttonX = columnLeft;
			float buttonY = rowTop;
			for (int k = 0; k < 4; k++) {
				if (k % 2 == 0) 
				{
					tempArr[arrIndex][k] = buttonX;
					buttonX += m_buttonSize;
				}
				else 
				{
					tempArr[arrIndex][k] = buttonY;
					buttonY += m_buttonSize;
				}
			}
			columnLeft = columnLeftStart + ((m_buttonSize + m_spacing) * (j + 1));
		}
		columnLeft = columnLeftStart;
		rowTop = rowTopStart + ((m_buttonSize + m_spacing) * (i + 1));
	}

	return tempArr;
}

void LightsOutGrid::clearGrid() {
	memset(m_buttonStateVect, 0, m_size * sizeof(int));
}

int* LightsOutGrid::getButtonStateVectVal() {
	int* result = (int*)malloc(m_size * sizeof(int));
	memcpy_s(result, sizeof(int) * m_size, m_buttonStateVect, sizeof(int) * m_size);
	return result;
}

int* LightsOutGrid::getButtonStateVectValInvert() {
	int* result = (int*)malloc(m_size * sizeof(int));
	for (int i = 0; i < m_size; i++) {
		result[i] = !m_buttonStateVect[i];
	}
	return result;
}

void LightsOutGrid::setButtonStateVect(int* vect) {
	int* temp = m_buttonStateVect;
	m_buttonStateVect = vect;
	free(temp);
}