// DebugInfo.cpp: 定义应用程序的入口点。
//

#include "DebugInfo.h"
#include <stdio.h>

StackTrace& StackTrace::instance() {
	static StackTrace object;
	return object;
}

inline void StackTrace::print(const std::string& filename) {
	// open file
	std::fstream fs(filename, std::ios::out | std::ios::trunc);
	if (!fs.is_open()) {
		std::cout << "is open" << std::endl;
		fs.close();
		return;
	}

	constexpr DWORD TRACE_MAX_STACK_FRAMES = 1024;
	constexpr DWORD TRACE_MAX_FUNCTION_NAME_LENGTH = 1024;

	// get stack trace
	VOID* BackTrace[TRACE_MAX_STACK_FRAMES]{ 0 };
	DWORD BackTraceHash{ 0 };
	auto numberOfFrames = ::CaptureStackBackTrace(0, TRACE_MAX_STACK_FRAMES, BackTrace, &BackTraceHash);

	char buf[sizeof(SYMBOL_INFO) + (TRACE_MAX_FUNCTION_NAME_LENGTH - 1) * sizeof(TCHAR)]{ 0 };
	SYMBOL_INFO* symbol = (SYMBOL_INFO*)buf;
	symbol->MaxNameLen = TRACE_MAX_FUNCTION_NAME_LENGTH;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	DWORD displacement;
	IMAGEHLP_LINE64 line;
	line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	auto outputstr = [](SYMBOL_INFO* symbol, IMAGEHLP_LINE64* line) -> std::string {
		std::stringstream str;
		str << "\tat " << symbol->Name << " in " << line->FileName << " line:" << line->LineNumber << " address:0x" << std::hex << (UINT64_C(symbol->Address & 0xffffffff)) << "\n";
		return str.str();
	};

	// export info
	for (auto i = 0; i < numberOfFrames; i++) {
		DWORD64 address = (DWORD64)(BackTrace[i]);
		SymFromAddr(mProcess, address, NULL, symbol);
		if (SymGetLineFromAddr64(mProcess, address, &displacement, &line)) {
			fs << outputstr(symbol, &line);
		} else {
			std::stringstream str;
			str << "\tSymGetLineFromAddr64 returned error code " << GetLastError() << " at " << symbol->Name << " address:0x" << std::hex << symbol->Address << "\n";
			fs << str.str();
		}
	}

	fs.close();
}

inline StackTrace::StackTrace() {
	mProcess = GetCurrentProcess();
	// load pdb
	::SymInitialize(mProcess, NULL, TRUE);
}

inline StackTrace::~StackTrace() {
	::SymCleanup(mProcess);
}

/****************************************************************************************************************************************************************************/

/*!
	获取当前系统时间
	0000.00.00.00.00.00.000
*/
static std::string GetCurrentDateTime() {
	SYSTEMTIME syst;
	GetLocalTime(&syst);
	int sz = snprintf(NULL, 0, ".%.4d.%.2d.%.2d.%.2d.%.2d.%.2d.%.3d.dmp", syst.wYear, syst.wMonth, syst.wDay, syst.wHour, syst.wMinute, syst.wSecond, syst.wMilliseconds);
	std::string strDateTime(sz + 1, 0);
	sprintf_s(strDateTime.data(), strDateTime.length(), ".%.4d.%.2d.%.2d.%.2d.%.2d.%.2d.%.3d.dmp", syst.wYear, syst.wMonth, syst.wDay, syst.wHour, syst.wMinute, syst.wSecond, syst.wMilliseconds);
	return strDateTime;
}

/*!
	获取当前模块文件完整路径
*/
static std::string GetCurrentModuleName() {
	CHAR szFilename[MAX_PATH]{ 0 };
	GetModuleFileNameA(NULL, szFilename, MAX_PATH);
	return std::string(szFilename);
}

static void CreateDumpFile(LPCSTR lpstrDumpFilePathName, EXCEPTION_POINTERS* pException) {
	// 创建Dump文件
	HANDLE hDumpFile = CreateFileA(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// Dump信息
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	// 写入Dump文件内容
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException) {
	auto moduleName = GetCurrentModuleName();
	auto dateTime = GetCurrentDateTime();

	// 输出文件名称
	std::string outFilenameDMP = moduleName + dateTime;
	std::string outFilenameStack = ".dmp.stack";

	MakeSureDirectoryPathExists(outFilenameDMP.c_str());

	StackTrace::instance().print(outFilenameStack);
	CreateDumpFile(outFilenameDMP.c_str(), pException);

	FatalAppExitA(NULL, "*** Unhandled Exception! ***\0");

	return EXCEPTION_EXECUTE_HANDLER;
}