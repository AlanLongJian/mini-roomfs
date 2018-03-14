#ifndef _ROMFS_H_
#define _ROMFS_H_

#include <stdio.h>
#include <string.h>

#define ROMFH_HRD   0 /* Ӳ�����ļ� */
#define ROMFH_DIR   1 /* Ŀ¼�ļ� */
#define ROMFH_REG   2 /* ��ͨ�ļ� */
#define ROMFH_LNK   3 /* �����ļ� */
#define ROMFH_BLK   4 /* ���豸�ļ� */
#define ROMFH_CHR   5 /* �ַ��豸�ļ� */
#define ROMFH_SCK   6 /* �׽����ļ� */
#define ROMFH_FIF   7
#define ROMFH_EXEC  8

#define ntohl(x)     (x)

#define ROMSB_WORD0 (0x2D726F6D)//('-','r','o','m')
#define ROMSB_WORD1 (0x3166732D)//('1','f','s','-')

#define ROMFS_NAME_SIZE     (16)

 
#define ROMFS_READWIND_SIZE     (32)    /* ����ʱ��ȡ���ֽ���*/      

#define ROMFS_BUFLEN            (ROMFS_READWIND_SIZE)


/*
offset      content
        +---+---+---+---+
  0     |  -  |  r  |  o  | m  |          +---+---+---+---+    The ASCII representation of those bytes
  4     |  1  |  f  |  s  |  -  |  /    (i.e. "-rom1fs-")
        +---+---+---+---+
  8     |     full size       |    The number of accessible bytes in this fs.
        +---+---+---+---+
 12     |    checksum   |       The checksum of the FIRST 512 BYTES.
        +---+---+---+---+
 16     |  volume name     |    The zero terminated name of the volume,
        :               :       padded to 16 byte boundary.
        +---+---+---+---+
 xx     |     file      |
        :    headers    :
*/

typedef unsigned char uint8;

typedef unsigned int uint32;

typedef enum _ROMFS_RET_
{
    eromfs_ok,
    eromfs_var_err,
    eromfs_data_err,
    eromfs_no_thisfil,
}eromfs;


/* On-disk "super block" */
typedef struct _st_romfs_super_block {
        uint8 word0[4];
        uint8 word1[4];
        uint8 size[4];
        uint8 checksum[4];
        char name[0];           /* volume name */
}romfs_superblock;

/*
offset      content
        +---+---+---+---+
        |     file header    ��      
        +---+---+---+---+      
        |      file date     ��      
        +---+---+---+---+      
        |     file header    ��      
        +---+---+---+---+      
        |      file date     ��      
        |     file header    ��      
        +---+---+---+---+      
        |      file date     ��      
        +---+---+---+---+      
        |      ����.        ��      
        +---+---+---+---+   
offset      content
        +---+---+---+---+
  0     | next filehdr   | X  |       The offset of the next file header
        +---+---+---+---+         (zero if no more files)
  4     |   spec.info       |       Info for directories/hard links/devices
        +---+---+---+---+
  8     |        size       |       The size of this file in bytes
        +---+---+---+---+
 12     |    checksum     |       Covering the meta data, including the file
        +---+---+---+---+         name, and padding
 16     |    file name      |       The zero terminated name of the file,
        :                    :       padded to 16 byte boundary*/
/* On disk inode */
typedef struct _st_romfs_inode{
        uint8 next[4];            /* low 4 bits see ROMFH_ */
        uint8 spec[4];
        uint8 size[4];
        uint8 checksum[4];            /* check sum of file header and file name */
        char name[0];
}romfs_fileheader;

typedef struct _ST_ROMFS_FIL_
{
    uint32 fileheader;
    //uint32 filept;                  /* �ļ�����ƫ�Ƶ�ַ */
    uint32 filesize;                /* �ļ����� */
    uint32 fseek;                   /* �ļ���дָ��*/
    char name[ROMFS_NAME_SIZE];
 }romfsFIL;

typedef struct _ST_ROMFS_FAT_
{
    romfs_superblock romfsspblk;    /* �ļ�ϵͳ������ */
    romfs_fileheader fileheader;    /* ��һ���ļ�ͷ */
    romfsFIL startfile;             /* ��һ���ļ��������� */
    uint32 romfssize;               /* �ļ�ϵͳ���� */
    //uint32 fstfileheader;           /* ��һ���ļ����ļ�ͷλ�� */
    //uint32 endfileheader;           /* �����ļ����ļ�ͷλ�� */
    uint32 readpt;                  /* ��дλ��ָ�� */
    uint32 readwind_size;           /* ��д���ȼ��� */
    uint8  romfsbuf[ROMFS_BUFLEN];  /* ��д������ */
}stRomfsFAT;


/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

extern eromfs romfs_mount(void);
/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

extern eromfs romfs_fopen(romfsFIL *fp, const char *filename ,uint32 mode);
/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

extern eromfs romfs_fread(
    romfsFIL *fp, 
	void* buff,		/* Pointer to data buffer */
	uint32 btr,		/* Number of bytes to read */
	uint32* br		/* Pointer to number of bytes read */
);

#endif



