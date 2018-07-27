/*===============================================================
*   Copyright (C) 2018 All rights reserved.
*   
*   文件名称：server.c
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
#include "serial.h"
#define BUFFER_SIZE 1024

int main(int argc,char *argv[])
{
	/*create socket*/
	int socket_fd;
	socket_fd=socket(AF_INET,SOCK_STREAM,0);
	if(socket_fd == -1)
	{
		perror("socket create error");
		return -1;
	}
	printf("create--success\n");
	/*bind socket*/
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	server_addr.sin_addr.s_addr=inet_addr(argv[1]);
	server_addr.sin_port=htons(8888);
	
	if(bind(socket_fd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0)
	{
		perror("socket bind error");
		return -1;
	}
	printf("bind--success\n");

	//listen two socket
	int count=0;
	int client_fd[2];
	while(1)
	{
		/*listen socket*/
		if(listen(socket_fd,5)<0)
		{
			perror("soket listen error");
			return -1;
		}
		printf("listen--success\n");

		/*accept connection*/
		
		struct sockaddr_in  client_addr;
		int len = sizeof(client_addr);
		client_fd[count] = accept(socket_fd,(struct sockaddr*)&client_addr,&len);
		if(client_fd[count] == -1)
		{
			perror("socket accept connection error");			
			return -1;
		}
		printf("ip:%s--port:%u\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		count ++;
		printf("accept--success--conn%d\n",count);
		if(count ==2)
		{
			break;
		}
	}
	
	/*data receive and data send*/

	pthread_t th_env;
	pthread_t th_pic;
	void *ret_env=NULL;
	void *ret_pic=NULL;
	pthread_create(&th_env,NULL,pthread_env,&client_fd[0]);
	pthread_create(&th_pic,NULL,pthread_pic,&client_fd[1]);
	while(1)
	{
	
		char str[10]={0};
		gets(str);
		if(strncmp(str,"quit",4) == 0)
		{
			convert_rgb_to_jpg_exit();
			//quit thread env
			pthread_cancel(th_env);
			pthread_join(th_env,&ret_env);
			printf("thread env exit success\n");
			//quit thread env
			pthread_cancel(th_pic);
			pthread_join(th_pic,&ret_pic);
			printf("thread pic exit success\n");
			break;
		}
	}	
	/*close socket*/
	close(socket_fd);
	close(client_fd[0]);
	close(client_fd[1]);
	return 0;
}
