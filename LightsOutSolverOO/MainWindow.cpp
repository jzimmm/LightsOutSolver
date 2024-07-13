#include "MainWindow.h"

MainWindow::MainWindow() :
    m_hwnd(NULL),
    m_clearButton(NULL),
    m_solutionButton(NULL),
    m_gridSizeCombo(NULL),
    m_lightsOnCheck(NULL),
    m_pDirect2dFactory(NULL),
    m_pDirectWriteFactory(NULL),
    m_pDWriteTextFormat(NULL),
    m_pRenderTarget(NULL),
    m_pLightSlateGrayBrush(NULL),
    m_pBlueBrush(NULL),
    m_pWhiteBrush(NULL),
    m_pRedBrush(NULL),
    m_lightsOutCalc(NULL),
    m_lightsOutGrid(NULL),
    m_solutionGrid(NULL)
{}

MainWindow::~MainWindow()
{
    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pDirectWriteFactory);
    SafeRelease(&m_pDWriteTextFormat);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pBlueBrush);
    SafeRelease(&m_pWhiteBrush);
    SafeRelease(&m_pRedBrush);
    delete m_lightsOutCalc;
    delete m_lightsOutGrid;
    delete m_solutionGrid;
}

HRESULT MainWindow::Initialize()
{
    HRESULT hr;

    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = MainWindow::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
        wcex.lpszClassName = L"D2DMainWindow";

        RegisterClassEx(&wcex);

        m_hwnd = CreateWindow(
            L"D2DMainWindow",
            L"Lights Out Solver",
            WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this);

        hr = m_hwnd ? S_OK : E_FAIL;
        if (m_hwnd)
        {
            SetClassLongPtr(m_hwnd,          // window handle 
                GCLP_HICON,              // changes icon 
                (LONG_PTR)LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(102))
            );

            float dpi = GetDpiForWindow(m_hwnd);

            SetWindowPos(
                m_hwnd,
                NULL,
                NULL,
                NULL,
                static_cast<int>(ceil(800.f * dpi / 96.f)),
                static_cast<int>(ceil(400.f * dpi / 96.f)),
                SWP_NOMOVE);
            ShowWindow(m_hwnd, SW_SHOWNORMAL);

            RECT rc;
            GetClientRect(m_hwnd, &rc);

            int xPos = (rc.right - rc.left) / 4;
            int yPos = (rc.bottom - rc.top) / 2.25;

            m_lightsOutGrid = new LightsOutGrid(m_hwnd, 3, xPos, yPos);

            xPos += (rc.right - rc.left) / 2;
            
            m_solutionGrid = new LightsOutGrid(m_hwnd, 3, xPos, yPos);

            UpdateWindow(m_hwnd);
        }

        m_lightsOutCalc = new LightsOutCalc(3, 3);
    }

    return hr;
}

void MainWindow::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HRESULT MainWindow::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;
    static const WCHAR msc_fontName[] = L"Verdana";
    static const FLOAT msc_fontSize = 12;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite factory.
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(m_pDirectWriteFactory),
            reinterpret_cast<IUnknown**>(&m_pDirectWriteFactory)
        );
    }

    if (SUCCEEDED(hr))
    {
        // Create a DirectWrite text format object.
        hr = m_pDirectWriteFactory->CreateTextFormat(
            msc_fontName,
            NULL,
            DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            msc_fontSize,
            L"", //locale
            &m_pDWriteTextFormat
        );
    }

    if (SUCCEEDED(hr))
    {
        // Center the text horizontally and vertically.
        m_pDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

        m_pDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    return hr;
}

HRESULT MainWindow::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(
                DXGI_FORMAT_B8G8R8A8_UNORM,
                D2D1_ALPHA_MODE_IGNORE),
            0,
            0,
            D2D1_RENDER_TARGET_USAGE_NONE,
            D2D1_FEATURE_LEVEL_DEFAULT
        );

        hr = m_pDirect2dFactory->CreateDCRenderTarget(&props, &m_pRenderTarget);

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::LightSlateGray),
                &m_pLightSlateGrayBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::DeepSkyBlue),
                &m_pBlueBrush
            );
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::White),
                &m_pWhiteBrush
            );
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Red),
                &m_pRedBrush
            );
        }

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::LawnGreen),
                &m_pGreenBrush
            );
        }
    }

    return hr;
}

HRESULT MainWindow::OnRender(const PAINTSTRUCT &ps)
{
    HRESULT hr;
    RECT rc;

    GetClientRect(m_hwnd, &rc);


    hr = CreateDeviceResources();

    if (SUCCEEDED(hr))
    {

        hr = m_pRenderTarget->BindDC(ps.hdc, &rc);

        m_pRenderTarget->BeginDraw();
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        m_pRenderTarget->Clear(D2D1::ColorF(0.128f, 0.128f, 0.128f, 1.f));

        D2D1_SIZE_F renderTargetSize = m_pRenderTarget->GetSize();

        static const WCHAR lightsOnText[] = L"Solve for Lights On";

        m_pRenderTarget->DrawText(
            lightsOnText,
            ARRAYSIZE(lightsOnText) - 1,
            m_pDWriteTextFormat,
            D2D1::RectF(510, 319, 660, 342),
            m_pWhiteBrush
        );
        

        float lineX = (rc.right - rc.left) / 2;
        float lineY1 = rc.top + 40;
        float lineY2 = rc.bottom - 75;

        m_pRenderTarget->DrawLine(
            D2D1::Point2F(lineX, lineY1),
            D2D1::Point2F(lineX, lineY2),
            m_pWhiteBrush,
            4.f
        );

        D2D1_ROUNDED_RECT* buttonArr = (D2D1_ROUNDED_RECT*)calloc(m_lightsOutGrid->m_size, sizeof(D2D1_ROUNDED_RECT));

        for (int i = 0; i < m_lightsOutGrid->m_size; i++) 
        {
            buttonArr[i] = D2D1::RoundedRect(
                D2D1::RectF(
                    m_lightsOutGrid->m_buttonPosArr[i][0],
                    m_lightsOutGrid->m_buttonPosArr[i][1],
                    m_lightsOutGrid->m_buttonPosArr[i][2],
                    m_lightsOutGrid->m_buttonPosArr[i][3]
                ),
                5.f,
                5.f
               );

            if (!m_lightsOutGrid->m_buttonOnArr[i]) 
            {
                m_pRenderTarget->FillRoundedRectangle(&buttonArr[i], m_pLightSlateGrayBrush);
            }
            else
            {
                m_pRenderTarget->FillRoundedRectangle(&buttonArr[i], m_pBlueBrush);
            }
        }

        D2D1_ROUNDED_RECT* solutionArr = nullptr;

        if (m_displaySolution)
        {
            solutionArr = (D2D1_ROUNDED_RECT*)calloc(m_solutionGrid->m_size, sizeof(D2D1_ROUNDED_RECT));

            for (int i = 0; i < m_solutionGrid->m_size; i++) 
            {
                solutionArr[i] = D2D1::RoundedRect(
                    D2D1::RectF(m_solutionGrid->m_buttonPosArr[i][0],
                        m_solutionGrid->m_buttonPosArr[i][1],
                        m_solutionGrid->m_buttonPosArr[i][2],
                        m_solutionGrid->m_buttonPosArr[i][3]
                    ),
                    5.f,
                    5.f
                );

                if (!m_solutionGrid->m_buttonOnArr[i])
                {
                    m_pRenderTarget->FillRoundedRectangle(&solutionArr[i], m_pLightSlateGrayBrush);
                }
                else
                {
                    m_pRenderTarget->FillRoundedRectangle(&solutionArr[i], m_pGreenBrush);
                }
            }
        }
        else if (m_displayNoSolution)
        {
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(502.f, 75.f),
                D2D1::Point2F(674.f, 245.f),
                m_pRedBrush,
                16.f
            );

            m_pRenderTarget->DrawLine(
                D2D1::Point2F(674.f, 75.f),
                D2D1::Point2F(502.f, 245.f),
                m_pRedBrush,
                16.f
            );
        }

        hr = m_pRenderTarget->EndDraw();
        free(buttonArr);

        if (solutionArr)
            free(solutionArr);
    }

    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }

    return hr;
}

void MainWindow::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pBlueBrush);
    SafeRelease(&m_pWhiteBrush);
    SafeRelease(&m_pRedBrush);
}

LRESULT CALLBACK MainWindow::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        MainWindow* pMainWindow = (MainWindow*)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pMainWindow)
        );

        int buttonYPos = 320;

        pMainWindow->m_gridSizeCombo = CreateWindow(L"COMBOBOX", TEXT(""),
            CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
            20, buttonYPos, 80, 80, hwnd, NULL, HINST_THISCOMPONENT,
            NULL);

        //ADD 2 ITEMS
        SendMessage(
            pMainWindow->m_gridSizeCombo,
            (UINT)CB_ADDSTRING,
            (WPARAM)0, (LPARAM)TEXT("3 x 3"));
        SendMessage(
            pMainWindow->m_gridSizeCombo,
            (UINT)CB_ADDSTRING,
            (WPARAM)0, (LPARAM)TEXT("5 x 5"));
        SendMessage(pMainWindow->m_gridSizeCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

        pMainWindow->m_clearButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"Clear",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            159,         // x position 
            buttonYPos,         // y position 
            80,        // Button width
            25,        // Button height
            hwnd,     // Parent window
            (HMENU)BTN_CLEAR,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        pMainWindow->m_solutionButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"Solution",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            345,         // x position 
            buttonYPos,         // y position 
            100,        // Button width
            25,        // Button height
            hwnd,     // Parent window
            (HMENU)BTN_SOLUTION,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        pMainWindow->m_lightsOnCheck = CreateWindow(
            L"BUTTON",
            L"",
            BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD,
            650,
            buttonYPos + 2,
            15,
            20,
            hwnd,
            (HMENU)BTN_LIGHTS_ON,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        result = 1;
    }
    else 
    {

        MainWindow* pMainWindow = reinterpret_cast<MainWindow*>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
            )));

        bool wasHandled = false;

        if (pMainWindow)
        {
            switch (message)
            {
            case WM_COMMAND:
                if (HIWORD(wParam) == CBN_SELCHANGE)
                {
                    int itemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
                        (WPARAM)0, (LPARAM)0);
                    TCHAR  listItem[6];
                    (TCHAR)SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT,
                        (WPARAM)itemIndex, (LPARAM)listItem);
                    pMainWindow->refreshGrid(hwnd, &pMainWindow->m_lightsOutGrid, pMainWindow->getGridSize(listItem));
                    pMainWindow->refreshGrid(hwnd, &pMainWindow->m_solutionGrid, pMainWindow->getGridSize(listItem));
                    pMainWindow->refreshCalc(pMainWindow->getGridSize(listItem));
                    pMainWindow->m_displaySolution = false;
                    pMainWindow->m_displayNoSolution = false;
                    InvalidateRect(hwnd, NULL, FALSE);
                }
                else if (HIWORD(wParam) == BN_CLICKED)
                {
                    if (LOWORD(wParam) == BTN_CLEAR)
                    {
                        pMainWindow->m_lightsOutGrid->clearGrid();
                    }
                    else if (LOWORD(wParam == BTN_SOLUTION))
                    {
                        if (IsDlgButtonChecked(hwnd, BTN_LIGHTS_ON) == BST_UNCHECKED)
                        {
                            int* solution = pMainWindow->m_lightsOutCalc->solve(pMainWindow->m_lightsOutGrid->getButtonOnArrByVal());
                            if (solution)
                            {
                                pMainWindow->m_solutionGrid->setButtonOnArr(solution);
                                pMainWindow->m_displaySolution = true;
                                pMainWindow->m_displayNoSolution = false;
                            }
                            else
                            {
                                pMainWindow->m_displayNoSolution = true;
                                pMainWindow->m_displaySolution = false;
                            }
                        }
                        else
                        {
                            int* solution = pMainWindow->m_lightsOutCalc->solve(pMainWindow->m_lightsOutGrid->getButtonOnArrByValInver());
                            if (solution)
                            {
                                pMainWindow->m_solutionGrid->setButtonOnArr(solution);
                                pMainWindow->m_displaySolution = true;
                            }
                            else
                            {
                                pMainWindow->m_displayNoSolution = true;
                                pMainWindow->m_displaySolution = false;
                            }
                        }
                    }

                    InvalidateRect(hwnd, NULL, false);
                }

                wasHandled = true;
                result = 0;
                break;

            case WM_DISPLAYCHANGE:
            {
                InvalidateRect(hwnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_CTLCOLORSTATIC:
            {
                if ((HWND)lParam == GetDlgItem(hwnd, BTN_LIGHTS_ON))
                {
                    return (LRESULT)CreateSolidBrush(RGB(33, 33, 33));
                }
            }
            break;
            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                pMainWindow->OnRender(ps);
                EndPaint(hwnd, &ps);
                ValidateRect(hwnd, NULL);
            }
            result = 0;
            wasHandled = true;
            break;
            case WM_LBUTTONUP:
            {
                if (pMainWindow->onGridClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
                {
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            result = 1;
            wasHandled = true;
            break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

bool MainWindow::onGridClick(int xPos, int yPos) {

    if (xPos < m_lightsOutGrid->m_buttonPosArr[0][0] || xPos > m_lightsOutGrid->m_buttonPosArr[m_lightsOutGrid->m_n - 1][2])
    {
        if (yPos < m_lightsOutGrid->m_buttonPosArr[0][1] || yPos > m_lightsOutGrid->m_buttonPosArr[m_lightsOutGrid->m_size - 1][3])
            return false;
    }

    for (int i = 0; i < m_lightsOutGrid->m_size; i++) {
        if (xPos > m_lightsOutGrid->m_buttonPosArr[i][0] && yPos > m_lightsOutGrid->m_buttonPosArr[i][1]) {
            if (xPos < m_lightsOutGrid->m_buttonPosArr[i][2] && yPos < m_lightsOutGrid->m_buttonPosArr[i][3])
            {
                m_lightsOutGrid->m_buttonOnArr[i] = !m_lightsOutGrid->m_buttonOnArr[i];
                return true;
            }
        }
    }
    return false;
}

int MainWindow::getGridSize(WCHAR* listItem) {
    if (*listItem == '3')
        return 3;
    if (*listItem == '5')
        return 5;
}

void MainWindow::refreshGrid(HWND hwnd, LightsOutGrid** grid, int n) {
    LightsOutGrid* temp = *grid;
    *grid = new LightsOutGrid(hwnd, n, temp->m_xPos, temp->m_yPos);
    delete temp;
}

void MainWindow::refreshCalc(int n) {
    LightsOutCalc* temp = m_lightsOutCalc;
    m_lightsOutCalc = new LightsOutCalc(n, n);
    delete temp;
}