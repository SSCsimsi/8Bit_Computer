// Nand_Ecc.cpp : 
//

#include "stdafx.h"
#include "Nand_Ecc.h"
//计算ECC代码
void NandTranResult(unsigned char reg2, unsigned char reg3, unsigned char *ECCCode)
{
	unsigned char temp1, temp2, i, a, b;
	temp1 = temp2 = 0;
	a = b = 0x80;
	for (i = 0; i<4; i++)
	{
		if (reg3&a)
			temp1 |= b;
		b >>= 1;
		if (reg2&a)
			temp1 |= b;
		b >>= 1;
		a >>= 1;
	}
	b = 0x80;
	for (i = 0; i<4; i++)
	{
		if (reg3&a)
			temp2 |= b;
		b >>= 1;
		if (reg2&a)
			temp2 |= b;
		b >>= 1;
		a >>= 1;
	}

	//将最终的ECC存入数组ECCCode
	ECCCode[0] = temp1;//存放高8bit
	ECCCode[1] = temp2;//存放中间的8bit
}
/******************************************************************
* 功能：计算256字节的ECC值
* unsigned char *pData : 源数据指针
* unsigned char *ECCCode ：Ecc值指针
*       初始值必须为： ECCCode[0]=0;ECCCode[1]=0;ECCCode[2]=0;
******************************************************************/
void NandCalECC_256BYTE( unsigned char *pData, unsigned char *ECCCode)
{
	unsigned char reg1, reg2, reg3, temp;
	int j;
	reg1 = reg2 = reg3 = 0;
	for (j = 0; j<256; j++)
	{
		temp = ECCTable[pData[j]];
		reg1 ^= (temp & 0x3f);
		if (temp & 0x40)
		{
			reg3 ^= (unsigned char)j;
			reg2 ^= (~((unsigned char)j));
		}
	}
	NandTranResult(reg2, reg3, ECCCode);  // get ECCCode[0] and ECCCode[1]
	//计算最终的ECC码
	//ECCCode[0] = ~ECCCode[0];
	//ECCCode[1] = ~ECCCode[1];
	  ECCCode[2] = (((~reg1) << 2) | 0x03);
}
/******************************************************************
* 功能：数据纠正，本函数在此并未用到
* 参数解释
* dat[]: 实际读取的数据
* ReadECC[]:保存数据时根据原始数据产生的ECC码
* CalECC[]:读取数据的同时产生的ECC码
*******************************************************************/
int NandCorrectData_256BYTE(unsigned char *dat, unsigned char *ReadECC, unsigned char *CalECC)
{
	unsigned char a, b, c, bit, add, i, d1, d2, d3;
	//计算
	d1 = ReadECC[0] ^ CalECC[0];
	d2 = ReadECC[1] ^ CalECC[1];
	d3 = ReadECC[2] ^ CalECC[2];
	//printf("d1=0x%0x,d2=0x%0x,d3=0x%0x/n",d1,d2,d3);
	if ((d1 | d2 | d3) == 0)
	{
		//无错误发生
		printf("无错误发生/n");
		return 0;
	}
	else
	{
		a = ((d1 >> 1) ^ d1) & 0x55;
		b = ((d2 >> 1) ^ d2) & 0x55;
		c = ((d3 >> 1) ^ d3) & 0x54;
		//此处的理论依据是：如果发生了1bit的ECC错误，那么ECC异或地结果是--每个配对的bit数据相反，即为0&1或者1&0
		if ((a == 0x55)&(b == 0x55)&(c == 0x54))
		{
			//可校正的1bit ECC错误

			//首先计算错误的Byte

			a = b = c = 0x80;
			add = 0;
			for (i = 0; i<4; i++)
			{
				if (d1&a)
					add |= b;
				a >>= 2;
				b >>= 1;
			}

			for (i = 0; i<4; i++)
			{
				if (d2&c)
					add |= b;
				c >>= 2;
				b >>= 1;
			}

			//计算发生错误的Bit
			bit = 0;
			a = 0x80;
			b = 0x04;

			// printf("d3 = 0x%0x/n",d3);
			for (i = 0; i<3; i++)
			{
				if (d3&a)
				{
					bit |= b;
					//   printf("Detected!/n");
				}
				else
				{
					//printf("d3=0x%0x,a=0x%0x,d3&a=0x%0x/n",d3,a,d3&a);
					//   printf("Not Detected!/n");
				}
				a >>= 2;
				b >>= 1;
			}
			//进行数据纠正
			// printf("开始进行数据纠正/n");
			// printf("Error byte: %2d,Error bit: %2d/n",add,bit);
			b = 0x01;
			b <<= bit;
			a = dat[add];
			a ^= b;
			dat[add] = a;
			return 1;
		}
		else
		{
			i = 0;
			// printf("计算异或结果d1,d2,d3中1的个数/n");
			//计算异或结果d1,d2,d3中1的个数
			while (d1)
			{
				if (d1 & 0x01)
					i++;
				d1 >>= 1;
			}
			while (d2)
			{
				if (d2 & 0x01)
					i++;
				d3 >>= 1;
			}
			while (d3)
			{
				if (d3 & 0x01)
					i++;
				d3 >>= 1;
			}
			if (i == 1)
			{
				//发生了ECC错误，即存放ECC数据的区域发生了错误，正常的情况下，无论多少
				//bit发生了反转，都不会出现i=1的情况，出现了这种情况的原因只可能是ECC代码本身有问题
				// printf("存放ECC数据的区域发生了错误/n");
				return 2;
			}
			else
			{
				//不可校正的ECC错误，即Uncorrectable Error
				// printf("Uncorrectable Error/n");
				return -1;
			}
		}
	}
	return -1;
}
