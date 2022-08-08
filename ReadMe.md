# DebugInfo

DebugInfo 实现一个 windows 程序崩溃转储功能，可帮助开发者快速定位现场部署程序崩溃问题。

## 快速开始

```c++
#include "DebugInfo.h"

/*!
测试程序：
1. 生成该程序，运行该程序，非 IDE 调试器运行，会自动在程序目录生成 .dmp 后缀的文件。
2. 使用 使用 winDbg 调试工具打开 .dmp 文件
输入 !analyze –v 指令即可看到详细的分析信息

注意：需要在部署机器上需要放入符号文件 *.pdb，程序崩溃时才会导出调用堆栈详细信息。
*/
int main()
{
	// 设置异常处理钩子
	SetUnhandledExceptionFilter(ApplicationCrashHandler);
	// 异常测试
	*(int*)nullptr = 0;
	return 0;
}
```