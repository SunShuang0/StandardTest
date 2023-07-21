#include "TestSDS.h"

int GetSelect(int nDefaultSelect, int nMaxSelect)
{
	/*
#define OPT_EXIT        -1
#define OPT_RETURN        -2
#define OPT_PREVIOUS    -3
#define OPT_NEXT		-4
	*/
	int nSel,ch;
	while(1)
	{
		ch = GETCH();
#if 0
#ifdef WIN32
		putch(ch);
#endif
#endif
		if((ch == 'e') || (ch == 'E') || (ch == 'q') || (ch == 'Q'))
		{
			PUTCH(ch);

			while((ch != '\n') && (ch != '\r')) 
				ch=GETCH();
			/*[..[E]]*/
			return OPT_EXIT;
		}
		else if((ch == 'n') || (ch == 'N') || (ch == 13) || (ch == 10))
		{
			PUTCH(ch);

			while((ch != '\n') && (ch != '\r')) 
				ch=GETCH();

			return nDefaultSelect;
		}
		else if((ch == 'P') || (ch == 'p'))
		{
			PUTCH(ch);

			while((ch != '\n') && (ch != '\r')) 
				ch=GETCH();
			/*.[...(P)]..*/
			return OPT_PREVIOUS;
		}
		else if((ch == 'r') || (ch == 'R') || (ch == 'B') || (ch == 'b'))
		{
			PUTCH(ch);

			while((ch != '\n') && (ch != '\r')) 
				ch=GETCH();

			return OPT_RETURN;
		}
		else if((ch == 'c') || (ch == 'C') )
		{
			PUTCH(ch);

			while((ch != '\n') && (ch != '\r')) 
				ch=GETCH();

			return OPT_CANCEL;
		}
		else if((ch > ('0' + nMaxSelect)) || (ch < '0'))
		{
			//边界测试
			printf("\n..........>");
			continue;
		}
		else
		{
			PUTCH(ch);

			nSel = ch - '0';
			while((ch != '\n') && (ch != '\r')) 
				ch=GETCH();

			if(nSel == 0)
			{
				return nDefaultSelect;
			}
			else
			{
				return nSel;
			}
		}
	}

	return nDefaultSelect;
}


int GetInputLength(int nDefaultLength, int nMin, int nMax)
{
	/*
#define OPT_EXIT        -1
#define OPT_RETURN        -2
#define OPT_PREVIOUS    -3
#define OPT_NEXT		-4
	*/
	int len,ch;

	ch = GETCH();

	if((ch == 'e') || (ch == 'E') || (ch == 'q') || (ch == 'Q'))
	{
		/*[退出[E]]*/
		while((ch != 13) && (ch != 10))
			ch=GETCH();

		return OPT_EXIT;
	}
	else if((ch == 'n') || (ch == 'N') || (ch == 13) || (ch == 10))
	{
		while((ch != 13) && (ch != 10))
			ch=GETCH();

		return nDefaultLength;
	}
	else if((ch == 'P') || (ch == 'p'))
	{
		/*有[上一步(P)]选项*/
		while((ch != 13) && (ch != 10))
			ch=GETCH();

		return OPT_PREVIOUS;
	}
	else if((ch == 'r') || (ch == 'R') || (ch == 'B') || (ch == 'b'))
	{
		while((ch != 13) && (ch != 10))
			ch=GETCH();

		return OPT_RETURN;
	}
	else if((ch == 'c') || (ch == 'C') )
	{
		while((ch != 13) && (ch != 10))
			ch=GETCH();

		return OPT_CANCEL;
	}

	len = 0;
	while(1)
	{
		if((ch > '9') || (ch < '0'))
		{
			ch = GETCH();
			continue;
		}

		PUTCH(ch);
		len = len * 10 + (ch - '0');

		ch = GETCH();

		if((ch == '\n') || (ch == '\r'))
			break;
	}

	if(len  == 0)
	{
		if(nDefaultLength > 0)
		{
			len = nDefaultLength;
		}
		else
		{
			len = nMin;
		}
	}
	else if(len < nMin)
	{
		if(nDefaultLength > 0)
		{
			len = nDefaultLength;
		}
		else
		{
			len = nMin;
		}
	}
	else if(len > nMax)
	{
		if(nDefaultLength > 0)
		{
			len = nDefaultLength;
		}
		else
		{
			len = nMax;
		}
	}
	else
	{
		;
	}

	return len;
}

#if 1
int GetInputIndexZone(int *pIndex1, int *pIndex2, int nMin, int nMax)
{
	/*
#define OPT_EXIT        -1
#define OPT_RETURN        -2
#define OPT_PREVIOUS    -3
#define OPT_NEXT		-4
	*/
	int ch;
	int index1 = 0;
	int index2 = 0;
	int uiFlag = 0;

	ch = GETCH();

	if((ch == 'e') || (ch == 'E') || (ch == 'q') || (ch == 'Q'))
	{
		/*[退出[E]]*/
		while((ch != 13) && (ch != 10))
			ch=GETCH();

		return OPT_EXIT;
	}
	else if((ch == 'n') || (ch == 'N') || (ch == 13) || (ch == 10))
	{
		while((ch != 13) && (ch != 10))
			ch=GETCH();

		*pIndex1 = nMin;
		*pIndex2 = nMin;

		return 0;
	}
	else if((ch == 'P') || (ch == 'p'))
	{
		/*有[上一步(P)]选项*/
		while((ch != 13) && (ch != 10))
			ch=GETCH();

		return OPT_PREVIOUS;
	}
	else if((ch == 'r') || (ch == 'R') || (ch == 'B') || (ch == 'b'))
	{
		while((ch != 13) && (ch != 10))
			ch=GETCH();

		return OPT_RETURN;
	}
	else if((ch == 'c') || (ch == 'C') )
	{
		while((ch != 13) && (ch != 10))
			ch=GETCH();

		return OPT_CANCEL;
	}

	while(1)
	{
		if((('0' <= ch) && (ch <= '9')) || ((ch == '-') && (index1 != 0)))
		{
			if((uiFlag == 0) && (ch == '-'))
			{
				uiFlag = 1;
			}

			if(uiFlag == 0)
			{
				index1 = index1 * 10 + (ch - '0');
			}

			if((uiFlag == 1) && (ch != '-'))
			{
				index2 = index2 * 10 + (ch - '0');			
			}

			PUTCH(ch);

			ch = GETCH();

			if((ch == '\n') || (ch == '\r'))
				break;
		}
		else
		{
			ch = GETCH();
			continue;
		}
	}

	if(index1  == 0)
	{
		*pIndex1 = nMin;
		*pIndex2 = nMin;
	}
	else
	{
		if(index2 <= index1)
		{
			if(index1 > nMax)
			{
				index1 = nMax;
			}

			if(index1 < nMin)
			{
				index1 = nMin;
			}

			*pIndex1 = index1;
			*pIndex2 = index1;
		}
		else
		{
			if(index2 < nMin)
			{
				index1 = nMin;
				index2 = nMin;
				*pIndex1 = index1;
				*pIndex2 = index2;
			}
			else if((index1 < nMin) && ((index2 >= nMin) && (index2 <= nMax)))
			{
				index1 = nMin;
				*pIndex1 = index1;
				*pIndex2 = index2;
			}
			else if(((index1 >= nMin) && (index1 <= nMax)) && (index2 > nMax))
			{
				index2 = nMax;
				*pIndex1 = index1;
				*pIndex2 = index2;
			}
			else if(index1 > nMax)
			{
				index1 = nMax;
				index2 = nMax;
				*pIndex1 = index1;
				*pIndex2 = index2;
			}
			else
			{
				*pIndex1 = index1;
				*pIndex2 = index2;	
			}
		}
	}

	return 0;
}
#endif

int GetString(char *str, int maxSize)
{
	/*
#define OPT_EXIT        -1
#define OPT_RETURN        -2
#define OPT_PREVIOUS    -3
#define OPT_NEXT		-4
	*/
	int ch;
	int i = 0;

	*str = '\0';

	while(1)
	{
		ch = GETCH();

		if((ch == 'n') || (ch == 'N') || (ch == 13) || (ch == 10))
		{
			while((ch != 13) && (ch != 10)) 
				ch=GETCH();

			return OPT_NEXT;
		}
		else if((ch == 'e') || (ch == 'E') || (ch == 'q') || (ch == 'Q'))
		{
			/*[退出[E]]*/
			while((ch != 13) && (ch != 10))
				ch=GETCH();

			return OPT_EXIT;
		}
		else if((ch == 'P') || (ch == 'p'))
		{
			while((ch != 13) && (ch != 10)) 
				ch=GETCH();

			return OPT_PREVIOUS;
		}
		else if((ch == 'r') || (ch == 'R') || (ch == 'B') || (ch == 'b'))
		{
			while((ch != 13) && (ch != 10))
				ch=GETCH();

			return OPT_RETURN;
		}
		else if((ch == 'c') || (ch == 'C') )
		{
			while((ch != 13) && (ch != 10))
				ch=GETCH();

			return OPT_CANCEL;
		}
		else if (ch == '\b')
		{
			continue;
		}
		else
		{
			break;
		}
	}

	PUTCH(ch);
	str[i++] = ch;
	str[i] = '\0';

	while(1)
    {
		ch = GETCH();

		if((ch == '\n') || (ch == '\r'))
		{
            break;
		}
        else if(ch == '\b')
        {
			if (i)
			{
				printf("\b \b");
				str[--i] = 0;
			}
        }
        else
        {
            PUTCH(ch);
            str[i++] = ch;
            str[i] = 0;
        }
		//if (i>=maxSize)
		//	break;
    }

	return i;
}

/*
int GetStringNoEcho(char *str, int maxSize)
{
	int res = 0;
	struct termios new_setting,init_setting;

	//get termios setting and save it 
	tcgetattr(0,&init_setting);
	new_setting=init_setting;

	new_setting.c_lflag&=~ECHO;
	tcsetattr(0,TCSANOW,&new_setting);

	res = GetString(str, maxSize);

	//restore the setting
	tcsetattr(0,TCSANOW,&init_setting);
	printf("\n");
	return res;
}
*/

#ifdef WIN32
int GetPasswd(char *buf,int maxSize)
{
    //int getch();
    int i = 0;
    char t;

    buf[0] = 0;

	while(1)
	{
		t = GETCH();

		if((t == 'e') || (t == 'E') || (t == 'q') || (t == 'Q'))
		{
			printf("*");
			buf[i++] = t;
			buf[i] = 0;

			t = GETCH();

			if((t == '\n') || (t == '\r'))
			{
				buf[--i] = 0;
				return OPT_EXIT;
			}
			else if(t == '\b')
			{
				printf("\b \b");
				buf[--i] = 0;
				continue;
			}
			else
			{
				break;
			}
		}
		else if((t == 'r') || (t == 'R') || (t == 'B') || (t == 'b'))
		{
			printf("*");
			buf[i++] = t;
			buf[i] = 0;

			t = GETCH();

			if((t == '\n') || (t == '\r'))
			{
				buf[--i] = 0;
         		return OPT_RETURN;
			}
			else if(t == '\b')
			{
				printf("\b \b");
				buf[--i] = 0;
				continue;
			}
			else
			{
				break;
			}
		}
		else if((t == 'P') || (t == 'p'))
		{
			printf("*");
			buf[i++] = t;
			buf[i] = 0;

			t = GETCH();

			if((t == '\n') || (t == '\r'))
			{
				buf[--i] = 0;
         		return OPT_PREVIOUS;
			}
			else if(t == '\b')
			{
				printf("\b \b");
				buf[--i] = 0;
				continue;
			}
			else
			{
				break;
			}
		}
		else if((t == 'c') || (t == 'C') )
		{
			printf("*");
			buf[i++] = t;
			buf[i] = 0;

			t = GETCH();

			if((t == '\n') || (t == '\r'))
			{
				buf[--i] = 0;
			    return OPT_CANCEL;
			}
			else if(t == '\b')
			{
				printf("\b \b");
				buf[--i] = 0;
				continue;
			}
			else
			{
				break;
			}
		}
		else if((t == 'n') || (t == 'N'))
		{
			printf("*");
			buf[i++] = t;
			buf[i] = 0;

			t = GETCH();

			if((t == '\n') || (t == '\r'))
			{
				buf[--i] = 0;
			    return OPT_NEXT;
			}
			else if(t == '\b')
			{
				printf("\b \b");
				buf[--i] = 0;
				continue;
			}
			else
			{
				break;
			}		
		}
		else if ((t == '\n') || (t == '\r'))
		{
			buf[i] = 0;
         	return OPT_NEXT;
		}
		else if (t == '\b')
		{
			continue;
		}
		else
		{
			break;
		}
	}

	printf("*");
	buf[i++] = t;
	buf[i] = 0;

	//t=GETCH();

	while(1)
    {
		t = GETCH();

		if((t == '\n') || (t == '\r'))
		{
            break;
		}
        else if(t == '\b')
        {
			if (i)
			{
				printf("\b \b");
				buf[--i] = 0;
			}
        }
        else
        {
            printf("*");
            buf[i++] = t;
            buf[i] = 0;
        }
		//if (i>=maxSize)
		//	break;
    }

	return i;
}
#else
int GetPasswd(char *str, int maxSize)
{
	int res = 0;
	struct termios new_setting,init_setting;

	//get termios setting and save it 
	tcgetattr(0,&init_setting);
	new_setting=init_setting;

	new_setting.c_lflag&=~ECHO;
	tcsetattr(0,TCSANOW,&new_setting);

	res = GetString(str, maxSize);

	//restore the setting
	tcsetattr(0,TCSANOW,&init_setting);
	printf("\n");
	return res;
}
#endif

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

void GetAnyKey()
{
	int ch;

	while ( (ch=getchar()) != '\n' && ch != EOF )
	{
		;
	}

	return ;
}

#if 0
char * GetErrMsg(unsigned int nErrCode, unsigned int nLangChn, char *sErrMsg)
{
	switch(nErrCode)
	{
	case SDR_OK:
		strcpy(sErrMsg, "执行正确");
		break;
	case SDR_UNKNOWERR:
	case SWR_CARD_UNKNOWERR:
		strcpy(sErrMsg, "未知错误");
		break;
	case SDR_NOTSUPPORT:
	case SWR_CARD_NOTSUPPORT:
		strcpy(sErrMsg, "不支持的接口调用");
		break;
	case SDR_COMMFAIL:
	case SWR_CARD_COMMFAIL:
		strcpy(sErrMsg, "通信错误");
		break;
	case SDR_HARDFAIL:
	case SWR_CARD_HARDFAIL:
		strcpy(sErrMsg, "硬件错误或运算模块无响应");
		break;
	case SDR_OPENDEVICE:
	case SWR_CARD_OPENDEVICE:
		strcpy(sErrMsg, "打开设备失败");
		break;
	case SDR_OPENSESSION:
	case SWR_CARD_OPENSESSION:
		strcpy(sErrMsg, "创建会话失败");
		break;
	case SDR_PARDENY:
	case SWR_CARD_PARDENY:
		strcpy(sErrMsg, "权限不满足");
		break;
	case SDR_KEYNOTEXIST:
	case SWR_CARD_KEYNOTEXIST:
		strcpy(sErrMsg, "不存在的密钥调用");
		break;
	case SDR_ALGNOTSUPPORT:
	case SWR_CARD_ALGNOTSUPPORT:
		strcpy(sErrMsg, "不支持的算法调用");
		break;
	case SDR_ALGMODNOTSUPPORT:
	case SWR_CARD_ALGMODNOTSUPPORT:
		strcpy(sErrMsg, "不支持的算法模式调用");
		break;
	case SDR_PKOPERR:
	case SWR_CARD_PKOPERR:
		strcpy(sErrMsg, "公钥运算失败");
		break;
	case SDR_SKOPERR:
	case SWR_CARD_SKOPERR:
		strcpy(sErrMsg, "私钥运算失败");
		break;
	case SDR_SIGNERR:
	case SWR_CARD_SIGNERR:
		strcpy(sErrMsg, "签名运算失败");
		break;
	case SDR_VERIFYERR:
	case SWR_CARD_VERIFYERR:
		strcpy(sErrMsg, "ECC验证签名失败");
		break;
	case SDR_SYMOPERR:
	case SWR_CARD_SYMOPERR:
		strcpy(sErrMsg, "对称算法运算失败");
		break;
	case SDR_STEPERR:
	case SWR_CARD_STEPERR:
		strcpy(sErrMsg, "多步运算步骤错误");
		break;
	case SDR_FILESIZEERR:
	case SWR_CARD_FILESIZEERR:
		strcpy(sErrMsg, "文件大小错误");
		break;
	case SDR_FILENOEXIST:
	case SWR_CARD_FILENOEXIST:
		strcpy(sErrMsg, "文件不存在");
		break;
	case SDR_FILEOFSERR:
	case SWR_CARD_FILEOFSERR:
		strcpy(sErrMsg, "文件起始位置错误");
		break;
	case SDR_KEYTYPEERR:
	case SWR_CARD_KEYTYPEERR:
		strcpy(sErrMsg, "密钥类型错误");
		break;
	case SDR_KEYERR:
	case SWR_CARD_KEYERR:
		strcpy(sErrMsg, "密钥错误或密钥存储已满");
		break;
	case SWR_INVALID_USER:
		strcpy(sErrMsg, "无效的用户名");
		break;
	case SWR_INVALID_AUTHENCODE:
		strcpy(sErrMsg, "无效的授权码");
		break;
	case SWR_PROTOCOL_VER_ERR:
		strcpy(sErrMsg, "不支持的协议版本");
		break;
	case SWR_INVALID_COMMAND:
		strcpy(sErrMsg, "错误的命令字");
		break;
	case SWR_INVALID_PARAMETERS:
		strcpy(sErrMsg, "错误的数据包格式或参数错误");
		break;
	case SWR_FILE_ALREADY_EXIST:
		strcpy(sErrMsg, "已存在同名文件");
		break;
	case SWR_CONFIG_ERR:
		strcpy(sErrMsg, "配置文件错误");
		break;
	case SWR_CARD_BUFFER_TOO_SMALL:
		strcpy(sErrMsg, "接收参数的缓存区太小");
		break;
	case SWR_CARD_DATA_PAD:
		strcpy(sErrMsg, "数据填充格式不正确");
		break;
	case SWR_CARD_DATA_SIZE:
		strcpy(sErrMsg, "明文或密文长度不符合相应的算法要求");
		break;
	case SWR_CARD_CRYPTO_NOT_INIT:
		strcpy(sErrMsg, "没有为相应的算法调用初始化函数");
		break;
	case SWR_CARD_MANAGEMENT_DENY:
	case SWR_CARD_MANAGEMENT_DENY_05:
		strcpy(sErrMsg, "管理权限不满足");
		break;
	case SWR_CARD_OPERATION_DENY:
	case SWR_CARD_OPERATION_DENY_05:
		strcpy(sErrMsg, "操作权限不满足");
		break;
	case SWR_CARD_DEVICE_STATUS_ERR:
	case SWR_CARD_DEVICE_STATUS_ERR_05:
		strcpy(sErrMsg, "当前设备状态不满足现有操作");
		break;
	case SWR_CARD_LOGIN_ERR:
	case SWR_CARD_LOGIN_ERR_05:
		strcpy(sErrMsg, "登录失败");
		break;
	case SWR_CARD_USERID_ERR:
	case SWR_CARD_USERID_ERR_05:
		strcpy(sErrMsg, "用户ID数目/号码错误");
		break;
	case SWR_CARD_PARAMENT_ERR:
	case SWR_CARD_PARAMENT_ERR_05:
		strcpy(sErrMsg, "参数错误");
		break;
	case SWR_CARD_READER_PIN_ERROR:
		strcpy(sErrMsg, "口令错误");
		break;
	case SWR_CARD_READER_NO_CARD:
		strcpy(sErrMsg, "未插卡");
		break;
	case SWR_CARD_READER_CARD_INSERT:
		strcpy(sErrMsg, "卡未插紧或卡插反");
		break;
	case SWR_CARD_READER_CARD_INSERT_TYPE:
		strcpy(sErrMsg, "插入卡类型错");
		break;
	default:
		if ((nErrCode >= SWR_CARD_READER_BASE) && (nErrCode <= (SWR_CARD_READER_BASE+0x0000FFFF)))
			strcpy(sErrMsg, "读卡器错误");
		else
			strcpy(sErrMsg, "未知错误");
	}

	return sErrMsg;
}
#endif

int FileReadValue(char *filename, char *value)
{
	FILE *fp;
	char cc;
	int i = 0;
	unsigned int rv;

	if((fp = fopen(filename, "r")) == NULL )  
	{
		return 0;
	}

	rv = fread(&cc, 1, 1,fp);
	if(rv < 1) {
		fclose(fp);
		return 0;
	}

	value[i++]=cc;

	while((!feof(fp)) && (cc > 32))
	{
		rv = fread(&cc, 1, 1,fp);
		if(rv < 1)
			break;

		value[i++]=cc;
	}

	value[i++]='\0';
	fclose(fp);
	return i;
}
