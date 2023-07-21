/*
* file: TestSDS.h
* Desc: swcsm test tool
* Copyright (c) SWXA
*
* Version     Date        By who     Desc
* ----------  ----------  ---------  -------------------------------
* v1.5.0000   2011.00.00  ......   ! Optimize source code
* v1.6.0000   2011.08.06  ......   ! Add 12 Card Supports 01、03、05、09版密码卡一次最大传输8192字节（2048 DWORDS）数据，12版密码卡一次最大传输16384字节（4096 DWORDS）数据
* v1.7.0000   2011.08.11  ......   ! Optimize source code about FunctionTest.c file
* v2.0.0000   2012.01.10  ......   ! Add SDF_GenerateKeyWithIPK_ECC\SDF_GenerateKeyWithEPK_ECC\SDF_ImportKeyWithISK_ECC\SDF_ExchangeDigitEnvelopeBaseOnECC functions test.
* v2.1.0000   2012.02.08  ......   ! Optimize The defination of ECCCipher struct according to GuoMi standard.
* v2.2.0000   2012.02.28  ......   ! Optimize The defination of ECCCipher struct so that absolutely follow the GuoMi standard.
* v2.4.0000   2012.03.12  ......   ! Optimize the defination of SDF_ImportKeyWithKEK funcitons about param data type.
* v2.6.0000   2012.04.09  ......   ! Add 16 card supports and optimize source codes.
* v2.8.0000   2012.07.23  ......   ! Optimize source code
*/

#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
#include <process.h>
#include <windows.h>
#include <io.h>
#include <conio.h>
#else
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <termios.h>
#endif

//#include "swsds.h"
#include "bjcaSDF.h"
#include "sgd_def.h"

#ifdef WIN32
#define PUTCH(ch) _putch(ch)
#define GETCH() _getch()
#define GETANYKEY() _getch()
#define SLEEP(msec) Sleep(msec)
#define	THREAD_EXIT() _endthread() 
#define GETCURRENTTHREADID GetCurrentThreadId
#else
#define PUTCH(ch) 
#define GETANYKEY() getchar()
#define GETCH() getchar()
#define SLEEP(msec) usleep(msec*1000)
#define	THREAD_EXIT() pthread_exit(NULL)
#define GETCURRENTTHREADID (int)pthread_self
#endif

#define OPT_EXIT        -1
#define OPT_RETURN      -2
#define OPT_PREVIOUS    -3
#define OPT_NEXT		-4
#define OPT_YES 		-5
#define OPT_CANCEL		-6

#define TESTSDS_VERSION  "v2.8"		//版本控制信息
#define _NOT_TEST_KEK_  1			//压力测试时不测试KEK功能
#define _NOT_USE_RANDOME_TEST_ 1	//性能测试时使用随机数据测试
#define MAX_TEST_DATA_LENGTH     102400 //100K
/*数据类型定义*/
typedef char				SGD_CHAR;
typedef char				SGD_INT8;
typedef short				SGD_INT16;
typedef int					SGD_INT32;
typedef long long			SGD_INT64;
typedef unsigned char		SGD_UCHAR;
typedef unsigned char		SGD_UINT8;
typedef unsigned short		SGD_UINT16;
typedef unsigned int		SGD_UINT32;
typedef unsigned long long	SGD_UINT64;
typedef unsigned int		SGD_RV;
typedef void*				SGD_OBJ;
typedef int					SGD_BOOL;


extern void* hDeviceHandle;	
extern int g_nTestRepeat;

//功能测试函数声明
int BasicFuncTest(int nMyPos, int nDefaultSelect);
int CreateFileTest(int nMyPos, void* hSessionHandle);
int DeleteFileTest(int nMyPos,void* hSessionHandle);
int DestroyKeyTest(int nMyPos, void* hSessionHandle, void* *hKeyHandle);
int ECCStdDataVerifyTest(int nMyPos, void* hSessionHandle);
int ECCStdDataDecTest(int nMyPos, void* hSessionHandle);
int ECCTransEnvelopTest(int nMyPos, void* hSessionHandle);
int ECCAgreementTest(int nMyPos, void* hSessionHandle);
int ECCFuncTest(int nMyPos, int nDefaultSelect);
int ExportECCPukTest(int nMyPos, void* hSessionHandle);
int ExportRSAPukTest(int nMyPos, void* hSessionHandle);
int ExtECCOptTest(int nMyPos, void* hSessionHandle);
int ExtECCSignTest(int nMyPos, void* hSessionHandle);
int ExtRSAOptTest(int nMyPos, void* hSessionHandle);
int FileFuncTest(int nMyPos, int nDefaultSelect);
int FunctionTest(int nMyPos, int nDefaultSelect);
int GenECCKeyPairTest(int nMyPos, void* hSessionHandle);
int GenKeyTest(int nMyPos, void* hSessionHandle, void* *phKeyHandle);
int GenRandomTest(int nMyPos, void* hSessionHandle);
int GenRSAKeyPairTest(int nMyPos, void* hSessionHandle);
int GetDeviceInfoTest(int nMyPos, void* hSessionHandle);
int HashFuncTest(int nMyPos, int nDefaultSelect);
int HashTest(int nMyPos, void* hSessionHandle);
int HashCorrectnessTest(int nMyPos, void* hSessionHandle);
int ImportKeyTest(int nMyPos, void* hSessionHandle, void* *phKeyHandle);
//int InSymmEncDecTest(int nMyPos, void* hSessionHandle);
//int IntECCOptTest(int nMyPos, void* hSessionHandle);
int IntECCSignTest(int nMyPos, void* hSessionHandle);
int IntRSAOptTest(int nMyPos, void* hSessionHandle);
int ReadFileTest(int nMyPos, void* hSessionHandle);
int RSAFuncTest(int nMyPos, int nDefaultSelect);
int SymmCorrectnessTest(int nMyPos, void* hSessionHandle);
int SymmCalculateMACTest(int nMyPos, void* hSessionHandle);
int SymmEncDecTest(int nMyPos, void* hSessionHandle, void* *phKeyHandle);
int SymmFuncTest(int nMyPos, int nDefaultSelect);
int TransEnvelopTest(int nMyPos, void* hSessionHandle);
int WriteFileTest(int nMyPos, void* hSessionHandle);

//性能测试
void PerformanceTestStatFunc(void* argv);
int GenKeyPairPerformanceTest(int nMyPos);
void GenKeyPairPerformanceTestFunc(void* argv);
int SignPerformanceTest(int nMyPos);
void SignPerformanceTestFunc(void* argv);
int VerifyPerformanceTest(int nMyPos);
void VerifyPerformanceTestFunc(void* argv);
int SymmEncPerformanceTest(int nMyPos);
void SymmEncPerformanceTestFunc(void* argv);
int SymmDecPerformanceTest(int nMyPos);
void SymmDecPerformanceTestFunc(void* argv);
int HashPerformanceTest(int nMyPos);
void HashPerformanceTestFunc(void* argv);

//辅助函数声明
int FileRead(char *filename, char *mode, unsigned char *buffer, size_t size);
int FileReadValue(char *filename, char *value);
int FileWrite(char *filename, char *mode, unsigned char *buffer, size_t size);
void GetAnyKey();
int GetInputIndexZone(int *pIndex1, int *pIndex2, int nMin, int nMax);
int GetInputLength(int nDefaultLength, int nMin, int nMax);
int GetPasswd(char *buf,int maxSize);
int GetSelect(int nDefaultSelect, int nMaxSelect);
int GetString(char *str, int maxSize);
int PrintData(char *itemName, unsigned char *sourceData, unsigned int dataLength, unsigned int rowCount);
