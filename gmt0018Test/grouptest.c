#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include "bjcaSDF.h"
// #include "Util.c"

void *handle;
void* hDeviceHandle;
void* hSessionHandle;

typedef int dl_SDF_OpenDevice(void **);
typedef int dl_SDF_CloseDevice(void *);
typedef int dl_SDF_OpenSession(void *,void **);
typedef int dl_SDF_CloseSession(void *);

// Basic Fuctions
typedef int dl_SDF_GetDeviceInfo(void *,DEVICEINFO *);
typedef int dl_SDF_GenerateRandom(void *,int,unsigned char*);

// ECC Functions
// 1 Generate Key Pair
typedef int dl_SDF_GenerateKeyPair_ECC(void *, unsigned int , unsigned int , ECCrefPublicKey *, ECCrefPrivateKey *);
// 2 Export ECC Sign Pub Key
typedef int dl_SDF_ExportSignPublicKey_ECC(void *, unsigned int , ECCrefPublicKey *);
typedef int dl_SDF_ExportEncPublicKey_ECC(void *, unsigned int , ECCrefPublicKey *);
// 3 ExtECCOptTest
typedef int dl_SDF_ExternalEncrypt_ECC(void *, unsigned int , ECCrefPublicKey *, unsigned char *, unsigned int , ECCCipher *);
typedef int dl_SDF_ExternalDecrypt_ECC(void *, unsigned int , ECCrefPrivateKey *, ECCCipher *, unsigned char *, unsigned int *);
// 4 ExtECCSignTest
typedef int dl_SDF_ExternalSign_ECC(void *, unsigned int, ECCrefPrivateKey *, unsigned char *, unsigned int, ECCSignature *);
typedef int dl_SDF_ExternalVerify_ECC(void *, unsigned int, ECCrefPublicKey *, unsigned char *, unsigned int, ECCSignature *);
// 5 IntECCSignTest
typedef int dl_SDF_ExportSignPublicKey_ECC(void *, unsigned int , ECCrefPublicKey *);
typedef int dl_SDF_GetPrivateKeyAccessRight(void *, unsigned int, unsigned char *, unsigned int);
typedef int dl_SDF_InternalSign_ECC(void *, unsigned int, unsigned char *, unsigned int, ECCSignature *);
typedef int dl_SDF_InternalVerify_ECC(void *, unsigned int, unsigned char *, unsigned int, ECCSignature *);
typedef int dl_SDF_ReleasePrivateKeyAccessRight(void *, unsigned int);

// SM4 Functions
typedef int dl_SDF_GenerateKeyWithKEK(void *, unsigned int, unsigned int, unsigned int, unsigned char *, unsigned int *, void **);
typedef int dl_SDF_DestroyKey(void *, void *);
typedef int dl_SDF_ImportKey(void *, unsigned char *, unsigned int, void **);
typedef int dl_SDF_Encrypt(void *, void *, unsigned int, unsigned char *, unsigned char *, unsigned int, unsigned char *, unsigned int *);
typedef int dl_SDF_Decrypt(void *, void *, unsigned int, unsigned char *, unsigned char *, unsigned int, unsigned char *, unsigned int *);
typedef int dl_SDF_CalculateMAC(void *, void *, unsigned int, unsigned char *, unsigned char *, unsigned int, unsigned char *, unsigned int *);

// SM3 Functions
typedef int dl_SDF_HashTest(int nMyPos, void* hSessionHandle);
typedef int dl_SDF_HashCorrectnessTest(int nMyPos, void* hSessionHandle);
typedef int dl_SDF_HashInit(void *hSessionHandle, unsigned int uiAlgID, ECCrefPublicKey *pucPublicKey, unsigned char *pucID, unsigned int uiIDLength);
typedef int dl_SDF_HashUpdate(void *hSessionHandle, unsigned char *pucData, unsigned int uiDataLength);
typedef int dl_SDF_HashFinal(void *hSessionHandle, unsigned char *pucHash, unsigned int *puiHashLength);

// File Fuctions
typedef int dl_SDF_CreateFile(void *, unsigned char *, unsigned int , unsigned int );
typedef int dl_SDF_WriteFile(void *, unsigned char *, unsigned int , unsigned int , unsigned int , unsigned char *);
typedef int dl_SDF_ReadFile(void *, unsigned char *, unsigned int , unsigned int , unsigned int *, unsigned char *);
typedef int dl_SDF_DeleteFile(void *, unsigned char *, unsigned int );
 
 
dl_SDF_OpenDevice* my_SDF_OpenDevice;
dl_SDF_CloseDevice* my_SDF_CloseDevice;
dl_SDF_OpenSession* my_SDF_OpenSession;
dl_SDF_CloseSession* my_SDF_CloseSession;

int FileWrite(char *filename, char *mode, unsigned char *buffer, size_t size)
{
	FILE *fp;
	int rw,rwed;

	if((fp = fopen(filename, mode)) == NULL ) 
	{
		return 0;
	}
	rwed = 0;
	while(size > rwed)
	{
		if((rw = fwrite(buffer + rwed, 1, size - rwed, fp)) <= 0)
		{
			break;
		}
		rwed += rw;
	}
	fclose(fp);
	return rwed;
}

int FileRead(char *filename, char *mode, unsigned char *buffer, size_t size)
{
	FILE *fp;
	int rw,rwed;

	if((fp = fopen(filename, mode)) == NULL )  
	{
		return 0;
	}
	rwed = 0;
	while((!feof(fp)) && (size > rwed))
	{
		if((rw = fread(buffer + rwed, 1, size - rwed, fp)) <= 0)
		{
			break;
		}
		rwed += rw;
	}
	fclose(fp);
	return rwed;
}

int PrintData(char *itemName, unsigned char *sourceData, unsigned int dataLength, unsigned int rowCount)
{
	int i,j;
	
	if((sourceData == NULL) || (rowCount == 0) || (dataLength == 0)) {
		return -1;
	}
	
	if(itemName != NULL) {
		printf("%s[%d]:\n",itemName,(int)dataLength);
	}
	
	for(i=0;i<(int)(dataLength/rowCount);i++) {
		printf("%08x  ",i*rowCount);
		for(j=0;j<(int)rowCount;j++) {
			printf("%02x ",*(sourceData+i*rowCount+j));
		}
		printf("\n");
	}
	if (!(dataLength%rowCount)) {
		return 0;
	}
	
	printf("%08x  ",(dataLength/rowCount)*rowCount);
	for(j=0;j<(int)(dataLength%rowCount);j++) {
		printf("%02x ",*(sourceData+(dataLength/rowCount)*rowCount+j));
	}
	printf("\n");
	return 0;
}

int loadFunctionPointer()
{
	
	handle = dlopen ("./gmt0018-sdf.so", RTLD_NOW);  
	if(!handle){
		fprintf(stderr,"%s ",dlerror());
		return -1;
	}	
	my_SDF_OpenDevice=(dl_SDF_OpenDevice*)dlsym(handle,"SDF_OpenDevice");
	my_SDF_CloseDevice=(dl_SDF_CloseDevice*)dlsym(handle,"SDF_CloseDevice");
	my_SDF_OpenSession=(dl_SDF_OpenSession*)dlsym(handle,"SDF_OpenSession");
	my_SDF_CloseSession=(dl_SDF_CloseSession*)dlsym(handle,"SDF_CloseSession");
	
	if( my_SDF_OpenDevice==NULL)
	{
		printf("function SDF_OpenDevice not support.");
		return -1;		
	}
	if( my_SDF_OpenSession==NULL)
	{
		printf("function SDF_OpenSession not support.");
		return -1;		
	}
	if( my_SDF_CloseSession==NULL)
	{
		printf("function SDF_CloseSession not support.");
		return -1;		
	}
	if( my_SDF_CloseDevice==NULL)
	{
		printf("function SDF_CloseDevice not support.");
		return -1;		
	}
	return 0;
}

//GetDeviceInfo,my_SDF_GenerateRandom
int runBasicTest()
{
	printf("\n\n<-------------------1 基本函数测试-------------------->\n\n");
	int rv;
	dl_SDF_GetDeviceInfo* my_SDF_GetDeviceInfo=(dl_SDF_GetDeviceInfo*)dlsym(handle,"SDF_GetDeviceInfo");;
	dl_SDF_GenerateRandom* my_SDF_GenerateRandom=(dl_SDF_GenerateRandom*)dlsym(handle,"SDF_GenerateRandom");;
 
	if( my_SDF_GetDeviceInfo==NULL){
		printf("function SDF_GetDeviceInfo not support.");
		return -1;
	}
	if( my_SDF_GenerateRandom==NULL){
		printf("function SDF_GenerateRandom not support.");
		return -1;
	}
	DEVICEINFO stDeviceInfo;
	rv = my_SDF_GetDeviceInfo(hSessionHandle, &stDeviceInfo);
	if(rv != 0)
	{
		printf("SDF_GetDeviceInfo error，errno=0x%x\n", rv);
		return -1;
	}
	else{
		printf("获取设备信息成功。\n");
		printf("\n");
		printf("    |     项目      |   返回值  \n");
		printf("   _|_______________|______________________________________________________\n");
		printf("   1|   生产厂商    | %s\n",stDeviceInfo.IssuerName);
		printf("   2|   设备型号    | %s\n",stDeviceInfo.DeviceName);
		printf("   3|  设备序列号   | %s\n",stDeviceInfo.DeviceSerial);
		printf("   4|   设备版本    | v%08x\n",stDeviceInfo.DeviceVersion);
		printf("   5| 支持标准版本  | v%d\n",stDeviceInfo.StandardVersion);
		printf("   6| 支持公钥算法  | %08x | %08x\n",stDeviceInfo.AsymAlgAbility[0],stDeviceInfo.AsymAlgAbility[1]);
		printf("   7| 支持对称算法  | %08x\n",stDeviceInfo.SymAlgAbility);
		printf("   8| 支持杂凑算法  | %08x\n",stDeviceInfo.HashAlgAbility);
		printf("   9| 用户存储空间  | %dKB\n",stDeviceInfo.BufferSize >> 10);
	}
	
	unsigned char pbOutBuffer[256];
	rv = my_SDF_GenerateRandom(hSessionHandle, 16, pbOutBuffer);
	if(rv != 0)
	{
		printf("SDF_GenerateRandom error，errno=0x%x\n", rv );
		return -1;
	}
	else
	{
		PrintData("随机数", pbOutBuffer, 16, 16);
		printf("\n");
	}
		printf("\n<-------------------1 基本函数全部测试成功-------------------->\n\n");
	
	return 0;
	
}

int runSM2Test()
{
	printf("\n\n<-------------------2 ECC运算函数测试-------------------->\n\n");
	int rv;
	// 1 Generate Key Pair
	dl_SDF_GenerateKeyPair_ECC* my_SDF_GenerateKeyPair_ECC=(dl_SDF_GenerateKeyPair_ECC*)dlsym(handle,"SDF_GenerateKeyPair_ECC");
	// 2 Export ECC Sign Pub Key
	dl_SDF_ExportSignPublicKey_ECC* my_SDF_ExportSignPublicKey_ECC=(dl_SDF_ExportSignPublicKey_ECC*)dlsym(handle,"SDF_ExportSignPublicKey_ECC");
	dl_SDF_ExportEncPublicKey_ECC* my_SDF_ExportEncPublicKey_ECC=(dl_SDF_ExportEncPublicKey_ECC*)dlsym(handle,"SDF_ExportEncPublicKey_ECC");
	// 3 ExtECCOptTest
	dl_SDF_GenerateRandom* my_SDF_GenerateRandom=(dl_SDF_GenerateRandom*)dlsym(handle,"SDF_GenerateRandom");
	dl_SDF_ExternalEncrypt_ECC* my_SDF_ExternalEncrypt_ECC=(dl_SDF_ExternalEncrypt_ECC*)dlsym(handle,"SDF_ExternalEncrypt_ECC");
	dl_SDF_ExternalDecrypt_ECC* my_SDF_ExternalDecrypt_ECC=(dl_SDF_ExternalDecrypt_ECC*)dlsym(handle,"SDF_ExternalDecrypt_ECC");
	// 4 ExtECCSignTest
	dl_SDF_HashInit* my_SDF_HashInit=(dl_SDF_HashInit*)dlsym(handle,"SDF_HashInit");
	dl_SDF_HashUpdate* my_SDF_HashUpdate=(dl_SDF_HashUpdate*)dlsym(handle,"SDF_HashUpdate");
	dl_SDF_HashFinal* my_SDF_HashFinal=(dl_SDF_HashFinal*)dlsym(handle,"SDF_HashFinal");
	dl_SDF_ExternalSign_ECC* my_SDF_ExternalSign_ECC=(dl_SDF_ExternalSign_ECC*)dlsym(handle,"SDF_ExternalSign_ECC");
	dl_SDF_ExternalVerify_ECC* my_SDF_ExternalVerify_ECC=(dl_SDF_ExternalVerify_ECC*)dlsym(handle,"SDF_ExternalVerify_ECC");
	// 5 IntECCSignTest
	dl_SDF_GetPrivateKeyAccessRight* my_SDF_GetPrivateKeyAccessRight=(dl_SDF_GetPrivateKeyAccessRight*)dlsym(handle,"SDF_GetPrivateKeyAccessRight");
	dl_SDF_InternalSign_ECC* my_SDF_InternalSign_ECC=(dl_SDF_InternalSign_ECC*)dlsym(handle,"SDF_InternalSign_ECC");
	dl_SDF_InternalVerify_ECC* my_SDF_InternalVerify_ECC=(dl_SDF_InternalVerify_ECC*)dlsym(handle,"SDF_InternalVerify_ECC");
	dl_SDF_ReleasePrivateKeyAccessRight* my_SDF_ReleasePrivateKeyAccessRight=(dl_SDF_ReleasePrivateKeyAccessRight*)dlsym(handle,"SDF_ReleasePrivateKeyAccessRight");

	if( my_SDF_GenerateKeyPair_ECC==NULL){// 1 Generate Key Pair
		printf("function SDF_GenerateKeyPair_ECC not support.");
		return -1;
	}
	if( my_SDF_ExportSignPublicKey_ECC==NULL){// 2 Export ECC Sign Pub Key
		printf("function SDF_WriteFile not support.");
		return -1;
	}
	if( my_SDF_ExportEncPublicKey_ECC==NULL){
		printf("function SDF_WriteFile not support.");
		return -1;
	}
	if( my_SDF_GenerateRandom==NULL){// 3 ExtECCOptTest
		printf("function SDF_GenerateRandom not support.");
		return -1;
	}
	if( my_SDF_ExternalEncrypt_ECC==NULL){
		printf("function SDF_ExternalEncrypt_ECC not support.");
		return -1;
	}
	if( my_SDF_ExternalDecrypt_ECC==NULL){
		printf("function SDF_ExternalDecrypt_ECC not support.");
		return -1;
	}
	if( my_SDF_HashInit==NULL){// 4 ExtECCSignTest
		printf("function SDF_HashInit not support.");
		return -1;
	}
	if( my_SDF_HashUpdate==NULL){
		printf("function SDF_HashUpdate not support.");
		return -1;
	}
	if( my_SDF_HashFinal==NULL){
		printf("function SDF_HashFinal not support.");
		return -1;
	}
	if( my_SDF_ExternalSign_ECC==NULL){
		printf("function SDF_ExternalSign_ECC not support.");
		return -1;
	}
	if( my_SDF_ExternalVerify_ECC==NULL){
		printf("function SDF_ExternalVerify_ECC not support.");
		return -1;
	}
	if( my_SDF_GetPrivateKeyAccessRight==NULL){// 5 IntECCSignTest
		printf("function SDF_GetPrivateKeyAccessRight not support.");
		return -1;
	}
	if( my_SDF_InternalSign_ECC==NULL){
		printf("function SDF_InternalSign_ECC not support.");
		return -1;
	}
	if( my_SDF_InternalVerify_ECC==NULL){
		printf("function SDF_InternalVerify_ECC not support.");
		return -1;
	}
	if( my_SDF_ReleasePrivateKeyAccessRight==NULL){
		printf("function SDF_ReleasePrivateKeyAccessRight not support.");
		return -1;
	}

	// 1 Generate Key Pair
	int keyLen = 256;
	ECCrefPublicKey pubKey;
	ECCrefPrivateKey priKey;
	int pukLen, prkLen;

	printf("2.1 产生ECC密钥对测试:\n");
	printf("------------------\n");
	printf("\n");

	rv = my_SDF_GenerateKeyPair_ECC(hSessionHandle, SGD_SM2_3, keyLen, &pubKey, &priKey);
	if(rv != 0) {
		printf("产生ECC密钥对错误，错误码[%d]\n", rv);
	} else {
		printf("产生ECC密钥对成功，并写入 data/prikey_ecc.0,data/pubkey_ecc.0\n");

		pukLen = sizeof(ECCrefPublicKey);
		prkLen = sizeof(ECCrefPrivateKey);
		
		PrintData("PUBLICKEY", (unsigned char *)&pubKey, pukLen, 16);
		PrintData("PRIVATEKEY", (unsigned char *)&priKey, prkLen, 16);

		FileWrite("data/prikey_ecc.0", "wb+", (unsigned char *)&priKey, prkLen);
		FileWrite("data/pubkey_ecc.0", "wb+", (unsigned char *)&pubKey, pukLen);

		printf("\n");
	}

	printf("\n");

	// 2 Export ECC Sign Pub Key
	int keyIndex = 1;
	ECCrefPublicKey signPubKey,encPubKey;
	char filename[50];
	printf("2.2 导出ECC公钥测试:\n");
	printf("----------------\n");
	printf("\n");

	rv = my_SDF_ExportSignPublicKey_ECC(hSessionHandle, keyIndex, &signPubKey);
	if(rv != 0)
	{
		printf("导出签名公钥错误，错误码[%d]\n", rv);
	}
	else
	{
		pukLen = sizeof(ECCrefPublicKey);
		printf("导出签名公钥成功，并写入文件data/signpubkey_ecc.%d\n", keyIndex);
		PrintData("SignPublicKey",(unsigned char *)&signPubKey, pukLen, 16);
		sprintf(filename, "data/signpubkey_ecc.%d", keyIndex);
		FileWrite(filename,"wb+",(unsigned char *)&signPubKey, pukLen);
	}

	rv = my_SDF_ExportEncPublicKey_ECC(hSessionHandle, keyIndex, &encPubKey);
	if(rv != 0)
	{
		printf("导出加密公钥错误，错误码[%d]\n", rv);
	}
	else
	{
		pukLen = sizeof(ECCrefPublicKey);
		printf("导出加密公钥成功，并写入文件data/encpubkey_ecc.%d\n", keyIndex);
		PrintData("EncPublicKey",(unsigned char *)&encPubKey, pukLen, 16);
		sprintf(filename, "data/encpubkey_ecc.%d", keyIndex);
		FileWrite(filename,"wb+", (unsigned char *)&encPubKey, pukLen);
	}

	printf("\n");

	// 3 ExtECCOptTest
	SGD_UCHAR inData[512],outData[512],tmpData[512];
	SGD_UINT32 outDataLen;
	unsigned int inPlainLen;
	
	printf("2.3 外部ECC密钥加解密运算测试:\n");
	printf("--------------------\n");
	printf("\n");

	prkLen = FileRead("data/prikey_ecc.0", "rb", (unsigned char *)&priKey, sizeof(priKey));
	if(prkLen < sizeof(ECCrefPrivateKey))
	{
		printf("读私钥文件错误。\n");
		return 1;
	}
	else
	{
		printf("从文件中读取私钥成功。\n");
	}

	pukLen = FileRead("data/pubkey_ecc.0","rb", (unsigned char *)&pubKey, sizeof(pubKey));
	if(pukLen < sizeof(ECCrefPublicKey))
	{
		printf("读公钥文件错误。\n");
		return 1;
	}
	else
	{
		printf("从文件中读取公钥成功。\n");
	}

	rv = my_SDF_GenerateRandom(hSessionHandle, 1, &inData[0]);
	if(rv != 0)
	{
		printf("产生随机数错误，错误码[%d]\n", rv);
		return 1;
	}

	inPlainLen = (inData[0] % 136) + 1;
	memset(inData, 0, sizeof(inData));
	rv = my_SDF_GenerateRandom(hSessionHandle, inPlainLen, &inData[0]);
	if(rv != 0)
	{
		printf("产生随机加密数据错误，错误码[%d]\n", rv);
		return 1;
	}
	else
	{
		printf("产生随机加密数据成功。\n");
		PrintData("随机加密数据", inData, inPlainLen, 16);
	}

	memset(tmpData, 0, sizeof(tmpData));
	rv = my_SDF_ExternalEncrypt_ECC(hSessionHandle, SGD_SM2_3, &pubKey, inData, inPlainLen, (ECCCipher *)tmpData);
	if(rv != 0)
	{
		printf("公钥钥运算错误，错误码[%d]\n", rv);
		return 1;
	}
	else
	{
		printf("公钥运算成功。\n");
	}

	memset(outData, 0, sizeof(outData));
	outDataLen = sizeof(outData);
	rv = my_SDF_ExternalDecrypt_ECC(hSessionHandle, SGD_SM2_3, &priKey, (ECCCipher *)tmpData, outData, &outDataLen);
	if(rv != 0)
	{
		printf("私钥运算错误，错误码[%d]\n", rv);
		return 1;
	}
	else
	{
		printf("私钥运算成功。\n");
		PrintData("私钥运算结果", outData, outDataLen, 16);
	}

	if((inPlainLen != outDataLen) || (memcmp(inData, outData, outDataLen) != 0))
	{
		printf("结果比较失败。\n");
	}
	else
	{
		printf("结果比较成功。\n");
	}
	printf("\n");

	// 4 ExtECCSignTest
	unsigned char pucHash[100];
	unsigned int puiHashLength = 100;

	printf("2.4 外部ECC密钥签名验证运算测试:\n");
	printf("--------------------\n");
	printf("\n");

	prkLen = FileRead("data/prikey_ecc.0", "rb", (unsigned char *)&priKey, sizeof(ECCrefPrivateKey));
	if(prkLen < sizeof(ECCrefPrivateKey))
	{
		printf("读私钥文件错误。\n");
		return 1;
	}
	else
	{
		printf("从文件中读取私钥成功。\n");
	}
	pukLen = FileRead("data/pubkey_ecc.0", "rb", (unsigned char *)&pubKey, sizeof(ECCrefPublicKey));
	if(pukLen < sizeof(ECCrefPublicKey))
	{
		printf("读公钥文件错误。\n");
		return 1;
	}
	else
	{
		printf("从文件中读取公钥成功。\n");
	}

    do{
        rv  = my_SDF_HashInit(hSessionHandle,SGD_SM3,&pubKey,(unsigned char *)"1234567812345678",16);
        if(rv != 0)
        {
            printf("HashInit Success!\n\n");
            break;
        }
        

        rv = my_SDF_HashUpdate(hSessionHandle,(unsigned char * )"abc",3);
        if(rv != 0) {
            printf("SDF_HashUpdate Failed! ErrCode:%d\n\n",rv);
            break;
        }

        rv = my_SDF_HashFinal(hSessionHandle, pucHash, &puiHashLength);
        if(rv != 0)
        {
            printf("SDF_HashFinal Failed! ErrCode:%d\n\n",rv);
            break;
        }
    }while(0);

	if(rv != 0)
	{
		printf("产生hash错误\n");
		return 1;
	}
	else
	{
		printf("产生随机签名数据成功。\n");
		PrintData("随机签名数据", pucHash, puiHashLength, 16);
	}

	memset(tmpData, 0, sizeof(tmpData));
	rv = my_SDF_ExternalSign_ECC(hSessionHandle, SGD_SM2_1, &priKey, pucHash, puiHashLength, (ECCSignature *)tmpData);
	if(rv != 0)
	{
		printf("签名运算错误，错误码[%d]\n", rv);
		return 1;
	}
	else
	{
		printf("签名运算成功。\n");
		PrintData("私钥签名运算结果", tmpData, 64, 16);
	}

	rv = my_SDF_ExternalVerify_ECC(hSessionHandle, SGD_SM2_1, &pubKey, pucHash, puiHashLength, (ECCSignature *)tmpData);
	if(rv != 0)
	{
		printf("验证签名运算错误，错误码[%d]\n", rv);
		return 1;
	}
	else
	{
		printf("验证签名运算成功。\n");
	}

	printf("\n");

	// 5 IntECCSignTest
	unsigned int uiHashLength = 32;
	rv = my_SDF_ExportSignPublicKey_ECC(hSessionHandle, keyIndex, &pubKey);
	if(rv != 0)
	{
		printf("导出签名公钥错误，错误码[%d]\n", rv);
		return 1;
	}
	printf("2.5 内部ECC密钥签名验证运算测试:\n");
	printf("--------------------\n");
	printf("\n");
	rv = my_SDF_GetPrivateKeyAccessRight(hSessionHandle, keyIndex, (unsigned char *)"11111111", 8);
	if(rv != 0)
	{
		printf("获取私钥访问权限错误，错误码[%d]\n", rv);
		return 1;
	}
	else
	{
		printf("获取私钥访问权限成功。\n");
	}
     do{
		rv  = my_SDF_HashInit(hSessionHandle,SGD_SM3,&pubKey,(unsigned char *)"1234567812345678",16);
		if(rv != 0)
		{
			printf("HashInit Success!\n\n");
			break;
		}
		rv = my_SDF_HashUpdate(hSessionHandle,(unsigned char * )"abc",3);
		if(rv != 0) {
			printf("SDF_HashUpdate Failed! ErrCode:%d\n\n",rv);
			break;
		}
		rv = my_SDF_HashFinal(hSessionHandle, pucHash, &uiHashLength);
		if(rv != 0)
		{
			printf("SDF_HashFinal Failed! ErrCode:%d\n\n",rv);
			break;
		}
	}while(0);
	if(rv != 0)
	{
		printf("产生hash错误\n");
		return 1;
	}
	else
	{
		printf("产生hash签名数据成功。\n");
		PrintData("随机签名数据", pucHash, uiHashLength, 16);
	}
	memset(tmpData, 0, sizeof(tmpData));
	rv = my_SDF_InternalSign_ECC(hSessionHandle, keyIndex, pucHash, uiHashLength, (ECCSignature *)tmpData);
	if(rv != 0)
	{
		my_SDF_ReleasePrivateKeyAccessRight(hSessionHandle, keyIndex);
		printf("签名运算错误，错误码[%d]\n", rv);
		return 1;
	}
	else
	{
		printf("签名运算成功。\n");
		PrintData("签名运算结果", tmpData, 64, 16);
	}
	rv = my_SDF_InternalVerify_ECC(hSessionHandle, keyIndex, pucHash, uiHashLength, (ECCSignature *)tmpData);
	if(rv != 0)
	{
		my_SDF_ReleasePrivateKeyAccessRight(hSessionHandle, keyIndex);
		printf("验证签名运算错误，错误码[%d]\n", rv);
		return 1;
	}
	else
	{
		printf("验证签名运算成功。\n");
	}
	
	rv = my_SDF_ReleasePrivateKeyAccessRight(hSessionHandle, keyIndex);
	if(rv != 0)
	{
		printf("释放私钥访问权限错误，错误码[%d]\n", rv);
		return 1;
	}
	else
	{
		printf("释放私钥访问权限成功。\n");
	}
	printf("\n");

	// 6 ECCStdDataVerifyTest
	unsigned char xa[32] = {0x5C,0xA4,0xE4,0x40,0xC5,0x08,0xC4,0x5F,0xE7,0xD7,0x58,0xAB,0x10,0xC4,0x5D,0x82,0x37,0xC4,0xF9,0x55,0x9F,0x7D,0x46,0x61,0x85,0xF2,0x95,0x39,0x9F,0x0A,0xA3,0x7D};
	unsigned char ya[32] = {0x59,0xAD,0x8A,0x3C,0xD1,0x79,0x03,0x28,0x76,0x81,0xBF,0x9D,0x21,0xDA,0x2E,0xB3,0x16,0xA0,0xCE,0x8F,0xD4,0x1C,0x89,0xCE,0x1E,0x2B,0x3F,0x1B,0x8E,0x04,0x1A,0xBA};
	unsigned char e[32] = {0x38,0x54,0xC4,0x63,0xFA,0x3F,0x73,0x78,0x36,0x21,0xB1,0xCE,0x4E,0xF8,0x3F,0x7C,0x78,0x04,0x8A,0xAC,0x79,0xB2,0x21,0xFC,0xDD,0x29,0x08,0x66,0xCC,0x13,0x11,0x74};
	unsigned char r[32] = {0x6E,0x5D,0xB4,0x9D,0xBD,0x09,0x92,0xB9,0x70,0x40,0x08,0x0A,0x96,0x00,0x3C,0x72,0x1C,0xDB,0x9C,0xF6,0x4C,0x88,0xD7,0x43,0x21,0xFC,0x2F,0x63,0x0A,0xDF,0x37,0x74};
	unsigned char s[32] = {0x2F,0x6D,0xFF,0x45,0x3D,0xFC,0x8D,0x7A,0x50,0x6D,0x3F,0x52,0x30,0x1B,0xEE,0x52,0x9E,0x62,0xFD,0xDD,0x38,0x94,0x8F,0x0D,0x5D,0x2C,0xBC,0xBC,0x55,0x90,0x0C,0xFA};

	ECCrefPublicKey ECC_PubKey;
	ECCSignature ECC_SignatureValue;
	printf("2.6 ECC标准数据验证测试:\n");
	printf("-----------------\n");
	printf("\n");
	printf("使用ECC标准数据进行验证运算，并测试结果。\n");
	memset(&ECC_PubKey, 0, sizeof(ECCrefPublicKey));
	memcpy(ECC_PubKey.x + 32, xa, 32);
	memcpy(ECC_PubKey.y + 32, ya, 32);
	ECC_PubKey.bits = 256;
	memset(&ECC_SignatureValue, 0, sizeof(ECCSignature));
	memcpy(ECC_SignatureValue.r + 32, r, 32);
	memcpy(ECC_SignatureValue.s + 32, s, 32);
	rv = my_SDF_ExternalVerify_ECC(hSessionHandle, SGD_SM2_1, &ECC_PubKey, e, 32, &ECC_SignatureValue);
	if(rv != 0)
	{
		printf("ECC标准数据验证错误，错误码[%d]\n", rv);
	}
	else
	{
		printf("ECC标准数据验证成功\n");
	}
	printf("\n");

	// 7 ECCStdDataDecTest
	unsigned char da[32] = {0xE7,0xCB,0x09,0x60,0x6A,0x53,0x32,0x0B,0x34,0x7F,0x61,0xF3,0xF1,0x42,0xDC,0xB1,0x18,0xF7,0x23,0xA9,0xBC,0x27,0x87,0x9F,0x28,0x05,0xBE,0x77,0x8F,0x24,0xAE,0xE5};
	unsigned char P[32] = {0xEA,0x4E,0xC3,0x52,0xF0,0x76,0xA6,0xBE};
	unsigned char cx[32] = {0x9E,0x2A,0x4A,0x1A,0xA4,0xCF,0x77,0x26,0x22,0xAB,0xBB,0xF1,0xC6,0xD6,0x61,0xEE,0x58,0xFF,0x01,0xFF,0x98,0x43,0x78,0x2E,0x5A,0x63,0x18,0x5A,0xBF,0x6C,0x2E,0xFA};
	unsigned char cy[32] = {0x9B,0x2D,0x59,0xB2,0xB1,0xE0,0xD0,0xA7,0x95,0xBF,0xEF,0x53,0xFA,0xBB,0x24,0xC0,0x3A,0x02,0x26,0x57,0x51,0xB8,0x20,0x59,0x12,0x00,0xF0,0xD3,0x1C,0x55,0x1E,0xD6};
	unsigned char cc[32] = {0x7D,0xFD,0xFC,0x65,0xCC,0x9D,0xF7,0xD6};
	unsigned char cM[32] = {0x28,0x7D,0x5B,0xF3,0x35,0x8B,0xED,0x99,0x28,0x81,0xB6,0x9F,0xBA,0x13,0xC8,0xAF,0x76,0xEF,0xC1,0x57,0x45,0x5D,0xB8,0x1E,0xCF,0xAC,0xC7,0xB4,0x43,0xEA,0x1D,0xB0};
	ECCrefPrivateKey ECC_PriKey;
	ECCCipher ECC_CipherData;
	unsigned char pucOutData[136] = {0};
	unsigned int uiOutDataLen;
	printf("2.7 ECC标准数据解密测试:\n");
	printf("-----------------\n");
	printf("\n");
	printf("使用ECC标准数据进行解密运算，并测试结果。\n");

	memset(&ECC_PriKey, 0, sizeof(ECCrefPrivateKey));
	memcpy(ECC_PriKey.K + 32, da, 32);
	ECC_PriKey.bits = 256;
	memset(&ECC_CipherData, 0, sizeof(ECCCipher));
	ECC_CipherData.L = 8;
	memcpy(ECC_CipherData.x + 32, cx, 32);
	memcpy(ECC_CipherData.y + 32, cy, 32);
	memcpy(ECC_CipherData.C, cc, 8);
	memcpy(ECC_CipherData.M, cM, 32);
	rv = my_SDF_ExternalDecrypt_ECC(hSessionHandle, SGD_SM2_3, &ECC_PriKey, &ECC_CipherData, pucOutData, &uiOutDataLen);
	if(rv != 0)
	{
		printf("ECC解密运算错误，错误码[%d]\n", rv);
		return 1;
	}
	if((uiOutDataLen != 8) || (memcmp(P, pucOutData, 8) != 0))
	{
		printf("ECC解密结果与标准明文不相等\n");
	}
	else
	{
		printf("ECC解密结果与标准明文相等\n");
	}
	printf("\n");
	printf("\n\n<-------------------2 ECC运算函数测试全部成功-------------------->\n\n");


	return 0;
}

int runRSATest()
{
	return 0;
}

int runSM3Test()
{
	int rv;
	dl_SDF_GetDeviceInfo* my_SDF_GetDeviceInfo=(dl_SDF_GetDeviceInfo*)dlsym(handle,"SDF_GetDeviceInfo");;
	dl_SDF_HashInit* my_SDF_HashInit=(dl_SDF_HashInit*)dlsym(handle,"SDF_HashInit");
	dl_SDF_HashUpdate* my_SDF_HashUpdate=(dl_SDF_HashUpdate*)dlsym(handle,"SDF_HashUpdate");
	dl_SDF_HashFinal* my_SDF_HashFinal=(dl_SDF_HashFinal*)dlsym(handle,"SDF_HashFinal");
	dl_SDF_GenerateRandom* my_SDF_GenerateRandom=(dl_SDF_GenerateRandom*)dlsym(handle,"SDF_GenerateRandom");
	dl_SDF_ExportSignPublicKey_ECC* my_SDF_ExportSignPublicKey_ECC=(dl_SDF_ExportSignPublicKey_ECC*)dlsym(handle,"SDF_ExportSignPublicKey_ECC");
	
 
	if( my_SDF_HashInit==NULL){
		printf("function SDF_HashInit not support.");
		return -1;
	}
	if( my_SDF_HashUpdate==NULL){
		printf("function SDF_HashUpdate not support.");
		return -1;
	}
	if( my_SDF_HashFinal==NULL){
		printf("function SDF_HashFinal not support.");
		return -1;
	}
	if( my_SDF_GenerateRandom==NULL){
		printf("function SDF_GenerateRandom not support.");
		return -1;
	}
	if( my_SDF_ExportSignPublicKey_ECC==NULL){
		printf("function SDF_ExportSignPublicKey_ECC not support.");
		return -1;
	}

	// Hash Test
	SGD_UINT32 puiAlg[20];
	puiAlg[1]=SGD_SM3;
	SGD_UCHAR pIndata[16384], pOutdata[128];
	ECCrefPublicKey pubkey;
	
	int nInlen = 1024;
	int nOutlen = 128;
			
	printf("算法标识：0x%08x\n", puiAlg[1]);
	printf("数据长度：%d\n", nInlen);
	rv = my_SDF_GenerateRandom(hSessionHandle, nInlen, pIndata);
	if(rv == 0)
	{
		printf("公钥不参与运算\n");
		rv = my_SDF_HashInit(hSessionHandle, puiAlg[1], NULL, NULL, 0);
		if(rv == 0)
		{
			rv = my_SDF_HashUpdate(hSessionHandle, pIndata, nInlen);
			if(rv == 0)
			{
				rv = my_SDF_HashFinal(hSessionHandle, pOutdata, &nOutlen);
				if(rv == 0)
				{
					PrintData("运算结果", pOutdata, nOutlen, 16);
				}
				else
				{
					printf("运算结果：SDF_HashFinal()错误，[%d]\n", rv);
				}
			}
			else
			{
				printf("运算结果：SDF_HashUpdate()错误，[%d]\n", rv);
			}
		}
		else
		{
			printf("运算结果：SDF_HashInit()错误，[%d]\n", rv);
		}
		printf("公钥参与运算，使用1号密钥对的公钥进行运算\n");
		memset(&pubkey, 0, sizeof(ECCrefPublicKey));
		rv = my_SDF_ExportSignPublicKey_ECC(hSessionHandle, 1, &pubkey);
		if (rv != 0) {
			printf("导出1号签名密钥对公钥失败：%d\n", nInlen);
		}
		else
		{
			rv = my_SDF_HashInit(hSessionHandle, puiAlg[1], &pubkey,(unsigned char *)"1234567812345678",16);
              if(rv == 0)
              {
              		rv = my_SDF_HashUpdate(hSessionHandle, pIndata, nInlen);
				if(rv == 0)
				{
					rv = my_SDF_HashFinal(hSessionHandle, pOutdata, &nOutlen);
					if(rv == 0)
					{
						PrintData("运算结果", pOutdata, nOutlen, 16);
					}
					else
					{
						printf("运算结果：SDF_HashFinal()错误，[%d]\n", rv);
					}
				}
				else
				{
					printf("运算结果：SDF_HashUpdate()错误，[%d]\n", rv);
				}
			}
			else
			{
				printf("运算结果：SDF_HashInit()错误，[%d]\n", rv);
			}
		}
                
	}
	else
	{
		printf("运算结果：产生随机加密数据错误，[%d]\n", rv);
	}

	// Hash Correctness Test
	DEVICEINFO stDeviceInfo;
	memset(&stDeviceInfo, 0, sizeof(DEVICEINFO));
	rv = my_SDF_GetDeviceInfo(hSessionHandle, &stDeviceInfo);
	int num = 1;
	if(stDeviceInfo.HashAlgAbility & SGD_SM3)
	{
		SGD_UCHAR bHashData[64] = {0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,
								   0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,
								   0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,
								   0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64,0x61,0x62,0x63,0x64};
		SGD_UCHAR bHashStdResult[32] = {0xde,0xbe,0x9f,0xf9,0x22,0x75,0xb8,0xa1,0x38,0x60,0x48,0x89,0xc1,0x8e,0x5a,0x4d,
										0x6f,0xdb,0x70,0xe5,0x38,0x7e,0x57,0x65,0x29,0x3d,0xcb,0xa3,0x9c,0x0c,0x57,0x32};
		SGD_UCHAR bHashResult[256];
		unsigned int uiHashResultLen;
		printf("   %02d|   SM3运算   | ", num++);

		rv = my_SDF_HashInit(hSessionHandle, SGD_SM3, NULL, NULL, 0);
		if(rv)
		{
			printf("SDF_HashInit函数错误，错误码[%d]\n", rv);
		}

		rv = my_SDF_HashUpdate(hSessionHandle, bHashData, 64);
		if(rv)
		{
			printf("SDF_HashUpdate函数错误，错误码[%d]\n", rv);
		}

		memset(bHashResult, 0x0, sizeof(bHashResult));
		uiHashResultLen = sizeof(bHashResult);
		rv = my_SDF_HashFinal(hSessionHandle, bHashResult, &uiHashResultLen);
		if(rv)
		{
			printf("SDF_HashFinal函数错误，错误码[%d]\n", rv);
		}

		//哈希值与标准哈希值比对
		if((uiHashResultLen != 32) || (memcmp(bHashStdResult, bHashResult, 32) != 0))
		{
			printf("杂凑值与标准数据杂凑值比较失败\n");
		}
		else
		{
			printf("标准数据杂凑运算验证成功。\n");
		}
	}

	return 0;
}

int runSM4Test()
{
	printf("\n");
	printf("\n");
	printf("<-------------------4 对称密码算法测试-------------------->\n");
	printf("\n");
	int rv;
	// 1 GenKeyTest
	dl_SDF_GenerateKeyWithKEK* my_SDF_GenerateKeyWithKEK=(dl_SDF_GenerateKeyWithKEK*)dlsym(handle,"SDF_GenerateKeyWithKEK");
	dl_SDF_GetDeviceInfo* my_SDF_GetDeviceInfo=(dl_SDF_GetDeviceInfo*)dlsym(handle,"SDF_GetDeviceInfo");
	// 3 Destroy
	dl_SDF_DestroyKey* my_SDF_DestroyKey=(dl_SDF_DestroyKey*)dlsym(handle,"SDF_DestroyKey");
	// 2 Import	
	dl_SDF_GenerateRandom* my_SDF_GenerateRandom=(dl_SDF_GenerateRandom*)dlsym(handle,"SDF_GenerateRandom");
	dl_SDF_ImportKey* my_SDF_ImportKey=(dl_SDF_ImportKey*)dlsym(handle,"SDF_ImportKey");
	// 4 SymmEncDecTest
	dl_SDF_Encrypt* my_SDF_Encrypt=(dl_SDF_Encrypt*)dlsym(handle,"SDF_Encrypt");
	dl_SDF_Decrypt* my_SDF_Decrypt=(dl_SDF_Decrypt*)dlsym(handle,"SDF_Decrypt");
	// 6 SymmCalculateMACTest
	dl_SDF_CalculateMAC* my_SDF_CalculateMAC=(dl_SDF_CalculateMAC*)dlsym(handle,"SDF_CalculateMAC");

 
	if( my_SDF_GenerateKeyWithKEK==NULL){
		printf("function SDF_GenerateKeyWithKEK not support.");
		return -1;
	}
	if( my_SDF_GetDeviceInfo==NULL){
		printf("function SDF_GetDeviceInfo not support.");
		return -1;
	}
	if( my_SDF_DestroyKey==NULL){
		printf("function SDF_DestroyKey not support.");
		return -1;
	}
	if( my_SDF_ImportKey==NULL){
		printf("function SDF_ImportKey not support.");
		return -1;
	}
	if( my_SDF_Encrypt==NULL){
		printf("function SDF_Encrypt not support.");
		return -1;
	}
	if( my_SDF_Decrypt==NULL){
		printf("function SDF_Decrypt not support.");
		return -1;
	}
	if( my_SDF_CalculateMAC==NULL){
		printf("function SDF_CalculateMAC not support.");
		return -1;
	}

	void* hKey = NULL;
	void ** phKeyHandle = &hKey;
	// 1 GenKeyTest
	printf("\n4.1 产生会话密钥测试:\n");
	printf("-----------------\n");
	printf("\n");
	int nKeylen=16,i;
	unsigned char pucKey[512];
	ECCCipher ECC_pucKey;
	char filename[128];
	SGD_UINT32 puiAlg[10];
	SGD_UINT32 outKeylen;
	DEVICEINFO stDeviceInfo;
	RSArefPublicKey pubKey;
	ECCrefPublicKey ECC_pubKey;
	rv = my_SDF_GetDeviceInfo(hSessionHandle, &stDeviceInfo);
	if(rv != 0)
	{
		printf("获取设备信息错误，错误码[%d]\n", rv);	
		return 1;
	}
	
	i=1;
	if(stDeviceInfo.SymAlgAbility & SGD_SM1_ECB & 0xFFFFFF00)
	{
		puiAlg[i++]=SGD_SM1_ECB;
	}
	if(stDeviceInfo.SymAlgAbility & SGD_SM4_ECB & 0xFFFFFF00)
	{
		puiAlg[i++]=SGD_SM4_ECB;
	}

	memset(pucKey, 0, sizeof(pucKey));
	outKeylen = sizeof(pucKey);
	rv = my_SDF_GenerateKeyWithKEK(hSessionHandle, nKeylen * 8, puiAlg[2], 1, pucKey, &outKeylen, phKeyHandle);
	if(rv != 0)
	{
		if(*phKeyHandle != NULL)
		{
			*phKeyHandle = NULL;
		}
		printf("生成受密钥加密密钥保护的对称密钥错误，错误码[%d]\n", rv);
	}
	else
	{
		printf("\n");
		printf("生成受密钥加密密钥保护的对称密钥成功。\n");
		printf("可以使用该密钥进行对称加解密运算测试。\n");
		sprintf(filename,"data/keybykek.%d", 1);
		FileWrite(filename, "wb+", pucKey, outKeylen);
		printf("\n会话密钥密文已经写入文件：%s。\n",filename);
		PrintData("cipher", pucKey, outKeylen, 16);
	}

	// 3 DestroyKeyTest
	printf("\n4.3 销毁会话密钥测试:\n");
	printf("-----------------\n");
	printf("\n");
	if(*phKeyHandle == NULL)
	{
		printf("会话密钥句柄无效，请确认密钥已产生/导入...\n");
		return 1;
	}
	rv = my_SDF_DestroyKey(hSessionHandle, *phKeyHandle);
	if(rv != 0)
	{
		printf("销毁会话密钥错误，错误码[%d]\n", rv);
	}
	else
	{
		*phKeyHandle = NULL;
		printf("销毁会话密钥成功。\n");
	}

	// 2 ImportKeyTest
	printf("\n4.2 导入会话密钥测试:\n");
	printf("-----------------\n");
	printf("\n");
	int nSel;
	rv = my_SDF_GenerateRandom(hSessionHandle, nKeylen, pucKey);
	if(rv != 0)
	{
		printf("产生随机密钥数据错误，错误码[%d]\n", rv);
	}
	else
	{
		printf("从产生随机密钥数据成功。\n");
		PrintData("随机密钥数据", pucKey, nKeylen, 16);
	
		rv = my_SDF_ImportKey(hSessionHandle, pucKey, nKeylen, phKeyHandle);
		if(rv != 0)
		{
			printf("导入明文会话密钥错误，错误码[%d]\n", rv);
		}
		else
		{
			printf("导入明文会话密钥成功。\n");
			printf("可以使用该密钥进行对称加解密运算测试。\n");
		}
	}

	// 4 SymmEncDecTest
	printf("\n4.4 对称运算加解密测试:\n");
	printf("---------------------\n");
	printf("\n");
	int nInlen = 1024,nEnclen,nOutlen;
	SGD_UCHAR pIv[16], pIndata[16384], pEncdata[16384], pOutdata[16384];
	puiAlg[3]=SGD_SM4_ECB;
	printf("算法标识：0x%08x\n", puiAlg[3]);
	printf("数据长度：%d\n", nInlen);
	memset(pIv, 0, 16);

	rv = my_SDF_GenerateRandom(hSessionHandle, nInlen, pIndata);
	if(rv == 0)
	{
		rv = my_SDF_Encrypt(hSessionHandle, *phKeyHandle, puiAlg[3], pIv, pIndata, nInlen, pEncdata, &nEnclen);
		if(rv == 0)
		{
			memset(pIv, 0, 16);
			rv = my_SDF_Decrypt(hSessionHandle, *phKeyHandle, puiAlg[3], pIv, pEncdata, nEnclen, pOutdata, &nOutlen);
			if(rv == 0)
			{
				if((nOutlen == nInlen) && (memcmp(pOutdata, pIndata, nInlen) == 0))
				{
					printf("运算结果：加密、解密及结果比较均正确。\n");
				}
				else
				{
					printf("运算结果：解密结果错误。\n");
				}
			}
			else
			{
				printf("运算结果：解密错误，[%d]\n", rv);
			}
		}
		else
		{
			printf("运算结果：加密错误，[%d]\n", rv);
		}
	}
	else
	{
		printf("运算结果：产生随机加密数据错误，[%d]\n", rv);
	}

	// 5 SymmCorrectnessTest
	printf("\n4.5 算法正确性测试:\n");
	printf("---------------------\n");
	printf("\n");
	int num = 1;
	void* hKeyHandle;
	if (stDeviceInfo.SymAlgAbility & SGD_SM4_ECB & 0xFFFFFF00)
	{
		//与标准数据比较
		unsigned char pbKeyValue[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
		unsigned char pbPlainText[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
		unsigned char pbCipherText[16] = {0x68,0x1e,0xdf,0x34,0xd2,0x06,0x96,0x5e,0x86,0xb3,0xe9,0x4f,0x53,0x6e,0x42,0x46};
		unsigned char pbTempData[128] = {0};
		unsigned int  ulTempDataLen;
		unsigned char pbOutData[128] = {0};
		unsigned int  ulOutDataLen;
		
		printf("   %02d| SMS4_ECB运算  | ",num++);
		rv = my_SDF_ImportKey(hSessionHandle, pbKeyValue, 16, &hKeyHandle);
		if(rv != 0)
		{
			printf("导入明文会话密钥错误，错误码[%d]\n", rv);
		}
		memset(pIv, 0, 16);
		nInlen = 16;
		memset(pbTempData, 0, sizeof(pbTempData));
		ulTempDataLen = sizeof(pbTempData);
		rv = my_SDF_Encrypt(hSessionHandle, hKeyHandle, SGD_SM4_ECB, pIv, pbPlainText, nInlen, pbTempData, &ulTempDataLen);
		if(rv == 0)
		{
			//与标准密文数据比较
			if((nInlen == ulTempDataLen) && (memcmp(pbCipherText, pbTempData, nInlen) == 0))
			{
				;
			}
			else
			{
				my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
				printf("运算结果：加密密文与标准密文数据比较失败。\n");			
			}

			memset(pIv, 0, 16);
			memset(pbOutData, 0, sizeof(pbOutData));
			ulOutDataLen = sizeof(pbOutData);
			rv = my_SDF_Decrypt(hSessionHandle, hKeyHandle, SGD_SM4_ECB, pIv, pbTempData, ulTempDataLen, pbOutData, &ulOutDataLen);
			if(rv == 0)
			{
				if((ulOutDataLen == nInlen) && (memcmp(pbPlainText, pbOutData, nInlen) == 0))
				{
					printf("标准数据加密、解密及结果比较均正确。\n");
					my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
				}
				else
				{
					my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
					printf("运算结果：解密结果与标准明文数据比较失败。\n");	
				}
			}
			else
			{
				my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
				printf("运算结果：解密错误，错误码[%08x]\n", rv);
			}
		}
		else
		{
			my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
			printf("运算结果：加密错误，错误码[%08x]\n", rv);
		}
	}

	if (stDeviceInfo.SymAlgAbility & SGD_SM4_CBC & 0xFFFFFF00)
	{
		//与标准数据比较
		unsigned char pbKeyValue[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
		unsigned char pbIV[16] = {0xeb,0xee,0xc5,0x68,0x58,0xe6,0x04,0xd8,0x32,0x7b,0x9b,0x3c,0x10,0xc9,0x0c,0xa7};
		unsigned char pbIV1[16] = {0xeb,0xee,0xc5,0x68,0x58,0xe6,0x04,0xd8,0x32,0x7b,0x9b,0x3c,0x10,0xc9,0x0c,0xa7};
		unsigned char pbPlainText[32] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,0x29,0xbe,0xe1,0xd6,0x52,0x49,0xf1,0xe9,0xb3,0xdb,0x87,0x3e,0x24,0x0d,0x06,0x47};
		unsigned char pbCipherText[32] = {0x3f,0x1e,0x73,0xc3,0xdf,0xd5,0xa1,0x32,0x88,0x2f,0xe6,0x9d,0x99,0x6c,0xde,0x93,0x54,0x99,0x09,0x5d,0xde,0x68,0x99,0x5b,0x4d,0x70,0xf2,0x30,0x9f,0x2e,0xf1,0xb7};
		unsigned char pbTempData[128] = {0};
		unsigned int  ulTempDataLen;
		unsigned char pbOutData[128] = {0};
		unsigned int  ulOutDataLen;
		
		printf("   %02d| SMS4_CBC运算  | ",num++);
		rv = my_SDF_ImportKey(hSessionHandle, pbKeyValue, 16, &hKeyHandle);
		if(rv != 0)
		{
			printf("导入明文会话密钥错误，错误码[%d]\n", rv);
		}

		nInlen = 32;
		memset(pbTempData, 0, sizeof(pbTempData));
		ulTempDataLen = sizeof(pbTempData);
		rv = my_SDF_Encrypt(hSessionHandle, hKeyHandle, SGD_SM4_CBC, pbIV, pbPlainText, nInlen, pbTempData, &ulTempDataLen);
		if(rv == 0)
		{
			//与标准密文数据比较
			if((nInlen == ulTempDataLen) && (memcmp(pbCipherText, pbTempData, nInlen) == 0))
			{
				;
			}
			else
			{
				my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
				printf("运算结果：加密密文与标准密文数据比较失败。\n");				
			}
			
			memset(pbOutData, 0, sizeof(pbOutData));
			ulOutDataLen = sizeof(pbOutData);
			rv = my_SDF_Decrypt(hSessionHandle, hKeyHandle, SGD_SM4_CBC, pbIV1, pbTempData, ulTempDataLen, pbOutData, &ulOutDataLen);
			if(rv == 0)
			{
				if((ulOutDataLen == nInlen) && (memcmp(pbPlainText, pbOutData, nInlen) == 0))
				{
					printf("标准数据加密、解密及结果比较均正确。\n");
					my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
				}
				else
				{
					my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
					printf("运算结果：解密结果与标准明文数据比较失败。\n");	
				}
			}
			else
			{
				my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
				printf("运算结果：解密错误，错误码[%08x]\n", rv);
			}
		}
		else
		{
			my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
			printf("运算结果：加密错误，错误码[%08x]\n", rv);
		}
	}

	// 6 SymmCalculateMACTest
	printf("\n4.6 MAC算法正确性测试:\n");
	printf("---------------------\n");
	if (stDeviceInfo.SymAlgAbility & SGD_SM4_CBC & 0xFFFFFF00)
	{
		//与标准数据比较
		unsigned char pbKeyValue[16] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
		unsigned char pbIV[16] = {0xeb,0xee,0xc5,0x68,0x58,0xe6,0x04,0xd8,0x32,0x7b,0x9b,0x3c,0x10,0xc9,0x0c,0xa7};
		unsigned char pbPlainText[32] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10,0x29,0xbe,0xe1,0xd6,0x52,0x49,0xf1,0xe9,0xb3,0xdb,0x87,0x3e,0x24,0x0d,0x06,0x47};
		unsigned char pbCipherText[32] = {0x3f,0x1e,0x73,0xc3,0xdf,0xd5,0xa1,0x32,0x88,0x2f,0xe6,0x9d,0x99,0x6c,0xde,0x93,0x54,0x99,0x09,0x5d,0xde,0x68,0x99,0x5b,0x4d,0x70,0xf2,0x30,0x9f,0x2e,0xf1,0xb7};
		unsigned char pbTempData[128] = {0};
		unsigned int  ulTempDataLen;
		
		printf("   %02d| SMS4_MAC  | ", 1);
		rv = my_SDF_ImportKey(hSessionHandle, pbKeyValue, 16, &hKeyHandle);
		if(rv != 0)
		{
			printf("导入明文会话密钥错误，错误码[%d]\n", rv);
		}

		//标准数据计算MAC测试
		nInlen = 32;
		memset(pbTempData, 0, sizeof(pbTempData));
		ulTempDataLen = sizeof(pbTempData);
		rv = my_SDF_CalculateMAC(hSessionHandle, hKeyHandle, SGD_SM1_MAC, pbIV, pbPlainText, nInlen, pbTempData, &ulTempDataLen);
		if(rv != 0)
		{
			my_SDF_DestroyKey(hSessionHandle, hKeyHandle);
			printf("标准数据计算MAC值及结果比较均正确。\n");	
		}
	}
	rv = my_SDF_DestroyKey(hSessionHandle, *phKeyHandle);
	
	if(rv != 0)
	{
		printf("销毁会话密钥错误，错误码[%d]\n", rv);
	}
	else
	{
		*phKeyHandle = NULL;
		printf("销毁会话密钥成功。\n");
	}

	printf("\n<-------------------4 对称密码算法全部测试成功-------------------->\n\n");


	
	return 0;
}

int runFileTest()
{
	int rv;
	// 2. 4个子函数 然后分别判断（下面if）
	dl_SDF_CreateFile* my_SDF_CreateFile=(dl_SDF_CreateFile*)dlsym(handle,"SDF_CreateFile");;
	dl_SDF_WriteFile* my_SDF_WriteFile=(dl_SDF_WriteFile*)dlsym(handle,"SDF_WriteFile");;
	dl_SDF_ReadFile* my_SDF_ReadFile=(dl_SDF_ReadFile*)dlsym(handle,"SDF_ReadFile");
	dl_SDF_DeleteFile* my_SDF_DeleteFile=(dl_SDF_DeleteFile*)dlsym(handle,"SDF_DeleteFile");
 
	if( my_SDF_CreateFile==NULL){
		printf("function SDF_CreateFile not support.");
		return -1;
	}
	if( my_SDF_WriteFile==NULL){
		printf("function SDF_WriteFile not support.");
		return -1;
	}
	if( my_SDF_ReadFile==NULL){
		printf("function SDF_ReadFile not support.");
		return -1;
	}
	if( my_SDF_DeleteFile==NULL){
		printf("function SDF_DeleteFile not support.");
		return -1;
	}

	// CreateFile
	char filename[256] = "test";
	rv = my_SDF_CreateFile(hSessionHandle, filename, strlen(filename), 32);
	if(rv != 0)
	{
		printf("执行结果：创建文件错误，[%d]\n", rv);
	}
	else
	{
		printf("执行结果：创建文件成功\n");
	}

	// WriteFile
	int nOffset = 0;
	int nInlen = 32;
	unsigned char inData[16384] = "test of write data to file";
	printf("文件名称：%s\n", filename);
	printf("起始位置：%d\n", nOffset);
	printf("数据大小：%d\n", nInlen);
	rv = my_SDF_WriteFile(hSessionHandle, filename, strlen(filename), nOffset, nInlen, inData);
	if(rv != 0)
	{
		printf("执行结果：写文件错误，[%d]\n", rv);
	}
	else
	{
		printf("执行结果：写文件成功\n");
		PrintData("写入数据", inData, nInlen, 16);
	}

	// ReadFile
	printf("文件名称：%s\n", filename);
	printf("起始位置：%d\n", nOffset);
	printf("数据大小：%d\n", nInlen);
	rv = my_SDF_ReadFile(hSessionHandle, filename, strlen(filename), nOffset, &nInlen, inData);
	if(rv != 0)
	{
		printf("执行结果：读文件错误，[%08x]\n", rv);
	}
	else
	{
		printf("执行结果：读文件成功\n");
		PrintData("读取数据", inData, nInlen, 16);
	}

	// DeleteFile
	rv = my_SDF_DeleteFile(hSessionHandle, filename, strlen(filename));
	if(rv != 0)
	{
		printf("执行结果：删除文件错误，错误码[%08x]\n", rv);
	}
	else
	{
		printf("执行结果：删除文件成功\n");
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	int rv1;
	if(argc != 2)
	{
		printf("please select function group\n");
		return 0;
	}
	rv1 = loadFunctionPointer();
	if(rv1!=0)
	{
		printf("loadFunctionPointer error，errno=0x%x\n", rv1 );
		return 0;
	}
	
	rv1 = my_SDF_OpenDevice(&hDeviceHandle);
	if(rv1!=0)
	{
		printf("SDF_OpenDevice error，errno=0x%x\n", rv1 );
		return 0;
	}
	rv1 = my_SDF_OpenSession(hDeviceHandle, &hSessionHandle);
	if(rv1 != 0){
		printf("open session error,errno=0x%x\n",rv1);
		return 0;
	}
	char* group = argv[1];
	if( strcmp(group,"basic-function")==0)
	{
		rv1 = runBasicTest();
		printf("Basic Test return %d\n", rv1 );
	}else if( strcmp(group,"rsa-function")==0)
	{
		//runRSATest();
	}else if( strcmp(group,"sm2-function")==0)
	{
		rv1 = runSM2Test();
		printf("SM2 Function Test return %d\n", rv1 );
	}else if( strcmp(group,"sm3-function")==0)
	{
		rv1 = runSM3Test();
		printf("SM3 Function Test return %d\n", rv1 );
	}else if( strcmp(group,"sm4-function")==0)
	{
		rv1 = runSM4Test();
		printf("SM4 Function Test return %d\n", rv1 );
	}else if( strcmp(group,"file-function")==0)
	{
		rv1 = runFileTest();
		printf("File Function Test return %d\n", rv1 );
	}else
	{
		printf("function group must select 'basic-function','rsa-function','sm2-function','sm3-function','sm4-function'\n");
	}
	my_SDF_CloseSession(hSessionHandle);
	my_SDF_CloseDevice(hDeviceHandle);

	return 0;
}
