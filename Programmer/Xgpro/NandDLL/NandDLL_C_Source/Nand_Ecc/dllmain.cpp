// dllmain.cpp : 定义 DLL 应用程序的入口点。
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
							  //定义一个全局的芯片参数变量，芯片的参数值从主程序传入
unsigned int iBlockIndex[4];  //多机编程时分别处理
 /***************************************************************************************************************
 * 函数功能：只是打开DLL文件时调用一次，可在此进行全局变量初始化
 ***************************************************************************************************************/

extern "C" USER_NAND_API int SetNandParam(NAND_PARAM_STRUCT * pNandParam)
{
	NandParam = *pNandParam;  // this is Chip's Params input from APP.
	//Add Init your code here

	return 0;//must return 0 , error when other value returned
}
/***************************************************************************************************************
* 函数功能：每次写入一个数据块（BLOCK）前调用
*
* unsigned char *pBlockBuff : 源数据指针  size: (NandParam.PageSize + NandParam.SpareSize)*NandParam.PagePerBlock
*       unsigned char *pBBT ：BBT 用户自定义坏块表指针, size: 同上
*unsigned int iCourrentBlock：当前要写入的块索引号   
*       unsigned int iDevice：当前调用函数的设备号，多机编程时值为 0-3,单机编程时始终为0
******************************************************************************************************************/

extern "C" USER_NAND_API int PreProcessBlock(unsigned char * pBlockBuff, unsigned char * pBBT, unsigned int iCourrentBlock, unsigned int iDevice)
{
	unsigned int iPage,i;
	unsigned char ECCCode[3];
	unsigned char * pPage;
	unsigned char * pSpare;
	iBlockIndex[iDevice&0x03] = iCourrentBlock;  //保存想要写入的块号
	//if (iCourrentBlock == 0)
	//{  //例：如果你想在第0块放入序列号 add serials code here

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
* 函数功能：每次写入一个数据块（BLOCK）完成后调用
*           注意：当写编程BBT表（用户自定义坏块表）完成后不会调用该函数
* unsigned char *pBlockBuff : 源数据指针  size: (NandParam.PageSize + NandParam.SpareSize)*NandParam.PagePerBlock
*       unsigned char *pBBT ：BBT 用户自定义坏块表指针, size: 同上
*    unsigned int iBlockSave：当前实际写入到块索引号
*       unsigned int iDevice：当前调用函数的设备号，多机编程时值为 0-3
******************************************************************************************************************/

extern "C" USER_NAND_API int ProgBlockResult(unsigned char * pBlockBuff, unsigned char * pBBT, unsigned int iBlockSave, unsigned int iDevice)
{
	//如果实际写入的块索引号与编程前想要写入的块号不同，表示跳过了坏块，用户可在此自行处理坏块表
	// 每次调用，BBT表内容都是同一块数据（多机编程，每台机机对应一块表），
	if (iBlockIndex[iDevice&0x03] != iBlockSave)
	{
		// add your code here  process BBT, 编程BBT表时，最后一次修改后的内容写入到芯片中
	}
	return 0;//must return 0 , error when other value returned
}
/***************************************************************************************************************
* 函数功能：只是每个芯片编程前调用一次，可进行用户变量初始化
*           unsigned int iDevice：当前调用函数的设备号，多机编程时值为 0-3
***************************************************************************************************************/
extern "C" USER_NAND_API int Init_NandParamIc(unsigned char * pBBT, unsigned int iDevice)
{
	//设置BBT表初值

	//Add your init code here

	return 0;//must return 0 , error when other value returned
}


