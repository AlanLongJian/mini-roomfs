/****************************************************************************
* Copyright (C), 2011 �ܶ�Ƕ��ʽ������ www.ourstm.net
*
* �������� �ܶ���STM32������V2,2.1,V3,MINI�ϵ���ͨ��           
* QQ: 9191274, ������sun68, Email: sun68@163.com 
* �Ա����̣�ourstm.taobao.com  
*
* �ļ���: spi_flash.c
* ���ݼ���:
*       �����̰�����SST25VF016B�ĵײ���������
*
* �ļ���ʷ:
* �汾��  ����       ����    ˵��
* v0.2    2011-7-07 sun68  �������ļ�
*
*/
#include <stdio.h>
//#include "demo.h"
#include "SPI_Flash.h"
//#include "headerfile.h"
#include "stm32f10x_lib.h"

typedef enum _E_SPIFLASH_CMD_
{
    eWritePage = 0x02,
    eWtiteDisable = 0x04,
    eReadStatue = 0x05,
    eWriteEn = 0x06,
    eFastRead = 0x0B,
    eEraseSector = 0x20,
    eWriteStatue = 0x50,
    eReadID = 0x90,
}eW25QxCmd;
/****************************************************************************
* ��    �ƣ�void wip(void)
* ��    �ܣ�æ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void W25Qx_Wait_Busy(void);
unsigned char W25Qx_ReadStatue(void);

/****************************************************************************
* ��    �ƣ�void wen(void)
* ��    �ܣ�дʹ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void W25Qx_WriteEn(void)
{
	Select_Flash();
	SPI_Flash_SendByte(eWriteEn);
	NotSelect_Flash();
}
/****************************************************************************
* ��    �ƣ�void wsr(void)
* ��    �ܣ�д״̬
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void W25Qx_WriteStatue(void)
{	
	Select_Flash();
	SPI_Flash_SendByte(eWriteStatue);
	NotSelect_Flash(); 
	Select_Flash();
	SPI_Flash_SendByte(0x01);
	SPI_Flash_SendByte(0x00); 
	NotSelect_Flash();
    W25Qx_Wait_Busy();
}
/****************************************************************************
* ��    �ƣ�void wip(void)
* ��    �ܣ�æ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void W25Qx_Wait_Busy(void)
{
    unsigned char a = 1;
    while((a & 0x01) == 1) 
        a = W25Qx_ReadStatue();	

}
/****************************************************************************
* ��    �ƣ�void wdis(void)
* ��    �ܣ�д��ֹ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void W25Qx_WriteDisable(void)
{
	Select_Flash();
	SPI_Flash_SendByte(eWtiteDisable); 
	NotSelect_Flash();
	W25Qx_Wait_Busy();
	
}	

/****************************************************************************
* ��    �ƣ�unsigned char rdsr(void)
* ��    �ܣ���״̬�Ĵ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
unsigned char W25Qx_ReadStatue(void)
{
	unsigned char busy;
	Select_Flash();
	SPI_Flash_SendByte(eReadStatue);
	busy = SPI_Flash_ReadByte();
	NotSelect_Flash();
	return(busy);
	
}
/****************************************************************************
* ��    �ƣ�void sect_clr(unsigned long a1)
* ��    �ܣ�ҳ����
* ��ڲ�����unsigned long a1--ҳ   
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void W25Qx_EraseSector(unsigned int SectorAddr)
{
    u32 DstAddr;
    
	//W25Qx_WriteStatue();
	DstAddr = SectorAddr * W25QX_SECTOR_SIZE;
	W25Qx_WriteEn();
	Select_Flash();
  	SPI_Flash_SendByte(eEraseSector);
	SPI_Flash_SendByte((DstAddr & 0xffffff)>>16);          //addh
	SPI_Flash_SendByte((DstAddr & 0xffff)>>8);          //addl 
	SPI_Flash_SendByte(DstAddr & 0xff);                 //wtt
	NotSelect_Flash();
	W25Qx_Wait_Busy();	
}
/****************************************************************************
* ��    �ƣ�void W25Qx_Read(unsigned long addr, unsigned char *readbuff, unsigned int NumOfByte)
* ��    �ܣ�ҳ��
* ��ڲ�����unsigned long addr--ҳ   unsigned char *readbuff--����   unsigned int NumOfByte--����
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/  
void W25Qx_Read(unsigned long addr, unsigned char *readbuff, unsigned int NumOfByte)
{
	unsigned int i=0; 	
	Select_Flash();
	SPI_Flash_SendByte(eFastRead);
	SPI_Flash_SendByte((addr&0xffffff)>>16);
	SPI_Flash_SendByte((addr&0xffff)>>8);
	SPI_Flash_SendByte(addr&0xff);
	SPI_Flash_SendByte(0);
	while(i<NumOfByte){	
		readbuff[i++]=SPI_Flash_ReadByte();
	}
	NotSelect_Flash();	 	
}
void W25Qx_WritePage(u32 addr, u8 *readbuff, u16 PageSize){
	u32 i;

  	W25Qx_WriteEn();
  	
	Select_Flash();    
	SPI_Flash_SendByte(eWritePage);
	SPI_Flash_SendByte((addr&0xffffff)>>16);
	SPI_Flash_SendByte((addr&0xffff)>>8);
	SPI_Flash_SendByte(addr&0xff);
	
	i = 0;
	while(i < PageSize)
	{
		SPI_Flash_SendByte(readbuff[i]);
		//printf("%02X ",readbuff[i]);
		i++;
	}
	NotSelect_Flash();
		
	W25Qx_Wait_Busy();
	
	W25Qx_WriteDisable();	

}

void W25QXX_Write_NoCheck(u32 WriteAddr,u8* pBuffer,u32 NumByteToWrite)   
{ 			 		 
	u32 pageremain;	   
	pageremain = W25QX_PAGE_SIZE - WriteAddr % W25QX_PAGE_SIZE; //��ҳʣ����ֽ���	
	
	if(NumByteToWrite <= pageremain)
	    pageremain = NumByteToWrite;//������256���ֽ�
	    
	while(1)
	{	   
		W25Qx_WritePage(WriteAddr,pBuffer, pageremain);
		
		if(NumByteToWrite == pageremain)
		    break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	
			NumByteToWrite -= pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			
			if(NumByteToWrite > W25QX_PAGE_SIZE)
			    pageremain = W25QX_PAGE_SIZE; //һ�ο���д��256���ֽ�
			else 
			    pageremain = NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 

#if 1
/* ע�������������дSECTOR �е����ݣ��ᶪ���ݵġ���FATFS��ʹ����Ϊ����BUF,���Բ�����ַ�����SECTOR��д��*/
void W25QX_Write(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	//u16 i;

 	if(pBuffer == NULL|| NumByteToWrite == 0)
 	    return;
   	
 	secpos = WriteAddr / W25QX_SECTOR_SIZE;//������ַ  
	secoff = WriteAddr % W25QX_SECTOR_SIZE;//�������ڵ�ƫ��
	secremain = W25QX_SECTOR_SIZE - secoff;//����ʣ��ռ��С
	
 	if(NumByteToWrite <= secremain)
 	    secremain = NumByteToWrite;//������4096���ֽ�

	while(1) 
	{	
        W25Qx_EraseSector(secpos);//����������� 
        W25QXX_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   

		if(NumByteToWrite == secremain)
		    break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff = 0;//ƫ��λ��Ϊ0 	 

		   	pBuffer += secremain;  //ָ��ƫ��
			WriteAddr += secremain;//д��ַƫ��	   
		   	NumByteToWrite -= secremain;				//�ֽ����ݼ�
		   	
			if(NumByteToWrite > W25QX_SECTOR_SIZE)
			    secremain = W25QX_SECTOR_SIZE;	//��һ����������д����
			else 
			    secremain = NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}
#else 
u8 W25QXX_BUFFER[W25QX_SECTOR_SIZE];		 
void W25QX_Write(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;
 	u8 *pSectorBuff;

 	if(pBuffer == NULL|| NumByteToWrite == 0)
 	    return;
   	
   	pSectorBuff = W25QXX_BUFFER;
 	secpos = WriteAddr / W25QX_SECTOR_SIZE;//������ַ  
	secoff = WriteAddr % W25QX_SECTOR_SIZE;//�������ڵ�ƫ��
	secremain = W25QX_SECTOR_SIZE - secoff;//����ʣ��ռ��С
	
 	if(NumByteToWrite <= secremain)
 	    secremain = NumByteToWrite;//������4096���ֽ�

	while(1) 
	{	
        //W25Qx_EraseSector(secpos);//����������� 
        //W25QXX_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		W25Qx_Read(secpos*W25QX_SECTOR_SIZE, pSectorBuff, W25QX_SECTOR_SIZE);//������������������
		
		for(i=0;i<secremain;i++)//У������
		{
			if(pSectorBuff[secoff+i] != 0XFF)
			    break;//��Ҫ����  	  
		}
		
		if(i<secremain)//��Ҫ����
		{
			W25Qx_EraseSector(secpos);		//�����������
			for(i=0;i<secremain;i++)	   		//����
			{
				pSectorBuff[i+secoff]=pBuffer[i];	  
			}
			W25QXX_Write_NoCheck(secpos*W25QX_SECTOR_SIZE, pSectorBuff, W25QX_SECTOR_SIZE);//д����������  

		}
		else 
		    W25QXX_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   

		if(NumByteToWrite == secremain)
		    break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff = 0;//ƫ��λ��Ϊ0 	 

		   	pBuffer += secremain;  //ָ��ƫ��
			WriteAddr += secremain;//д��ַƫ��	   
		   	NumByteToWrite -= secremain;				//�ֽ����ݼ�
		   	
			if(NumByteToWrite > W25QX_SECTOR_SIZE)
			    secremain = W25QX_SECTOR_SIZE;	//��һ����������д����
			else 
			    secremain = NumByteToWrite;			//��һ����������д����
		}	 
	};	 
}
#endif
/****************************************************************************
* ��    �ƣ�void FlashReadID(void)
* ��    �ܣ��������뼰�ͺŵĺ���
* ��ڲ�����  
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void FlashReadID(unsigned char *pFacID, unsigned char *pDevID)
{
	Select_Flash();	
  	SPI_Flash_SendByte(eReadID);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
  	*pFacID = SPI_Flash_ReadByte();		          //BFH: ������SST
	*pDevID = SPI_Flash_ReadByte();	              //41H: �����ͺ�SST25VF016B     
  	NotSelect_Flash();	
}

/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_Flash_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
   
  /* ʹ��SPI1 ʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 ,ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);	
	
  /* ���� SPI1 ����: SCK, MISO �� MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* ����PC4ΪSST25VF016B��Ƭѡ  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);   
 
  /* SPI1���� */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* ʹ��SPI1  */
  SPI_Cmd(SPI1, ENABLE);   
  NotSelect_Flash();
 
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
u8 SPI_Flash_ReadByte(void)
{
  return (SPI_Flash_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte 
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_Flash_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  //NotSelect_Flash();  while(1);
  /* Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(SPI1, byte);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}/******************* (C) COPYRIGHT 2011 �ܶ�STM32 *****END OF FILE****/



