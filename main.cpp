#include "DebugInfo.h"

/*!
���Գ���
1. ���ɸó������иó��򣬷� IDE ���������У����Զ��ڳ���Ŀ¼���� .dmp ��׺���ļ���
2. ʹ�� ʹ�� winDbg ���Թ��ߴ� .dmp �ļ�
���� !analyze �Cv ָ��ɿ�����ϸ�ķ�����Ϣ

ע�⣺��Ҫ�ڲ����������Ҫ��������ļ� *.pdb���������ʱ�Żᵼ�����ö�ջ��ϸ��Ϣ��
*/
int main()
{
	// �����쳣������
	SetUnhandledExceptionFilter(ApplicationCrashHandler);
	// �쳣����
	*(int*)nullptr = 0;
	return 0;
}