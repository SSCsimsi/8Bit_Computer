
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
//ֻ�����Խ��漰У��d�йأ�һ�黺�棬ָ��ַ������һ�黺��
///////////////////////////////////////////////////////////////////////
//��������޸Ĺ��Ļ������������,�������λ�úܶ���ԼӴ��ֵ
#define MAX_BLOCK      10  

////////////////////////////////////////////////////////////////////////////////////////////////////
//��Ӧ��������ֵ����,���ɸ���
#define CODE_MEMORY    0
#define DATA_MEMORY    1
#define USERROW_MEMORY 2
#define ID_MEMORY      3
//���ɸ���
typedef struct STRUCOPERATORTag
{
	int     nCount;                //���޸Ļ����������ţ�����޸��˼��黺��,���趨��������
	                               //�趨ʱ�����ֲ��ɴ���MAX_BLOCK
	char    SerialRecord[512];     //���ظ�Ӧ�ó��򣬼�¼���ļ��еı����Ϣ���512�������Ǳ�׼��ASCII���ַ���
	                               //�����������Ҫ���¼��ÿ�����������Ϣ
	UINT    MemoryType[MAX_BLOCK]; //���޸Ļ��������CODE_MEMORY��DATA_MEMORY��USERROW_MEMORY��ID_MEMORY�е�һ��
	UINT    StartAddr[MAX_BLOCK];  //�ÿ黺��Ŀ�ʼ��ַ
	UINT    Len[MAX_BLOCK];        //�ÿ黺�����޸��˵ĳ��ȣ��������ģ�

}
STRUCOPERATOR,*pSTRUCOPERATOR;
///////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//���Ͻṹֻ�ǲ���ʱ��Ӧ�ó����л�����ʹ�ã��趨������ṹ��ֵ��
//ÿ�ε�����ԣ����� nCount�����������һ�����Զ�ѭ����ʾ���涨�������
//��� nCount=1,��ֻ��ʾһ�����棬��ʾMemoryType[0]��StartAddr[0];Len[0];�趨������
//�趨ֵ������ȷ���󣬸������趨��ֵ���ɳ���оƬ��Ӧλ�õ�����
////////////////////////////////////////////////////////////////////////////////////////

//оƬ��Ϣ�ṹ�����ɸ���
#define LINECOUNTER             5  //��ʾ��Ϣ����

typedef struct ICINFORMATIONTag
{
	char      IcName[50];                     //оƬ����
	int       NoteCount;                     //��ʾ��Ϣ����
	char      MemoryType[LINECOUNTER][100];  //��Ϣ����
}
ICINFORMATION,*pICINFORMATION;

//DLLҪ�����ĺ���

//�Զ�IC���кż��㺯��
GETSERIAL_API pSTRUCOPERATOR GetSerial(unsigned char * pCode,unsigned char * pEEdata,unsigned char * pUserRow,unsigned char * pID,char * pSerial,unsigned char * pUniqueKey,unsigned char * pConfig);
GETSERIAL_API pICINFORMATION GetCurrentIcInfo(void); //���ص�ǰDLL����㷨��Ӧ��оƬ�ͺ�
GETSERIAL_API int EndProgAccess(int state); //DLL������

