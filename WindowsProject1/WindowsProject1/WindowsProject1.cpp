// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WindowsProject1.h"
#pragma warning (disable : 4996)

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

PIP_ADAPTER_INFO pAdapterInfo;
PIP_ADAPTER_INFO pAdapter;

wchar_t octets[MAX_OCTETS][MAX_OCTET_LENGTH];

WCHAR Gipstr[INET_ADDRSTRLEN];


// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON)); // 메뉴표시줄 위 아이콘
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MYICON)); // 여기가 작업표시줄

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// 변환할 멀티바이트 문자열
LPCWSTR multiByteToWideChar(char multibyteString[]) {
    // 변환할 버퍼의 크기를 계산
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, multibyteString, -1, NULL, 0);
    WCHAR* ret = new WCHAR[bufferSize];

    // 유니코드 문자열을 저장할 버퍼 할당
    WCHAR* wideString = (WCHAR*)malloc(bufferSize * sizeof(WCHAR));

    // 멀티바이트 문자열을 유니코드 문자열로 변환
    MultiByteToWideChar(CP_UTF8, 0, multibyteString, -1, ret, bufferSize);

    return ret;
}
PIP_ADAPTER_INFO getSelectAdapterInfo(int sel) {
    PIP_ADAPTER_INFO pAdapterInfo1 = pAdapterInfo;
    
    int i = 0;

    while (true) {
        if (i > sel) {
            return NULL;
        }
        if (i == sel) {
            break;
        }
        pAdapterInfo1 = pAdapterInfo1->Next;
        i++;
    }
    
    return pAdapterInfo1;
}

void getAdapterInfo(HWND hWnd, HWND hAdtpCmbBox) {
    DWORD dwRetVal = 0;
    UINT i;

    /* variables used to print DHCP time info */
    struct tm newtime;
    char buffer[32];
    errno_t error;

    WCHAR adapterDes[100];

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

    // 콤보박스의 내용을 초기화
    SendMessage(hAdtpCmbBox, CB_RESETCONTENT, 0, 0);

    pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {

        MessageBox(hWnd, L"Error", _T("누른 버튼"), NULL);
        return;
    }


    // Make an initial call to GetAdaptersInfo to get
    // the necessary size into the ulOutBufLen variable
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(ulOutBufLen);
        ZeroMemory(pAdapterInfo, ulOutBufLen); // 메모리 초기화
        if (pAdapterInfo == NULL) {
            MessageBox(hWnd, L"Error allocating memory needed to call GetAdaptersinfo", _T("누른 버튼"), NULL);
            return;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo; 

        while (pAdapter) {
            //printf("\tComboIndex: \t%d\n", pAdapter->ComboIndex);

            //printf("\tAdapter Name: \t%s\n", pAdapter->AdapterName);
            //printf("\tAdapter Desc: \t%s\n", pAdapter->Description);
            //printf("\tAdapter Addr: \t");
            for (i = 0; i < pAdapter->AddressLength; i++) {
                if (i == (pAdapter->AddressLength - 1))
                    printf("%.2X\n", (int)pAdapter->Address[i]);
                else
                    printf("%.2X-", (int)pAdapter->Address[i]);
            }
            //printf("\tIndex: \t%d\n", pAdapter->Index);
            //printf("\tType: \t");
            
            switch (pAdapter->Type) {
            case MIB_IF_TYPE_OTHER:
                swprintf(adapterDes, sizeof(adapterDes) / sizeof(WCHAR), L"%ls %ls", multiByteToWideChar(pAdapter->Description), L" (Other)");
                SendMessage(hAdtpCmbBox, CB_ADDSTRING, 0, (LPARAM)adapterDes);
                break;
            case MIB_IF_TYPE_ETHERNET:
                swprintf(adapterDes, sizeof(adapterDes) / sizeof(WCHAR), L"%ls %ls", multiByteToWideChar(pAdapter->Description), L" (Ethernet)");
                SendMessage(hAdtpCmbBox, CB_ADDSTRING, 0, (LPARAM)adapterDes);
                break;
            case 71:
                swprintf(adapterDes, sizeof(adapterDes) / sizeof(WCHAR), L"%ls %ls", multiByteToWideChar(pAdapter->Description), L" (Wifi)");
                SendMessage(hAdtpCmbBox, CB_ADDSTRING, 0, (LPARAM)adapterDes);
                break;
            case MIB_IF_TYPE_TOKENRING:
                swprintf(adapterDes, sizeof(adapterDes) / sizeof(WCHAR), L"%ls %ls", multiByteToWideChar(pAdapter->Description), L" (Token Ring)");
                SendMessage(hAdtpCmbBox, CB_ADDSTRING, 0, (LPARAM)adapterDes);
                break;
            case MIB_IF_TYPE_FDDI:
                swprintf(adapterDes, sizeof(adapterDes) / sizeof(WCHAR), L"%ls %ls", multiByteToWideChar(pAdapter->Description), L" (FDDI)");
                SendMessage(hAdtpCmbBox, CB_ADDSTRING, 0, (LPARAM)adapterDes);
                break;
            case MIB_IF_TYPE_PPP:
                swprintf(adapterDes, sizeof(adapterDes) / sizeof(WCHAR), L"%ls %ls", multiByteToWideChar(pAdapter->Description), L" (PPP)");
                SendMessage(hAdtpCmbBox, CB_ADDSTRING, 0, (LPARAM)adapterDes);
                break;
            case MIB_IF_TYPE_LOOPBACK:
                swprintf(adapterDes, sizeof(adapterDes) / sizeof(WCHAR), L"%ls %ls", multiByteToWideChar(pAdapter->Description), L" (Loopback)");
                SendMessage(hAdtpCmbBox, CB_ADDSTRING, 0, (LPARAM)adapterDes);
                break;
            case MIB_IF_TYPE_SLIP:
                swprintf(adapterDes, sizeof(adapterDes) / sizeof(WCHAR), L"%ls %ls", multiByteToWideChar(pAdapter->Description), L" (Slip)");
                SendMessage(hAdtpCmbBox, CB_ADDSTRING, 0, (LPARAM)adapterDes);
                break;
            default:
                swprintf(adapterDes, sizeof(adapterDes) / sizeof(WCHAR), L"%ls %ls", multiByteToWideChar(pAdapter->Description), L" (Unknown type)");
                SendMessage(hAdtpCmbBox, CB_ADDSTRING, 0, (LPARAM)adapterDes);
                break;
            }

            //printf("\tIP Address: \t%s\n", pAdapter->IpAddressList.IpAddress.String);
            //printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);
            //printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
            //printf("\t***\n");

            if (pAdapter->DhcpEnabled) {
                //printf("\tDHCP Enabled: Yes\n");
                //printf("\t  DHCP Server: \t%s\n",
                  //  pAdapter->DhcpServer.IpAddress.String);

                printf("\t  Lease Obtained: ");
                /* Display local time */
                error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
                if (error)
                    printf("Invalid Argument to _localtime32_s\n");
                else {
                    // Convert to an ASCII representation 
                    error = asctime_s(buffer, 32, &newtime);
                    if (error)
                        printf("Invalid Argument to asctime_s\n");
                    else
                        /* asctime_s returns the string terminated by \n\0 */
                        printf("%s", buffer);
                }

                printf("\t  Lease Expires:  ");
                error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
                if (error)
                    printf("Invalid Argument to _localtime32_s\n");
                else {
                    // Convert to an ASCII representation 
                    error = asctime_s(buffer, 32, &newtime);
                    if (error)
                        printf("Invalid Argument to asctime_s\n");
                    else
                        /* asctime_s returns the string terminated by \n\0 */
                        printf("%s", buffer);
                }
            }
            else
                printf("\tDHCP Enabled: No\n");

            if (pAdapter->HaveWins) {
                printf("\tHave Wins: Yes\n");
                printf("\t  Primary Wins Server:    %s\n",
                    pAdapter->PrimaryWinsServer.IpAddress.String);
                printf("\t  Secondary Wins Server:  %s\n",
                    pAdapter->SecondaryWinsServer.IpAddress.String);
            }
            else
                printf("\tHave Wins: No\n");
            pAdapter = pAdapter->Next;
            printf("\n");
        }
    }
    else {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
    }
  //if (pAdapterInfo)
        //free(pAdapterInfo);

}
// IP 문자열 . 으로 나누기
void getIpSplit(wchar_t* ip, wchar_t octets[MAX_OCTETS][MAX_OCTET_LENGTH]) {
    int i = 0;
    wchar_t* token;
    wchar_t* ip_copy = wcsdup(ip); // IP 주소의 복사본 생성

    // 점(.)을 기준으로 문자열을 분리
    token = wcstok(ip_copy, L".");
    while (token != NULL && i < MAX_OCTETS) {
        wcsncpy(octets[i], token, MAX_OCTET_LENGTH - 1); // 옥텟 복사
        octets[i][MAX_OCTET_LENGTH - 1] = L'\0'; // 널 종료
        i++;
        token = wcstok(NULL, L".");
    }

    free(ip_copy); // 동적 메모리 해제
}

void setIpCat(HWND hWnd, int idBase, wchar_t* result) {
    wchar_t octets[MAX_OCTETS][MAX_OCTET_LENGTH] = { L"0", L"0", L"0", L"0" };

    for (int i = 0; i < MAX_OCTETS; i++) {
        wchar_t buffer[MAX_OCTET_LENGTH] = { 0 };
        GetDlgItemText(hWnd, idBase + i, buffer, MAX_OCTET_LENGTH);
        wcscpy(octets[i], buffer);
    }

    // 결과 IP 주소 조합
    wcscpy(result, octets[0]);
    for (int i = 1; i < MAX_OCTETS; i++) {
        wcscat(result, L".");
        wcscat(result, octets[i]);
    }
}
// Edit Readonly 세팅 함수
void SetEditReadOnly(HWND hWnd, BOOL isReadOnly) {
    SendMessage(GetDlgItem(hWnd, IDC_IP1), EM_SETREADONLY, isReadOnly, 0);
    SendMessage(GetDlgItem(hWnd, IDC_IP2), EM_SETREADONLY, isReadOnly, 0);
    SendMessage(GetDlgItem(hWnd, IDC_IP3), EM_SETREADONLY, isReadOnly, 0);
    SendMessage(GetDlgItem(hWnd, IDC_IP4), EM_SETREADONLY, isReadOnly, 0);

    SendMessage(GetDlgItem(hWnd, IDC_SUBNET1), EM_SETREADONLY, isReadOnly, 0);
    SendMessage(GetDlgItem(hWnd, IDC_SUBNET2), EM_SETREADONLY, isReadOnly, 0);
    SendMessage(GetDlgItem(hWnd, IDC_SUBNET3), EM_SETREADONLY, isReadOnly, 0);
    SendMessage(GetDlgItem(hWnd, IDC_SUBNET4), EM_SETREADONLY, isReadOnly, 0);

    SendMessage(GetDlgItem(hWnd, IDC_GW1), EM_SETREADONLY, isReadOnly, 0);
    SendMessage(GetDlgItem(hWnd, IDC_GW2), EM_SETREADONLY, isReadOnly, 0);
    SendMessage(GetDlgItem(hWnd, IDC_GW3), EM_SETREADONLY, isReadOnly, 0);
    SendMessage(GetDlgItem(hWnd, IDC_GW4), EM_SETREADONLY, isReadOnly, 0);
}

void SetIpAddress(LPCWSTR adapterName, LPCWSTR ipAddress, LPCWSTR subnetMask, LPCWSTR gateway) {
    WCHAR command[512];
    swprintf(command, sizeof(command) / sizeof(WCHAR),
        L"netsh interface ipv4 set address name=\"%s\" static %s %s %s",
        adapterName, ipAddress, subnetMask, gateway);

    // netsh 명령어 실행
    _wsystem(command);
}

void SetDhcpSw(LPCWSTR adapterName) {
    WCHAR command[512];
    swprintf(command, sizeof(command) / sizeof(WCHAR),
        L"netsh interface ipv4 set address name=\"%s\" source=dhcp",
        adapterName);

    // netsh 명령어 실행
    _wsystem(command);
}

// 초기화 함수
void InitializeControls(HWND hWnd) {
    // EDIT 컨트롤 초기화
    SetWindowText(GetDlgItem(hWnd, IDC_IP1), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_IP2), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_IP3), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_IP4), L"");

    SetWindowText(GetDlgItem(hWnd, IDC_SUBNET1), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_SUBNET2), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_SUBNET3), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_SUBNET4), L"");

    SetWindowText(GetDlgItem(hWnd, IDC_GW1), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_GW2), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_GW3), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_GW4), L"");

    // STATIC 컨트롤 초기화
    SetWindowText(GetDlgItem(hWnd, IDC_DHCP_TXT1), L"");
    SetWindowText(GetDlgItem(hWnd, IDC_INT_NAME), L"");
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            PIP_ADAPTER_INFO pAdapterInfo2;

            if (LOWORD(wParam) == IDC_COMBOBOX && HIWORD(wParam) == CBN_SELCHANGE ) {
                // 콤보박스에서 선택 이벤트
                int selectedIndex = SendMessage(GetDlgItem(hWnd, IDC_COMBOBOX), CB_GETCURSEL, 0, 0);
                if (selectedIndex != CB_ERR) {
                    TCHAR selectedText[256];
                    SendMessage(GetDlgItem(hWnd, IDC_COMBOBOX), CB_GETLBTEXT, selectedIndex, (LPARAM)selectedText);
                    pAdapterInfo2 = getSelectAdapterInfo(selectedIndex);
                    if (pAdapterInfo2 == NULL) break;
                    int wLen = MultiByteToWideChar(CP_ACP, 0, pAdapterInfo2->IpAddressList.IpAddress.String, -1, NULL, 0);
                    wchar_t* wIpString = (wchar_t*)malloc(wLen * sizeof(wchar_t));
                    MultiByteToWideChar(CP_ACP, 0, pAdapterInfo2->IpAddressList.IpAddress.String, -1, wIpString, wLen);
                    getIpSplit(wIpString, octets);

                    SendMessage(GetDlgItem(hWnd, IDC_IP1), WM_SETTEXT, 0, (LPARAM)octets[0]);
                    SendMessage(GetDlgItem(hWnd, IDC_IP2), WM_SETTEXT, 0, (LPARAM)octets[1]);
                    SendMessage(GetDlgItem(hWnd, IDC_IP3), WM_SETTEXT, 0, (LPARAM)octets[2]);
                    SendMessage(GetDlgItem(hWnd, IDC_IP4), WM_SETTEXT, 0, (LPARAM)octets[3]);

                    wLen = MultiByteToWideChar(CP_ACP, 0, pAdapterInfo2->IpAddressList.IpMask.String, -1, NULL, 0);
                    wIpString = (wchar_t*)malloc(wLen * sizeof(wchar_t));
                    MultiByteToWideChar(CP_ACP, 0, pAdapterInfo2->IpAddressList.IpMask.String, -1, wIpString, wLen);
                    getIpSplit(wIpString, octets);

                    SendMessage(GetDlgItem(hWnd, IDC_SUBNET1), WM_SETTEXT, 0, (LPARAM)octets[0]);
                    SendMessage(GetDlgItem(hWnd, IDC_SUBNET2), WM_SETTEXT, 0, (LPARAM)octets[1]);
                    SendMessage(GetDlgItem(hWnd, IDC_SUBNET3), WM_SETTEXT, 0, (LPARAM)octets[2]);
                    SendMessage(GetDlgItem(hWnd, IDC_SUBNET4), WM_SETTEXT, 0, (LPARAM)octets[3]);

                    wLen = MultiByteToWideChar(CP_ACP, 0, pAdapterInfo2->GatewayList.IpAddress.String, -1, NULL, 0);
                    wIpString = (wchar_t*)malloc(wLen * sizeof(wchar_t));
                    MultiByteToWideChar(CP_ACP, 0, pAdapterInfo2->GatewayList.IpAddress.String, -1, wIpString, wLen);
                    getIpSplit(wIpString, octets);

                    SendMessage(GetDlgItem(hWnd, IDC_GW1), WM_SETTEXT, 0, (LPARAM)octets[0]);
                    SendMessage(GetDlgItem(hWnd, IDC_GW2), WM_SETTEXT, 0, (LPARAM)octets[1]);
                    SendMessage(GetDlgItem(hWnd, IDC_GW3), WM_SETTEXT, 0, (LPARAM)octets[2]);
                    SendMessage(GetDlgItem(hWnd, IDC_GW4), WM_SETTEXT, 0, (LPARAM)octets[3]);

                    if (pAdapterInfo2->DhcpEnabled) { // DHCP ON 이면
                        SendMessage(GetDlgItem(hWnd, IDC_DHCP_TXT1), WM_SETTEXT, 0, (LPARAM)L"YES");
                        ShowWindow(GetDlgItem(hWnd, IDC_DHCHSW_BUTTON), SW_HIDE);   // DHCP 버튼 숨김
                    }
                    else {
                        SendMessage(GetDlgItem(hWnd, IDC_DHCP_TXT1), WM_SETTEXT, 0, (LPARAM)L"NO");
                        ShowWindow(GetDlgItem(hWnd, IDC_DHCHSW_BUTTON), SW_SHOW);   // DHCP 버튼 보여짐
                    }
                    if (pAdapterInfo2->Type == 71) {
                        SendMessage(GetDlgItem(hWnd, IDC_INT_NAME), WM_SETTEXT, 0, (LPARAM)L"Wi-Fi");

                    }
                    else if (pAdapterInfo2->Type == MIB_IF_TYPE_ETHERNET) {
                        SendMessage(GetDlgItem(hWnd, IDC_INT_NAME), WM_SETTEXT, 0, (LPARAM)L"이더넷");
                    }
                }
            }
            if (LOWORD(wParam) == IDC_APPLY_BUTTON) { // 버튼 클릭 이벤트 처리
                // EDIT 컨트롤의 READONLY 상태 확인
                BOOL isReadonly1 = (GetWindowLong(GetDlgItem(hWnd, IDC_IP1), GWL_STYLE) & ES_READONLY) != 0;
                // 각 EDIT 컨트롤의 READONLY 상태 확인 후 메시지 표시
                if (isReadonly1) {
                    MessageBox(hWnd, L"입력 필드의 상태가 READONLY입니다. 수정할 수 없습니다.", L"알림", MB_OK);
                    break;
                }
                // ComboBox 다시 활성화
                HWND hComboBox = GetDlgItem(hWnd, IDC_COMBOBOX);
                EnableWindow(hComboBox, TRUE);

                wchar_t ipAddress[16] = L"";
                wchar_t subnetMask[16] = L"";
                wchar_t gateway[16] = L"";

                // IP 주소 조합
                setIpCat(hWnd, IDC_IP1, ipAddress);
                setIpCat(hWnd, IDC_SUBNET1, subnetMask);
                setIpCat(hWnd, IDC_GW1, gateway);

                //MessageBox(hWnd, ipAddress, NULL, NULL);
                //MessageBox(hWnd, subnetMask, NULL, NULL);
                //MessageBox(hWnd, gateway, NULL, NULL);
                // STATIC 컨트롤에서 텍스트를 가져올 임시 버퍼
                wchar_t buffer[256];  // 충분한 크기의 버퍼
                GetWindowText(GetDlgItem(hWnd, IDC_INT_NAME), buffer, sizeof(buffer) / sizeof(buffer[0]));

                if (wcscmp(buffer, L"Wi-Fi") == 0) {
                    SetIpAddress(L"Wi-Fi", ipAddress, subnetMask, gateway);
                }
                else if (wcscmp(buffer, L"이더넷") == 0) {
                    SetIpAddress(L"이더넷", ipAddress, subnetMask, gateway);
                }
                else {
                    MessageBox(hWnd, L"설정된 인터페이스를 확인해주세요.", NULL, NULL);
                }

                // 현재 READONLY 상태 가져오기
                BOOL isReadOnly = (GetWindowLongPtr(GetDlgItem(hWnd, IDC_IP1), GWL_STYLE) & ES_READONLY) != 0;

                if (!isReadOnly) {
                    // READONLY 상태를 토글
                    SetEditReadOnly(hWnd, !isReadOnly);

                    MessageBox(hWnd, L"저장완료되었습니다.", L"알림", MB_OK);

                }

                Sleep(5000);
                // 설정 변경된 것 새로고침
                // 어댑터 정보 가져오기
                getAdapterInfo(hWnd, hComboBox);

                // 추가 설정 초기화
                InitializeControls(hWnd);

                //MessageBox(hWnd, L"APPLY 버튼이 클릭되었습니다!", L"알림", MB_OK);
            }
            if (LOWORD(wParam) == IDC_EDIT_BUTTON) { // 버튼 클릭 이벤트 처리
                if (GetWindowTextLength(GetDlgItem(hWnd, IDC_IP1)) == 0) {
                    MessageBox(hWnd, L"인터페이스를 선택해주세요.", L"알림", MB_OK);
                    break;
                }
                // ComboBox 비활성화
                HWND hComboBox = GetDlgItem(hWnd, IDC_COMBOBOX);
                EnableWindow(hComboBox, FALSE);

                // 현재 READONLY 상태 가져오기
                BOOL isReadOnly = (GetWindowLongPtr(GetDlgItem(hWnd, IDC_IP1), GWL_STYLE) & ES_READONLY) != 0;


                // 상태 확인 메시지
                if (isReadOnly) {
                    // READONLY 상태를 토글
                    SetEditReadOnly(hWnd, !isReadOnly);

                    MessageBox(hWnd, L"쓰기 가능 상태로 변경되었습니다.", L"알림", MB_OK);
                }
            }
            if (LOWORD(wParam) == IDC_DHCHSW_BUTTON) {

                HWND hComboBox = GetDlgItem(hWnd, IDC_COMBOBOX);

                // STATIC 컨트롤에서 텍스트를 가져올 임시 버퍼
                wchar_t buffer[256];  // 충분한 크기의 버퍼
                GetWindowText(GetDlgItem(hWnd, IDC_DHCP_TXT1), buffer, sizeof(buffer) / sizeof(buffer[0]));

                if (wcscmp(buffer, L"YES") == 0) {
                    GetWindowText(GetDlgItem(hWnd, IDC_INT_NAME), buffer, sizeof(buffer) / sizeof(buffer[0]));
                    SetDhcpSw(buffer);

                }
                else if (wcscmp(buffer, L"NO") == 0) {
                    GetWindowText(GetDlgItem(hWnd, IDC_INT_NAME), buffer, sizeof(buffer) / sizeof(buffer[0]));
                    SetDhcpSw(buffer);
                }

                MessageBox(hWnd, L"DHCP로 설정되었습니다.", L"알림", MB_OK);

                Sleep(5000);
                // 설정 변경된 것 새로고침
                getAdapterInfo(hWnd, hComboBox);

                InitializeControls(hWnd);
            }
            if (LOWORD(wParam) == IDC_REFRESH_BUTTON) {
                HWND hComboBox = GetDlgItem(hWnd, IDC_COMBOBOX);
                // 설정 변경된 것 새로고침
                // 어댑터 정보 가져오기
                getAdapterInfo(hWnd, hComboBox);

                // 추가 설정 초기화
                InitializeControls(hWnd);
            }
            // 입력한 값이 255를 넘지 않는지 체크
            if (HIWORD(wParam) == EN_CHANGE) {
                int editControls[] = { IDC_IP1, IDC_IP2, IDC_IP3, IDC_IP4, IDC_SUBNET1, IDC_SUBNET2, IDC_SUBNET3, IDC_SUBNET4, IDC_GW1, IDC_GW2, IDC_GW3, IDC_GW4};

                // EN_CHANGE가 발생한 컨트롤이 배열에 있는지 확인
                for (int i = 0; i < sizeof(editControls) / sizeof(editControls[0]); i++) {
                    if (LOWORD(wParam) == editControls[i]) {
                        HWND hEdit = GetDlgItem(hWnd, editControls[i]);
                        wchar_t buffer[4];
                        GetWindowText(hEdit, buffer, 4);

                        // 입력된 값이 255를 초과하는지 확인
                        int value = _wtoi(buffer);
                        if (value > 255) {
                            SetWindowText(hEdit, L"255"); // 255 초과하면 255로 변경
                        }
                        else if (buffer[0] == L'0' && wcslen(buffer) > 1) {
                            // 0으로 시작하는 경우 앞의 0 제거
                            swprintf(buffer, 4, L"%d", value); // 숫자로 변환 후 다시 문자열로 저장
                            SetWindowText(hEdit, buffer);
                        }
                        break;
                    }
                }
            }

            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_CREATE: {

        CreateWindowEx(0, L"COMBOBOX", NULL, CBS_DROPDOWN | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
            100, 0, 500, 300, hWnd, (HMENU)IDC_COMBOBOX, GetModuleHandle(NULL), NULL);

        getAdapterInfo(hWnd, GetDlgItem(hWnd, IDC_COMBOBOX));

        // IP 주소 입력 필드 생성
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            30, 30, 40, 25, hWnd, (HMENU)IDC_IP1, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_IP1), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            80, 30, 40, 25, hWnd, (HMENU)IDC_IP2, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_IP2), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            130, 30, 40, 25, hWnd, (HMENU)IDC_IP3, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_IP3), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            180, 30, 40, 25, hWnd, (HMENU)IDC_IP4, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_IP4), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(
            0, L"STATIC", L"Current IP",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            230, 30, 200, 25,  // 위치와 크기
            hWnd, (HMENU)IDC_IP_TXT, NULL, NULL
        );


        // 서브넷 마스크 입력 필드 생성
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            30, 60, 40, 25, hWnd, (HMENU)IDC_SUBNET1, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_SUBNET1), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            80, 60, 40, 25, hWnd, (HMENU)IDC_SUBNET2, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_SUBNET2), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            130, 60, 40, 25, hWnd, (HMENU)IDC_SUBNET3, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_SUBNET3), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            180, 60, 40, 25, hWnd, (HMENU)IDC_SUBNET4, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_SUBNET4), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(
            0, L"STATIC", L"Current SUBNET MAST",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            230, 60, 200, 25,  // 위치와 크기
            hWnd, (HMENU)IDC_SUBNET_TXT, NULL, NULL
        );

        // 게이트웨이 입력 필드 생성
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            30, 90, 40, 20, hWnd, (HMENU)IDC_GW1, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_GW1), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            80, 90, 40, 20, hWnd, (HMENU)IDC_GW2, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_GW2), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            130, 90, 40, 20, hWnd, (HMENU)IDC_GW3, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_GW3), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_READONLY,
            180, 90, 40, 20, hWnd, (HMENU)IDC_GW4, NULL, NULL);
        SendMessage(GetDlgItem(hWnd, IDC_GW4), EM_SETLIMITTEXT, 3, 0);
        CreateWindowEx(
            0, L"STATIC", L"Current GATEWAY",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            230, 90, 200, 25,  // 위치와 크기
            hWnd, (HMENU)IDC_GW_TXT, NULL, NULL
        );
        CreateWindowEx(
            0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            20, 120, 200, 25,  // 위치와 크기
            hWnd, (HMENU)IDC_DHCP_TXT1, NULL, NULL
        );
        CreateWindowEx(
            0, L"STATIC", L"DHCP",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            230, 120, 200, 25,  // 위치와 크기
            hWnd, (HMENU)IDC_DHCP_TXT2, NULL, NULL
        );
        CreateWindowEx(
            0, L"STATIC", L"",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            230, 150, 200, 25,  // 위치와 크기
            hWnd, (HMENU)IDC_INT_NAME, NULL, NULL
        );
        
        // 적용 버튼 생성
        CreateWindowEx(0, L"BUTTON", L"Edit", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            160, 300, 50, 25, hWnd, (HMENU)IDC_EDIT_BUTTON, NULL, NULL);
        CreateWindowEx(0, L"BUTTON", L"Apply", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            230, 300, 50, 25, hWnd, (HMENU)IDC_APPLY_BUTTON, NULL, NULL);
        CreateWindowEx(0, L"BUTTON", L"DHCP Switching", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            300, 300, 150, 25, hWnd, (HMENU)IDC_DHCHSW_BUTTON, NULL, NULL);
        CreateWindowEx(0, L"BUTTON", L"Refresh", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            500, 300, 150, 25, hWnd, (HMENU)IDC_REFRESH_BUTTON, NULL, NULL);
        ShowWindow(GetDlgItem(hWnd, IDC_DHCHSW_BUTTON), SW_HIDE);   // DHCP 버튼 숨김
        break;
    }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

            TextOut(hdc, 100, 0, _T("현재 적용된 IP"), 9);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
