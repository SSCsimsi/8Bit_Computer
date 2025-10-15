// GetSerial.cpp : Defines the entry point for the DLL application.
//////////////////////////////////////////////////////////////////////////////////////////////
//  DLL自定义算法使用例子 环境：WIN32 VC 6.0
///////////////////////////////////////////////////////////////////////////////////////////////
//需要修改的内容:
//1、芯片名称及提示----------------------------  IcInformation[]
//2、自定义算法--------------------------------  GETSERIAL_API int GetSerial(.....)
//3、算法返回结构的值--------------------------  PSTRUCOPERATOR 对应的值;
//   这个内容对应的值是对缓冲修改的部位传送给编程器应用程序，在测试时切换测试界面及校验和使用
//   ,设置必须准确，跟据你改变的内容，主程序进行重新计算校验和，防止电脑内存损坏等不确定因素引
//   起的数据改变，如果主程序检测到校验和错误会即刻提示错误，对批量生产时特别重要。特别是对一
//   次性的OTP ROM可防止主电脑硬件等故障而产生的废片。
////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GetSerial.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>




STRUCOPERATOR OprateResult;  //芯片操作结果

BOOL APIENTRY DllMain( HANDLE hModule, 
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
};

int TestValue=0;

///////////////////////////////////////////////////////////////////////////
//当前编程芯片的型号名称必须和菜单提示的型号相同
//可以只是菜单提示的型号其中几个连续的字符，注意字母的大小写
//编程器根据这个名称来判断，该DLL自定义文件是否合适现在选择的芯片
//////////////////////////////////////////////////////////////////////////
ICINFORMATION IcInformation=               //被编程芯片的有关信息
{
	 "PIC18F4550",                        //当前编程芯片的型号名称必须和菜单提示的型号相同
		                                   //编程器根据这个名称来判断，该DLL自定义文件是否合适现在选择的芯片
										   //可以是部分字符相同


      LINECOUNTER,                         //显示在设编号设置对话框内信息的行数LINECOUNTER
	  "[DEVICE]",                          //后面显示的信息内行，总5行=LINECOUNTER
	  "Xingong TL866II DLL Test",
	  "[Description]",
	  "PIC18F4550自定义序列号测试",
	  "               2018-01-11",

};

/////////////////////////////////////////////////////////////////////////
// 自动IC序列号计算函数，函数名称不可更改
// 返回值： 返回对芯片缓冲区操作的区块结果的结构指针PSTRUCOPERATOR
//   参数：     5个缓冲区指针  （应用程序传给DLL的）
//       pCode: 对应编程器程序区的数据指针
//     pEEdata: 对应编程器数据区的数据指针
//    pUserRow: 对应编程器USER ROW的数据指针，
//              在87C51编程时为Envrypt Table加密阵列的指针
//         pID: 对应编程器ID区的数据指针
//     pSerial: 前一次编程时的序列号
//              pSerial 对应值必须为ASCII码字符串(最大长度不可超过32个字符)
//              这个值可以作为一个初始值使用，也可以不使用这个值，如果不使用，需要处理在DLL文件中保存并处理初始编号。
//              使用这个字符串，需要把新的初值放在这个缓冲内，写芯片时，应用程序会自动保存，
//              在编程下一个芯片时，新的值会传送到DLL，这样可省略在DLL文件的初值保存部分程序
//              你也可以把它看成这是个编程器主程序调用DLL的计数据使用
//  pUniqueKey: 厂家设定的芯片唯一设别标志指针。对厂家每个芯片设有唯一序列号的单片机（并且无法更改的芯片），可用于自定义,最大128字节
//              加密算法，在程序中对该标志进行加密判断，使芯片中的原代码即使被克隆，也无法在另一个芯片上直接使用。
//              注：对于ATMEL AVR芯片虽没有唯一标识，但本编程器可自动读入RC校正字节，该字节不可更改，几十个芯片才有一个可能相同。
//              用户可更改对应存储位置加密算法的数据，使程序只能在这个芯片上正常工作，就可实现对芯片进行加密
//              （上电后，单片机软件读入OSCCAL寄存器，就是对应校正字节值）
//              有此方法，可方便实现量产，否则你需要对每个芯片的程序进行分别编译。
 //   pConfig  配置字指针，最长32字节
//////////////////////////////////////////////////////////////////////
GETSERIAL_API pSTRUCOPERATOR GetSerial(unsigned char * pCode,unsigned char * pEEdata,unsigned char * pUserRow,unsigned char * pID,char * pSerial,unsigned char * pUniqueKey,unsigned char * pConfig)
{
	//你可在这里读取或改变缓冲区全部数据
	//操作注意事项：
	//1、对缓冲区的操作，包刮读取或更改内容不可大于当前芯片的容量大小
	//   否则可能产生不可预料的严重重错误。
	//2、可以5个任意的缓冲区内放入任何数据，例如放入加密算法后的数据
	//3、对没有EEDATA 或USER ROW 或ID存储区的芯片，不可操作对应区块的缓存
	//4、在允许自动编号的情况下，编程器每测试或编程前，调用一次这个函数。
  int i;

     int  SerialNumber;
	 SerialNumber=atoi(pSerial);//转换字符串为10进制数   
      //对缓冲区操作例子，注意地址不要超出芯片的容量。
      /////////////////////////////////////////////////////////////////////
      UINT Code_StartAdd=0x3545;       //code区修改
      TestValue++;
	  //CODE区从0x3545开始的32字节全部填入为TestValue
	  for(i=0;i<32;i++)pCode[Code_StartAdd+i]=TestValue;

	  //EEDATA区从0开始的64字节全部填入为随机数
	  srand( (unsigned)time( NULL ) ); //seed
	  
      for(i=0;i<64;i++)pEEdata[i]=(unsigned char)(rand()&0xFF);

      //ID只有8字节，放入应用程序传下来的序列号
	  //注意这里放入的是ASCII码值，你也可转换成10进制值后放入
      for(i=0;i<8;i++)pID[i]=pSerial[i];

      //修改CODE区的另一块
	  for(i=0;i<32;i++)pCode[i]=(TestValue+i);
      //////////////////////////////////////////////////////////////////////
	  //设置操作区块，提示对那些存储区块进行了编号操作
	  //上面修改的内容必须在这里详细正确的描述
 	  OprateResult.nCount=4;  //修改了4处

	  OprateResult.MemoryType[0] = CODE_MEMORY;       //第1处的缓冲区类型
	  OprateResult.StartAddr[0]  = Code_StartAdd;     //第1处的开始地址
	  OprateResult.Len[0]        = 32;                //第1处的长度
  
	  OprateResult.MemoryType[1] = DATA_MEMORY;       //第2处的缓冲区类型
	  OprateResult.StartAddr[1]  = 0;                 //第2处的开始地址
	  OprateResult.Len[1]        = 64;                //第2处的长度

	  OprateResult.MemoryType[2] = ID_MEMORY;         //第3处的缓冲区类型
	  OprateResult.StartAddr[2]  = 0;                 //第3处的开始地址
	  OprateResult.Len[2]        = 8;                 //第3处的长度

	  OprateResult.MemoryType[3] = CODE_MEMORY;       //第4处的缓冲区类型
	  OprateResult.StartAddr[3]  = 0;                 //第4处的开始地址
	  OprateResult.Len[3]        = 32;                //第4处的长度

      //记录到文件的信息
	  //把当前的序列号，记录到输出文本文件中，以便查看记录，
	  //这对那些随机的唯一序列号相当重要,如果有，把随机生成的号加入到OprateResult.SerialRecord字符串中。
	  //OprateResult.SerialRecord总长不可超过512字节，该内容是主程序记录到文件中用的
	  //如果你不记录这个内容，那么你自已也不知道这个芯片的随机号是什么，

	  //如果应用程序记录的方式，不能符合你的要求，你可不使用应用程序中的文件记录方式
	  //在这里添加自建文件进行编号记录

                //在这里添加自定义记录代码//         

	  //把序列号放入到OprateResult结构中，供主程序记录
	  //特别提醒：OprateResult.SerialRecord必须是\0x结尾的ASCII码字符串
	  //          这里只记录了SerialNumber，如果需要，你可以把上面4处内容全部放入到这个字符串中，但必须转换为ASCII码
	  sprintf(OprateResult.SerialRecord,"%.8d",SerialNumber);


      //初值处理，把pSerial的内参加运算后的结果放入到芯片部分缓存后处理新的序列号
      //注意，在应用程序中设定的初值格式，必须符合你在DLL处理的要求，应用程序中输入可为任意可见字符，最长32字符
      //另外，这个字符串长度确定不要超过32字节（不包刮最后0x00,相当于33字节的缓存）
       //例，这里是简单算法举例，假定编号为10进制整数，应用程序中，初始编号设定不到大于8位的10进制数

	  SerialNumber++;//序列号加1 
	  //放入新的序列号，长度为8位，应用程序可记录当前号
	  sprintf(pSerial,"%.8d",SerialNumber);

	return &OprateResult;  //返回操作结果
}
// This is an example of an exported function.
//////////////////////////////////////////////////////////////////////
//返回IC名称，这个函数不可更改，编程器调用该函数,取得芯片型号名称
/////////////////////////////////////////////////////////////////////
GETSERIAL_API pICINFORMATION GetCurrentIcInfo(void)
{
	return &IcInformation; //直接返回IC信息的指针
}
//////////////////////////////////////////////////////////////////////
//编程结束后，编程器调用该函数,函数名称不可更改
//参数 state是主程序传给DLL的： 为0是表示编程芯片正确,其他值表示编程时出现错误
//这个函数是DLL错误处理用的，在这里可以加入你要的错误处理内容，也可不处理
/////////////////////////////////////////////////////////////////////
GETSERIAL_API int EndProgAccess(int state)
{
	if(state==0)
	{
		//正确时的处理
	}
	else
	{
		//出错时的处理
	}
	return 0; //必须返回0
}
