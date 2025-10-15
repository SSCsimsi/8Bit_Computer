// Nand_Ecc.cpp : 
//

#include "stdafx.h"
#include "Nand_Ecc.h"
//����ECC����
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

	//�����յ�ECC��������ECCCode
	ECCCode[0] = temp1;//��Ÿ�8bit
	ECCCode[1] = temp2;//����м��8bit
}
/******************************************************************
* ���ܣ�����256�ֽڵ�ECCֵ
* unsigned char *pData : Դ����ָ��
* unsigned char *ECCCode ��Eccֵָ��
*       ��ʼֵ����Ϊ�� ECCCode[0]=0;ECCCode[1]=0;ECCCode[2]=0;
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
	//�������յ�ECC��
	//ECCCode[0] = ~ECCCode[0];
	//ECCCode[1] = ~ECCCode[1];
	  ECCCode[2] = (((~reg1) << 2) | 0x03);
}
/******************************************************************
* ���ܣ����ݾ������������ڴ˲�δ�õ�
* ��������
* dat[]: ʵ�ʶ�ȡ������
* ReadECC[]:��������ʱ����ԭʼ���ݲ�����ECC��
* CalECC[]:��ȡ���ݵ�ͬʱ������ECC��
*******************************************************************/
int NandCorrectData_256BYTE(unsigned char *dat, unsigned char *ReadECC, unsigned char *CalECC)
{
	unsigned char a, b, c, bit, add, i, d1, d2, d3;
	//����
	d1 = ReadECC[0] ^ CalECC[0];
	d2 = ReadECC[1] ^ CalECC[1];
	d3 = ReadECC[2] ^ CalECC[2];
	//printf("d1=0x%0x,d2=0x%0x,d3=0x%0x/n",d1,d2,d3);
	if ((d1 | d2 | d3) == 0)
	{
		//�޴�����
		printf("�޴�����/n");
		return 0;
	}
	else
	{
		a = ((d1 >> 1) ^ d1) & 0x55;
		b = ((d2 >> 1) ^ d2) & 0x55;
		c = ((d3 >> 1) ^ d3) & 0x54;
		//�˴������������ǣ����������1bit��ECC������ôECC���ؽ����--ÿ����Ե�bit�����෴����Ϊ0&1����1&0
		if ((a == 0x55)&(b == 0x55)&(c == 0x54))
		{
			//��У����1bit ECC����

			//���ȼ�������Byte

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

			//���㷢�������Bit
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
			//�������ݾ���
			// printf("��ʼ�������ݾ���/n");
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
			// printf("���������d1,d2,d3��1�ĸ���/n");
			//���������d1,d2,d3��1�ĸ���
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
				//������ECC���󣬼����ECC���ݵ��������˴�������������£����۶���
				//bit�����˷�ת�����������i=1����������������������ԭ��ֻ������ECC���뱾��������
				// printf("���ECC���ݵ��������˴���/n");
				return 2;
			}
			else
			{
				//����У����ECC���󣬼�Uncorrectable Error
				// printf("Uncorrectable Error/n");
				return -1;
			}
		}
	}
	return -1;
}
