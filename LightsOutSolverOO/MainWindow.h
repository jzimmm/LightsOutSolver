#pragma once

#include "LightsOutSolverOO.h"
#include "LightsOutGrid.h"
#include "LightsOutCalc.h"

#define BTN_CLEAR  100
#define BTN_SOLUTION 101
#define BTN_LIGHTS_ON 102

template<class Interface>
inline void SafeRelease(
    Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}

class MainWindow
{
private:
    HWND m_hwnd;
    HWND m_clearButton;
    HWND m_solutionButton;
    HWND m_gridSizeCombo;
    HWND m_lightsOnCheck;
    ID2D1Factory* m_pDirect2dFactory;
    IDWriteFactory* m_pDirectWriteFactory;
    IDWriteTextFormat* m_pDWriteTextFormat;
    ID2D1DCRenderTarget* m_pRenderTarget;
    ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
    ID2D1SolidColorBrush* m_pBlueBrush;
    ID2D1SolidColorBrush* m_pWhiteBrush;
    ID2D1SolidColorBrush* m_pRedBrush;
    ID2D1SolidColorBrush* m_pGreenBrush;
    LightsOutCalc* m_lightsOutCalc;
    LightsOutGrid* m_lightsOutGrid;
    LightsOutGrid* m_solutionGrid;
    bool m_displaySolution = false;
    bool m_displayNoSolution = false;

public:
    MainWindow();
    ~MainWindow();
    HRESULT Initialize();
    void RunMessageLoop();

private:
    HRESULT CreateDeviceIndependentResources();
    HRESULT CreateDeviceResources();
    void DiscardDeviceResources();
    HRESULT OnRender(const PAINTSTRUCT& ps);

    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );

    void registerStaticControls(HWND hwnd);
    bool onGridClick(int xPos, int yPos);
    void onGridSizeChange(TCHAR* listItem);
    void onSolutionClick(int* gridState);
    int getGridSize(WCHAR* listItem);
    void refreshGrid(HWND hwnd, LightsOutGrid** grid, int n);
    void refreshCalc(int n);

};

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif