// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "Nand_Ecc.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

NAND_PARAM_STRUCT NandParam;  // Courrent IC Parameter information 
							  //����һ��ȫ�ֵ�оƬ����������оƬ�Ĳ���ֵ����������
unsigned int iBlockIndex[4];  //������ʱ�ֱ���
 /***************************************************************************************************************
 * �������ܣ�ֻ�Ǵ�DLL�ļ�ʱ����һ�Σ����ڴ˽���ȫ�ֱ�����ʼ��
 ***************************************************************************************************************/

extern "C" USER_NAND_API int SetNandParam(NAND_PARAM_STRUCT * pNandParam)
{
	NandParam = *pNandParam;  // this is Chip's Params input from APP.
	//Add Init your code here

	return 0;//must return 0 , error when other value returned
}
/***************************************************************************************************************
* �������ܣ�ÿ��д��һ�����ݿ飨BLOCK��ǰ����
*
* unsigned char *pBlockBuff : Դ����ָ��  size: (NandParam.PageSize + NandParam.SpareSize)*NandParam.PagePerBlock
*       unsigned char *pBBT ��BBT �û��Զ��廵���ָ��, size: ͬ��
*unsigned int iCourrentBlock����ǰҪд��Ŀ�������   
*       unsigned int iDevice����ǰ���ú������豸�ţ�������ʱֵΪ 0-3,�������ʱʼ��Ϊ0
******************************************************************************************************************/

extern "C" USER_NAND_API int PreProcessBlock(unsigned char * pBlockBuff, unsigned char * pBBT, unsigned int iCourrentBlock, unsigned int iDevice)
{
	unsigned int iPage,i;
	unsigned char ECCCode[3];
	unsigned char * pPage;
	unsigned char * pSpare;
	iBlockIndex[iDevice&0x03] = iCourrentBlock;  //������Ҫд��Ŀ��
	//if (iCourrentBlock == 0)
	//{  //������������ڵ�0��������к� add serials code here

	//}
	pSpare = pBlockBuff;
	pSpare += NandParam.PageSize;   //ponit to spare of the  first page
	pPage = pBlockBuff;             //ponit to data of the  first page
	for (iPage = 0; iPage < NandParam.PagePerBlock; iPage++)// cal ECC of all pages
	{
		/**************************************************************************************
		*  Spare Area in NAND FLASH  Note: this example just for 512 bytes + 16 bytes
		***************************************************************************************
		*  0-4th bytes   |   5th     |       6-8th       |   9-12      |    13-15th       |
		*    reserved    | Bad Mark  | ECC0 | ECC1 |ECC2 |  Reserved   |ECC0 | ECC1 |ECC2 |
		***************************************************************************************/
		for (i = 0; i < NandParam.SpareSize; i++)pSpare[i] = 0xFF;    // set 0xFF  all of spare area

		ECCCode[0] = 0; ECCCode[1] = 0; ECCCode[2] = 0;
		NandCalECC_256BYTE(pPage, ECCCode);                           //First 256 bytes
		pSpare[6] = ECCCode[0];                                       // Save value
		pSpare[7] = ECCCode[1]; 
		pSpare[8] = ECCCode[2];

		ECCCode[0] = 0; ECCCode[1] = 0; ECCCode[2] = 0;
		NandCalECC_256BYTE((pPage+256), ECCCode);                     //Second 256 bytes
		pSpare[13] = ECCCode[0];
		pSpare[14] = ECCCode[1];
		pSpare[15] = ECCCode[2];

		pPage += (NandParam.PageSize + NandParam.SpareSize);          //Point to next page data.
		pSpare += (NandParam.PageSize + NandParam.SpareSize);         //Point to next page spare size
	}

	return 0;//must return 0 , error when other value returned
}
/***************************************************************************************************************
* �������ܣ�ÿ��д��һ�����ݿ飨BLOCK����ɺ����
*           ע�⣺��д���BBT���û��Զ��廵�����ɺ󲻻���øú���
* unsigned char *pBlockBuff : Դ����ָ��  size: (NandParam.PageSize + NandParam.SpareSize)*NandParam.PagePerBlock
*       unsigned char *pBBT ��BBT �û��Զ��廵���ָ��, size: ͬ��
*    unsigned int iBlockSave����ǰʵ��д�뵽��������
*       unsigned int iDevice����ǰ���ú������豸�ţ�������ʱֵΪ 0-3
******************************************************************************************************************/

extern "C" USER_NAND_API int ProgBlockResult(unsigned char * pBlockBuff, unsigned char * pBBT, unsigned int iBlockSave, unsigned int iDevice)
{
	//���ʵ��д��Ŀ�����������ǰ��Ҫд��Ŀ�Ų�ͬ����ʾ�����˻��飬�û����ڴ����д������
	// ÿ�ε��ã�BBT�����ݶ���ͬһ�����ݣ������̣�ÿ̨������Ӧһ�����
	if (iBlockIndex[iDevice&0x03] != iBlockSave)
	{
		// add your code here  process BBT, ���BBT��ʱ�����һ���޸ĺ������д�뵽оƬ��
	}
	return 0;//must return 0 , error when other value returned
}
/***************************************************************************************************************
* �������ܣ�ֻ��ÿ��оƬ���ǰ����һ�Σ��ɽ����û�������ʼ��
*           unsigned int iDevice����ǰ���ú������豸�ţ�������ʱֵΪ 0-3
***************************************************************************************************************/
extern "C" USER_NAND_API int Init_NandParamIc(unsigned char * pBBT, unsigned int iDevice)
{
	//����BBT���ֵ

	//Add your init code here

	return 0;//must return 0 , error when other value returned
}


