#pragma once
#if defined(_WIN32) || defined(_WIN64)
#if _MSC_VER >= 1910
#pragma execution_character_set("utf-8")
#endif // _MSC_VER >= 1910
#include <Windows.h>
const auto ENABLE_UNICODE_CONSOLE = []()
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    return 0;
}();
#endif // defined(_WIN32) || defined(_WIN64)
