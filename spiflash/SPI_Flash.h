#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include "stm32f10x_type.h"

#define W25QX128JV (1)

#define Dummy_Byte 0xA5

#define W25QX_PAGE_SIZE             (256)
#define W25QX_PAGES_PER_SECTOR      (16)
#define W25QX_SECTOR_SIZE          (W25QX_PAGE_SIZE*W25QX_PAGES_PER_SECTOR)
#define W25QX_SECTOR_PER_BLOCK      (16)
#define W25QX_BLOCK_SIZE           (W25QX_SECTOR_SIZE*W25QX_SECTOR_PER_BLOCK) 

#ifdef W25QX32JV
    #define W25QX_SECTOR_NUM        (1024)
#elif  W25QX128JV
    #define W25QX_SECTOR_NUM        (4096)
#endif

/* Select SPI FLASH: ChipSelect pin low  */
#define Select_Flash()          GPIO_ResetBits(GPIOB, GPIO_Pin_8)
/* Deselect SPI FLASH: ChipSelect pin high */
#define NotSelect_Flash()       GPIO_SetBits(GPIOB, GPIO_Pin_8)

#define SPI_Flash_ResetOFF()    GPIO_SetBits(GPIOB, GPIO_Pin_6)



extern void SPI_Flash_Init(void);	         //SPI��ʼ��
extern u8 SPI_Flash_ReadByte(void);		//flash����������������һ���ֽ�
extern u8 SPI_Flash_SendByte(u8 byte);		//	FLASH������������������һ���ֽ�
/****************************************************************************
* ��    �ƣ�void FlashReadID(void)
* ��    �ܣ��������뼰�ͺŵĺ���
* ��ڲ�����  
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
extern void FlashReadID(unsigned char *pFacID, unsigned char *pDevID);

/****************************************************************************
* ��    �ƣ�void W25Qx_Read(unsigned long addr, unsigned char *readbuff, unsigned int NumOfByte)
* ��    �ܣ�ҳ��
* ��ڲ�����unsigned long addr--ҳ   unsigned char *readbuff--����   unsigned int NumOfByte--����
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/  
extern void W25Qx_EraseSector(unsigned int SectorAddr);

extern void W25Qx_Read(unsigned long addr, unsigned char *readbuff, unsigned int NumOfByte);
extern void W25QX_Write(u32 WriteAddr,u8* pBuffer,u16 NumByteToWrite);   
extern void W25Qx_EraseSector(unsigned int SectorAddr);

extern void W25QXX_Write_NoCheck(u32 WriteAddr,u8* pBuffer,u32 NumByteToWrite); 
#endif


