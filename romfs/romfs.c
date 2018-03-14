
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "romfs.h"

const char rom1fsstr[8] = {'-','r','o','m','1','f','s','-'};

static uint32 romfs_getWORD(uint8 *pByte)
{
    return (uint32)((pByte[0] << 24)|(pByte[1] << 16)|(pByte[2] << 8) | pByte[3]);
}
/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

static int romfs_min(int a, int b)
{
	return a<b ? a : b;
}
/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

static uint32 romfs_checksum(void *data, int size)
{
	uint32 sum, *ptr;

	sum = 0; ptr = data;
	size>>=2;
	while (size>0) {
		sum += ntohl(*ptr++);
		size--;
	}
	return sum;
}
/**************************************************************
* ��������	romfs_fscheck 
* ��������: ���FLASH ��superblock �Ϸ���
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/
static eromfs romfs_fscheck(void *pBuf)
{
    //romfs_superblock *pblk = (romfs_superblock *)pBuf;

    if(pBuf == NULL )
        return eromfs_var_err;

    if(!memcmp(pBuf, rom1fsstr, sizeof(rom1fsstr)))
        return eromfs_ok;
        
    return eromfs_data_err;
}
static stRomfsFAT romfs;
/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

static eromfs romfs_setreadwind_size(uint32 size)
{
    if(size ==0 || size > ROMFS_BUFLEN)
        return eromfs_var_err;

    romfs.readwind_size = size;

    return eromfs_ok;   
}
/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

static uint32 romfs_getnextheader_addr(romfs_fileheader *pHeader)
{
    uint32 next;
    
    if(pHeader == NULL)
        return 0;

    next = romfs_getWORD(pHeader->next);
    next &= 0xFFFFFFF0;
    return next;
}
/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

static uint32 romfs_checkheaderType(romfs_fileheader  *pHeader)
{
    return (uint32)romfs_getWORD(pHeader->spec);
}

static char *romfs_getfilename(romfs_fileheader  *pHeader)
{
    return pHeader->name;
}
/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-10
**************************************************************/
static const char *romfs_getname_frompath(const char *pPath, char mask)
{
    uint32 len;
    uint32 i;
    const char *pfilename;

    if(pPath == NULL)
        return NULL;

    len = strlen(pPath);

   if(pPath[len - 1] == mask)
        return NULL;
    
    for(i = len; i > 0; i--)
    {
        if(pPath[i - 1] == mask)
        {
            pfilename = pPath + i ;
            return pfilename;
        }
    }

   return  (pfilename = pPath + i ); 

}
/**************************************************************
* ��������	 romfs_readwindows
* ��������: ���ļ�ϵͳ���ں���
* �������: ��
* �������: ��ָ��λ�õ����ݵ��ļ�ϵͳ���
* ����ֵ:	  	��
* ����:	 by longj 2017-11-09
**************************************************************/

static void romfs_readwindows(void)
{
    W25Qx_Read( romfs.readpt, (unsigned char *)&romfs.romfsbuf[0],romfs.readwind_size);
}
/**************************************************************
* ��������	 romfs_movewind2
* ��������: �ƶ���ȡ���ڵ�ָ��λ�ã�ʵ����ת��
* �������: uint32 Addr 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

static romfs_fileheader * romfs_movewind2(uint32 Addr)
{
    if(Addr > romfs.romfssize)
        return NULL;
        
    romfs.readpt = Addr;
    romfs_readwindows();

    return (romfs_fileheader *)romfs.romfsbuf;
}
/**************************************************************
* ��������	 
* ��������: ��ʼ�� romfs
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

eromfs romfs_init(uint32 *pFirstHeader)
{
    eromfs ret;

    *pFirstHeader = 0;

    memset(&romfs, 0 ,sizeof(romfs));
    romfs.readwind_size = ROMFS_BUFLEN;
    
    romfs_readwindows();
    ret = romfs_fscheck((void *)&romfs.romfsbuf[0]);
    if(ret != eromfs_ok)
        return ret;

    memcpy(&romfs.romfsspblk, &romfs.romfsbuf[0], sizeof(romfs_superblock));
    romfs.romfssize = romfs_getWORD(romfs.romfsspblk.size);
    
    if(pFirstHeader)
        *pFirstHeader = sizeof(romfs_superblock) + ROMFS_NAME_SIZE;
    return ret;
}


/* ת���ɴ�д��׺�� */
void romfs_getUpperOLower(char *str, char *dstchar, uint32 toUroL)
{
    uint32 len,i;
    int (*tfsion)(int a);
    len = strlen(str);

    if(toUroL)
        tfsion = toupper;
    else
        tfsion = tolower;
        
    for(i = 0; i < len ; i++)
    {
        dstchar[i] = tfsion(*(str + i));
    }
    
}
/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-10
**************************************************************/
eromfs romfs_findfile(romfsFIL *startFIL, romfsFIL *FIL, const char *pPath)
{
    eromfs ret;
    uint32 readIndex,i;
    romfs_fileheader *pheader;
    char *filename;
    char *dstfil;
    char *suffix;
    char *namebuf[ROMFS_NAME_SIZE];
    uint8 mask;

    if(FIL == NULL || pPath == NULL)
        return eromfs_var_err;
    
    mask =  '/';
    dstfil = (char *)romfs_getname_frompath(pPath,mask);

    memset(namebuf, 0 , sizeof(namebuf));
    strcpy((char *)&namebuf[0], dstfil);         /* �����ļ���*/
    
    mask =  '.';
    suffix = (char *)romfs_getname_frompath((char *)&namebuf[0],mask);        /* �ҵ���׺�� */

    if(suffix) /* �к�׺�� */
        romfs_getUpperOLower(suffix,suffix,0); /* ת��Сд*/
    
    //romfs.readpt = romfs.fstfileheader;/* �ļ������ǵ���ģ�����ÿ�ζ�ֻ�ܴӵ�һ���ļ���ʼ�� */
    readIndex = startFIL->fileheader;

    for(;;)/* �����ҵ���һ���ļ���һ��ROMFS ǰ��������Ŀ¼�ļ����ֱ��ǵ�ǰĿ¼�ļ�����һ��Ŀ¼�ļ� */
    {
        pheader = romfs_movewind2(readIndex);
        if(pheader == NULL)
        {
            ret = eromfs_no_thisfil;
            break;
        }
        
        filename = romfs_getfilename(pheader);
        
        filename[ ROMFS_NAME_SIZE - 1 ] = '\0';
        
        if(strcmp((char *)&namebuf[0],filename) == 0)
        {
            FIL->filesize = romfs_getWORD(pheader->size);   /* �����ļ����� */

            FIL->fileheader = readIndex;
                
            strcpy(FIL->name, filename);                        /* �����ļ��� */
   
            FIL->fseek = 0; /* ��дָ����0 */
            //memcpy(&romfs.fileheader, romfs.romfsbuf, sizeof(romfs_fileheader));/* ��¼��һ���ļ�ͷ*/
            ret = eromfs_ok;
            break;
        }
        readIndex = romfs_getnextheader_addr(pheader);
    }
    return ret;
}
/**************************************************************
* ��������	 romfs_mount
* ��������: romfs �ļ�ϵͳ���غ���
* �������: ��
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

eromfs romfs_mount(void)
{
    romfsFIL file, startfile;
    eromfs ret;
    //uint8 readbuf[ROMFS_READWIND_SIZE];
    uint32 fileheader;
    //romfs_fileheader *pheader;

    ret = romfs_init(&fileheader);

    startfile.fileheader = fileheader;
    
    if(!fileheader || ret != eromfs_ok)
        return eromfs_data_err;

        
    /* �����ҵ���һ���ļ���һ��ROMFS ǰ��������Ŀ¼�ļ����ֱ��ǵ�ǰĿ¼�ļ�����һ��Ŀ¼�ļ� */
    if(romfs_findfile(&startfile,&file, ".") != eromfs_ok) //||        romfs_scanfile(&file, ".") == eromfs_ok)
    {
        return eromfs_no_thisfil;
    } 
    
    memcpy(&romfs.startfile, &file, sizeof(romfs.startfile));
    
    return eromfs_ok;
}


/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

eromfs romfs_fopen(romfsFIL *fp, const char *pPath ,uint32 mode)
{
  eromfs ret ;
  ret = romfs_findfile(&romfs.startfile, fp, pPath);
  return ret;
}
/**************************************************************
* ��������	 
* ��������: 
* �������: 
* �������: 
* ����ֵ:	 
* ����:	 by longj 2017-11-09
**************************************************************/

eromfs romfs_fread(
    romfsFIL *fp, 
	void* buff,		/* Pointer to data buffer */
	uint32 btr,		/* Number of bytes to read */
	uint32* br		/* Pointer to number of bytes read */
)
{
    uint32 dataindex;
    uint32 lstlen;
    
    if(fp == NULL || buff == NULL || btr == 0 || br == NULL)
        return eromfs_var_err;

    *br = 0;

    if(fp->filesize == 0)
        return eromfs_ok;
        
    dataindex = fp->fileheader + sizeof(romfs_fileheader) + ROMFS_NAME_SIZE + fp->fseek;
    lstlen = fp->filesize - fp->fseek;
    
    if(lstlen > btr)
    {
        W25Qx_Read(dataindex, buff, btr);
        fp->fseek += btr;
        *br = btr;
    }
    else if(lstlen)
    {
        W25Qx_Read(dataindex, buff, lstlen);
        fp->fseek += lstlen;
        *br = lstlen;
    }
    
    return eromfs_ok;
}
