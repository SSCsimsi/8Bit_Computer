
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GETSERIAL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GETSERIAL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef GETSERIAL_EXPORTS
#define GETSERIAL_API __declspec(dllexport)
#else
#define GETSERIAL_API __declspec(dllimport)
#endif
///////////////////////////////////////////////////////////////////////
//只跟测试界面及校验d有关，一块缓存，指地址连续的一块缓存
///////////////////////////////////////////////////////////////////////
//定义最大修改过的缓冲区域块数据,如果你编号位置很多可以加大此值
#define MAX_BLOCK      10  

////////////////////////////////////////////////////////////////////////////////////////////////////
//对应缓冲区块值定义,不可更改
#define CODE_MEMORY    0
#define DATA_MEMORY    1
#define USERROW_MEMORY 2
#define ID_MEMORY      3
//不可更改
typedef struct STRUCOPERATORTag
{
	int     nCount;                //（修改缓冲块的索引号）编号修改了几块缓冲,就设定该数量。
	                               //设定时，数字不可大于MAX_BLOCK
	char    SerialRecord[512];     //返回给应用程序，记录在文件中的编号信息，最长512，必须是标准的ASCII码字符串
	                               //这个内容是你要求记录的每个编号特征信息
	UINT    MemoryType[MAX_BLOCK]; //被修改缓冲的区块CODE_MEMORY、DATA_MEMORY、USERROW_MEMORY、ID_MEMORY中的一个
	UINT    StartAddr[MAX_BLOCK];  //该块缓冲的开始地址
	UINT    Len[MAX_BLOCK];        //该块缓冲区修改了的长度，（连续的）

}
STRUCOPERATOR,*pSTRUCOPERATOR;
///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//以上结构只是测试时，应用程序切换界面使用，设定好这个结构的值后
//每次点击测试，根据 nCount定义的数量，一个个自动循环显示下面定义的区块
//如果 nCount=1,则只显示一个界面，显示MemoryType[0]，StartAddr[0];Len[0];设定的内容
//设定值必须正确无误，各区块设定的值不可超出芯片对应位置的容量
////////////////////////////////////////////////////////////////////////////////////////

//芯片信息结构，不可更改
#define LINECOUNTER             5  //提示信息条数

typedef struct ICINFORMATIONTag
{
	char      IcName[50];                     //芯片名称
	int       NoteCount;                     //提示信息条数
	char      MemoryType[LINECOUNTER][100];  //信息内容
}
ICINFORMATION,*pICINFORMATION;

//DLL要导出的函数

//自动IC序列号计算函数
GETSERIAL_API pSTRUCOPERATOR GetSerial(unsigned char * pCode,unsigned char * pEEdata,unsigned char * pUserRow,unsigned char * pID,char * pSerial,unsigned char * pUniqueKey,unsigned char * pConfig);
GETSERIAL_API pICINFORMATION GetCurrentIcInfo(void); //返回当前DLL编号算法对应的芯片型号
GETSERIAL_API int EndProgAccess(int state); //DLL错误处理

