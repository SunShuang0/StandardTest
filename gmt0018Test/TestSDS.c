#include "TestSDS.h"

void* hDeviceHandle; /*全局设备句柄*/
//unsigned int g_nTestRepeat;
//#define 0				0
int main(int argc, char *argv[])
{
	int rv;
	int nSel = 1;
    
	//连接设备
	rv = SDF_OpenDevice(&hDeviceHandle);
	if(rv != 0)
	{
		printf("打开设备错误，错误码[0x%08x]\n", rv);
		printf("\n按任意键退出...");
		GETCH();

		return rv;
	}
    else{
        printf("打开设备成功\n");
    }

	while(1)
	{
		printf("\n");
		printf("\n");
		printf("\n");
		printf("\n");
		printf("\n");
		printf("\n");
		printf("\n");
		printf("<-------------------BJCA加密机测试程序 [%s]-------------------->\n", TESTSDS_VERSION);
		printf("\n");
	    rv = FunctionTest(1, 1);

	    if((nSel == OPT_EXIT) || (rv == OPT_EXIT))
	    {
		    SDF_CloseDevice(hDeviceHandle);
            printf("关闭设备成功\n");

		    return OPT_EXIT;
	    }
	}

	return OPT_EXIT;
}