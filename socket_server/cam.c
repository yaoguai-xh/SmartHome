/*===============================================================
*   Copyright (C) 2018 All rights reserved.
*   
*   文件名称：cam.c
*   创 建 者：xuhao
*   创建日期：2018年07月10日
*   描    述：
*
*   更新日志：
*
================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "cam.h"

void *pthread_pic(void *arg)
{
	int *client_fd=arg;
	char RecvBuffer[BUFFER_SIZE]={0};
	char *rgb_buf;
	char *yuv_buf;
	struct jpg_buf_t *jpg;

	int camera_fd;
	char *cam_dev="/dev/video0";
	unsigned int cam_width=480;
	unsigned int cam_hight=640;
	unsigned int zhen_size;
	unsigned int ismjpeg;
	unsigned int pic_size;
	unsigned int index=0;
	
	//init camera
	camera_fd=camera_init(cam_dev,&cam_width,&cam_hight,&zhen_size,&ismjpeg);
	if(camera_fd<0)
	{
		perror("camera init error");
		return (void *)0;
	}
	if(ismjpeg==0)
	{
		printf("picture type is jpeg\n");
	}
	else if(ismjpeg!=0)
	{
		printf("picture type is not jpeg\n");
		rgb_buf=malloc(cam_width*cam_hight*3);
		convert_rgb_to_jpg_init();
	}
	//init rgb to jpg
	
	//camera start
	int ret;
	if((ret=camera_start(camera_fd))<0)
	{
		perror("camera start error");
		return (void *)0;
	}
	//malloc space	
	jpg = malloc(sizeof(struct jpg_buf_t));

	while(1)
	{
		//read socket client_fd[1]
		memset(RecvBuffer,0,sizeof(RecvBuffer));
		read(*client_fd,RecvBuffer,sizeof(RecvBuffer));
		if(strstr("PIC_SEND",RecvBuffer)!=NULL)
		{
			printf("received: %s\n",RecvBuffer);
			//dqbuf			
			if((ret=camera_dqbuf(camera_fd,(void **)&yuv_buf,&pic_size,&index))<0)
			{
				perror("dqbuf error");
				return (void *)0;
			}
			if(ismjpeg == 0){
				memcpy(jpg->jpg_buf,yuv_buf,pic_size);
				jpg->jpg_size = pic_size;
			}
			else if(ismjpeg == 1)
			{
				//convert_yuv_to_rgb			
				convert_yuv_to_rgb(yuv_buf,rgb_buf, cam_width,cam_hight, 24);
				//convert_rgb_to_jpg
				memset(jpg, 0, sizeof(jpg));
				if((pic_size=convert_rgb_to_jpg_work(rgb_buf,jpg->jpg_buf,cam_width,cam_hight,24,60))<0)
				{
					perror("convert_rgb_to_jpg error");
					return (void *)0;
				}
				jpg->jpg_size=pic_size;
			}
			
			//send file size
			printf("pic size= %d\n",pic_size);
			char pic_size_buffer[40]={0};
			sprintf(pic_size_buffer,"%d",pic_size);
			write(*client_fd,pic_size_buffer,sizeof(pic_size_buffer));
			
			//send picture
			ret=write(*client_fd,jpg->jpg_buf,pic_size);
			printf("real write:%d\n",ret);
			printf("image send success\n");
			//eqbuf
			if((ret=camera_eqbuf(camera_fd,index))<0)
			{
				perror("eqbuf error");
				return (void *)0;
			}
						
			//index control
			
			if(index==3)
			{
				index=0;
			}
			else
			{
				index++;
			}
		}
		pthread_testcancel();
	}	
	return (void *)0;
}

