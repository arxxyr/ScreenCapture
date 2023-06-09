#include <Windows.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <iostream>
#include <string>
#include <format>
#include "SDL.h"
#include "MainWin.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    auto logger = spdlog::basic_logger_mt("logger", "log.log");  //todoд�ļ�����û��Ȩ�ޣ���־·�����õ�AppData��ȥ
    logger->info("start");
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        logger->info(std::format("SDL_Init Error {0}", SDL_GetError()));
        return 0;
    }

    {
        MainWin mainWin;
        mainWin.Start();
    }
    SDL_Quit();
    return 0;   
}