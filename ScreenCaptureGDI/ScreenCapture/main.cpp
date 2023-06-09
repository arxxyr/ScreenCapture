#include <Windows.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <iostream>
#include <string>
#include <format>
#include "MainWin.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    auto logger = spdlog::basic_logger_mt("logger", "log.log");  //todoд�ļ�����û��Ȩ�ޣ���־·�����õ�AppData��ȥ
    logger->info("start");
    MainWin mainWin(hInstance);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) == TRUE) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}