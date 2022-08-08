// DebugInfo.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
#include <Windows.h>
#include <dbghelp.h>
#include <fstream>
#include <sstream>
#include <iostream>

#pragma comment(lib,"Dbghelp.lib")
// TODO: 在此处引用程序需要的其他标头。

/*!ref
https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-syminitialize
https://docs.microsoft.com/en-us/windows/win32/debug/capturestackbacktrace
https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/nf-dbghelp-symfromaddr
https://docs.microsoft.com/en-us/windows/win32/api/dbghelp/ns-dbghelp-symbol_info
*/
class StackTrace {
public:
	static StackTrace& instance();
	void print(const std::string& filename);
private:
	StackTrace();
	~StackTrace();
	HANDLE mProcess{ nullptr };
};

LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException);