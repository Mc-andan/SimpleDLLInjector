#include <windows.h>
#include <tlhelp32.h>
#include <commdlg.h>
#include <stdio.h>
#define IDT_LISTBOX 3000
#define IDT_EDIT 3001
#define IDT_BUTTON 3001
#define IDT_BROSWER 3002
#define IDT_UN 3003
HWND hWnd;
HINSTANCE hIns;
HWND hListbox;
HWND hEdit;
char exetemp[MAX_PATH+1],*p;
int selpid=0;
OPENFILENAME ofn;
char szFile[MAX_PATH];
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_LBUTTONDOWN:
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps = {0};
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
    case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDT_LISTBOX:
                    if(HIWORD(wParam)==1)
                    {
                        memset(exetemp,0,261);
                        SendMessageA(hListbox,LB_GETTEXT,SendMessageA(hListbox,LB_GETCURSEL,0,0),(LPARAM)exetemp);
                        p=exetemp;
                        while(*p!=':'&&*p!=0)p++;
                        selpid=atoi(++p);
                        printf("你点击了：%d\n",selpid);
                    }
                    return 0;
                case IDT_BROSWER:
                    {
                        if(HIWORD(wParam)==0)
                        {
                        ZeroMemory(&ofn, sizeof(ofn));
                        ofn.lStructSize = sizeof(ofn);
                        ofn.hwndOwner = hWnd;
                        ofn.lpstrFile = szFile;
                        ofn.nMaxFile = sizeof(szFile);
                        ofn.lpstrFilter = "dll files (*.dll)\0*.dll";
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                        ofn.lpstrTitle = "浏览";
                        if(GetOpenFileName(&ofn))
                            SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)szFile);
                        }
                        return 0;
                    }
                case IDT_BUTTON:
                    {
                       if(HIWORD(wParam)==0)
                        {
                        HANDLE hRemoteProcess = OpenProcess(PROCESS_CREATE_THREAD |
                                    PROCESS_VM_OPERATION|
                                    PROCESS_VM_WRITE,
                                    FALSE, selpid);
                                    printf("%d",strlen(szFile)+1);
                        FARPROC load=GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
                        char * pszLibFileRemote = (char *) VirtualAllocEx(  hRemoteProcess,
                                                        NULL, strlen(szFile)+1,
                                                        MEM_COMMIT,
                                                        PAGE_READWRITE);
                        WriteProcessMemory(hRemoteProcess, pszLibFileRemote, (PVOID) szFile, strlen(szFile)+1, NULL);
                        CreateRemoteThread(hRemoteProcess, NULL, 0,(LPTHREAD_START_ROUTINE)load, pszLibFileRemote, 0, NULL);
                        }
                        return 0;

                    }
                case IDT_UN:
                    {
                      if(HIWORD(wParam)==0)
                        {
                        DWORD hDLL;
                        HANDLE hRemoteProcess = OpenProcess(PROCESS_CREATE_THREAD |
                                    PROCESS_VM_OPERATION|
                                    PROCESS_VM_WRITE,
                                    FALSE, selpid);
                                    printf("%d",strlen(szFile)+1);

                        char * pszLibFileRemote = (char *) VirtualAllocEx(  hRemoteProcess,
                                                        NULL, strlen(szFile)+1,
                                                        MEM_COMMIT,
                                                        PAGE_READWRITE);
                        WriteProcessMemory(hRemoteProcess, pszLibFileRemote, (PVOID) szFile, strlen(szFile)+1, NULL);
                        FARPROC load=GetProcAddress(GetModuleHandle("Kernel32"), "GetModuleHandleW");
                        HANDLE hRemoteThread=CreateRemoteThread(hRemoteProcess, NULL, 0,(LPTHREAD_START_ROUTINE)load, pszLibFileRemote, 0, NULL);
                        WaitForSingleObject(hRemoteThread,INFINITE);
                        GetExitCodeThread(hRemoteThread,&hDLL);
                        load=GetProcAddress(GetModuleHandle("Kernel32"), "FreeLibrary");
                        CreateRemoteThread(hRemoteProcess, NULL, 0,(LPTHREAD_START_ROUTINE)load,&hDLL, 0, NULL);
                        }
                        return 0;

                    }
                    return 0;

            }
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
    //对于我们不想处理的消息, 比如鼠标在窗口上移动时发出的消息
    //我们就原样传给默认的窗口消息处理函数处理, 不然应用程序会失去响应
}
LRESULT AddItem(HWND hwnd, LPSTR lpstr)  {
  return SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)lpstr);
}
HWND NewLabel(char *text,int x,int y,int w,int h)
{
    HWND temp=CreateWindowEx(0,"STATIC",text,WS_CHILD | WS_VISIBLE,x,y,w,h,hWnd,0,hIns,NULL);
    SendMessage(temp
                , WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
    return temp;
}
HWND NewButton(char *text,int IDT,int x,int y,int w,int h)
{
    HWND temp= CreateWindowEx(0,"Button",text,WS_CHILD | WS_VISIBLE|BS_DEFPUSHBUTTON,x,y,w,h,hWnd,(HMENU)IDT,hIns,NULL);
     SendMessage(temp
                , WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
    return temp;
}
HWND NewEdit(int IDT,int x,int y,int w,int h)
{
    HWND temp= CreateWindowEx(0,"Edit","",WS_CHILD | WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL,x,y,w,h,hWnd,(HMENU)IDT,hIns,NULL);
     SendMessage(temp
                , WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
    return temp;
}
HWND NewListbox(int IDT,int x,int y,int w,int h)
{
    HWND temp= CreateWindowEx(WS_EX_CLIENTEDGE,"Listbox","",WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_TABSTOP | WS_VSCROLL,x,y,w,h,hWnd,(HMENU)IDT,hIns,NULL);
     SendMessage(temp
                , WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
    return temp;
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;            //窗口类结构, 为注册窗口类作准备
    wc.cbClsExtra = 0;                //附加的类信息, 没有, 设为0
    wc.cbSize = sizeof(wc);            //WNDCLASSEX结构的大小
    wc.cbWndExtra = 0;                //窗口额外内存, 没有, 设为0
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE+1);            //窗口背景, 这里使用灰色背景
    wc.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));        //应用程序使用的鼠标类型
    wc.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));    //光标类型
    wc.hIconSm = NULL;                    //应用程序程序的小光标, 不管, 设为 NULL
    wc.hInstance = hInstance;            //应用程序程序实例句柄, 由 WinMain 函数传递过来
    wc.lpfnWndProc = WndProc;            //这个很重要, 就是当前窗口的消息处理函数, 传递 WndProc 的地址
    wc.lpszClassName = "crab_class";    //创建类时使用的类名, 可以自定义
    wc.lpszMenuName = NULL;                //菜单, 没有, 就使用 NULL
    wc.style = CS_HREDRAW | CS_VREDRAW;    //类的风格, 垂直重绘, 水平重绘
    hIns=hInstance;
    if(!RegisterClassEx(&wc))    //注册窗口类
    {
        MessageBox(NULL, "Register Class Failed!", NULL, MB_OK);
        return 1;
    }
    hWnd = CreateWindowEx(
                0,
                "crab_class",
                "螃蟹DLL注入器",
                WS_EX_OVERLAPPEDWINDOW|WS_MINIMIZEBOX|WS_SYSMENU,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                500,
                320,
                NULL,
                NULL,
                hInstance,
                NULL
                );
    if(!hWnd)
    {
        MessageBox(NULL, "CreateWindowEx Failed!", NULL, MB_OK);
        return 2;
    }
    NewLabel("螃蟹DLL注入器v1.0(2016-02-14)\n\tQQ:529493022",260,160,200,40);
    hListbox =NewListbox(IDT_LISTBOX,1, 1, 200, 300);
    NewLabel("要注入的DLL：",220,35,200,20);
    NewButton("注入",IDT_BUTTON,300,80,40,20);
    NewButton("浏览",IDT_BROSWER,430,50,40,20);
    NewButton("卸载",IDT_UN,350,80,40,20);
    hEdit= NewEdit(IDT_EDIT,220,50,200,20);


    UpdateWindow(hWnd);
    ShowWindow(hWnd, nCmdShow);
    MSG msg;


    PROCESSENTRY32 pid;
    HANDLE pids=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    Process32First(pids,&pid);
    wsprintf(pid.szExeFile,"%s:%d",pid.szExeFile,pid.th32ProcessID);
    AddItem(hListbox,pid.szExeFile);
    while(Process32Next(pids,&pid))
    {
    wsprintf(pid.szExeFile,"%s:%d",pid.szExeFile,pid.th32ProcessID);
    AddItem(hListbox,pid.szExeFile);

    }

    //进入消息循环
    BOOL bRet;
    while((bRet = GetMessage(&msg, NULL, 0, 0)) != -1)
    {
        if(bRet == 0)
        {
            break;
        }

        TranslateMessage(&msg);    //翻译消息
        DispatchMessage(&msg);    //分发消息
    }

    return msg.wParam;
}
