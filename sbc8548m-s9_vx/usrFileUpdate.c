#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <ioLib.h>
#include <string.h>
#include <sys/stat.h>

/**
 *  \brief usrFileUpdate
 *  �û���ftp�ļ��и���
 *  \param char *fileName
 *  \return 0--��ʾOK
 *          -1--��ʾ���ļ�ʧ��
 */
int usrFileUpdate(char *fileName)
{
    int fd;
    UINT32 fileSize;
    char *buf = NULL;
    char *str = NULL;
    struct stat fileStat;
    int ret = 0;
    
    /* read data from ftp file to buffer */
    str = malloc(200*sizeof(char));
    memset(str, 0, 200*sizeof(char));
    printf("read file from ftp!\n");
    sprintf(str, "host:%s", fileName);
    
    /* ��ȡ�ļ���С */
    if (stat(str, (struct stat *)&fileStat) != OK) {
        printf("the ftp file could not be 0 size\n");
        ret = -2;
        goto error;
    }

    /* ��ȡ�ļ����������� */
    fileSize = fileStat.st_size;
    fd = open(str, 0x202, 0x777);
    if (fd < 0) {
        printf("open the ftp %s file failed!\n", fileName);
        ret = -1;
        goto error;
    }

    buf = malloc(fileSize*sizeof(char));
    read(fd, buf, fileSize);
    close(fd);

    printf("write file to tffs!\n");
    memset(str, 0, 200*sizeof(char));
    sprintf(str, "/tffs0/%s", fileName);
    fd = open(str, 0x202, 0x777);
    if (fd < 0) {
        printf("open the tffs0 %s file failed!\n", fileName);
        ret = -1;
        goto error;
    }
    write(fd, buf, fileSize);
    close(fd);
    
error:
    if (buf != NULL)
        free(buf);
    if (str != NULL)
        free(str);
    
    return ret;
}
