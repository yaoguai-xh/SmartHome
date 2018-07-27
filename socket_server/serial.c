/*===============================================================
*   Copyright (C) 2018 All rights reserved.
*
*   文件名称：ser.c
*   创 建 者：heshijie
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
#include <pthread.h>
#include "serial.h"
#define PATH "/dev/ttyUSB0"
//#define FD 3
#define SPEED 115200
#define STOP 1
#define SIZE 36
#define BUFFER_SIZE 1024
int FD=0x03;

	char RecvBuffer[BUFFER_SIZE];
	char SendBuffer[BUFFER_SIZE];
	char recBuf[36] = {0};
	char sendBuf[36] = {0};
	int recFd, openFd, setFd, initFd, closeFd, sendFd;


void *pthread_ser(void *arg)
{
	while(1)
	{
		//read serial data
		recFd = serial_recv_exact_nbytes(initFd, recBuf, SIZE);
		// receive data test
		/*
		int i = 0;
		printf("size of recBuf is : %d\n", strlen(recBuf));
		while(i < 36){
			printf("%d:%x\t\t\t",i+1,recBuf[i++]);
			if(i % 2 == 0)
				printf("\n");
		}*/
		int n1 = recBuf[20];
		n1 &= 0x000000ff;
		sensor_state.temp = recBuf[5];
		sensor_state.humi = recBuf[7];

		int n2 = recBuf[21], n3 = recBuf[22], n4 = recBuf[23];

		printf("\n");
		if(recFd == -1){
			printf("recBuf receive failed!\n");
			return (void *)-1;
		}
		if(recBuf[1] != 0x03){
			printf("received data Id error~\n");
			return (void *)-1;
		}
		if(recBuf[2] != 0x24){
			printf("received data Len error~\n");
			return (void *)-1;
		}
		sensor_state.light = n1 + (n2<<8);
	
	}
}
void *pthread_env(void *arg)
{
	int *client_fd=arg;

	memset(sendBuf,0,sizeof(sendBuf));
	sendBuf[0] = 0xdd; sendBuf[1] = 0x03; sendBuf[2] = 0x24;
	
	// Init Serial
	if((initFd = serial_init(PATH)) == -1){
		printf("Init failed!\n");
		return (void *)-1;
	}
	
	pthread_t th_ser;
	void *ret_ser=NULL;
	pthread_create(&th_ser,NULL,pthread_ser,NULL);

	while(1)
	{
		//read socket data
		memset(RecvBuffer,0,sizeof(RecvBuffer));
		read(*client_fd,RecvBuffer,sizeof(RecvBuffer));
		
		if(strstr("ENV_SEND",RecvBuffer)!=NULL)
		{
			printf("recBuf received: %s \n", recBuf);
			printf("RecvBuffer received: %s\n",RecvBuffer);
			memset(SendBuffer,0,sizeof(SendBuffer));
			sprintf(SendBuffer,"%dt%dh%dl",sensor_state.temp,sensor_state.humi,sensor_state.light);
			write(*client_fd,SendBuffer,sizeof(SendBuffer));
		}
		else if(strstr("LED_ON",RecvBuffer)!=NULL)
		{
			sendBuf[4] = 0x00;
			printf("recBuf received: %s \n", recBuf);
			printf("RecvBuffer received: %s\n",RecvBuffer);
			if((sendFd = serial_send_exact_nbytes(initFd, sendBuf, SIZE)) == -1)
			{
				printf("Send failed!\n");
				return (void *)-1;
			}
			printf("send success!\n");
		}
		else if(strstr("LED_OFF",RecvBuffer)!=NULL)
		{
			sendBuf[4] = 0x01;
			printf("recBuf received: %s \n", recBuf);
			printf("RecvBuffer received: %s\n",RecvBuffer);
			if((sendFd = serial_send_exact_nbytes(initFd, sendBuf, SIZE)) == -1)
			{
				printf("Send failed!\n");
				return (void *)-1;
			}
			printf("send success!\n");
		}
		else if(strstr("FAN_ON",RecvBuffer)!=NULL)
		{
			sendBuf[4] = 0x04;
			printf("recBuf received: %s \n", recBuf);
			printf("RecvBuffer received: %s\n",RecvBuffer);
			if((sendFd = serial_send_exact_nbytes(initFd, sendBuf, SIZE)) == -1)
			{
				printf("Send failed!\n");
				return (void *)-1;
			}
			printf("send success!\n");
		}
		else if(strstr("FAN_OFF",RecvBuffer)!=NULL)
		{
			sendBuf[4] = 0x08;
			printf("recBuf received: %s \n", recBuf);
			printf("RecvBuffer received: %s\n",RecvBuffer);
			if((sendFd = serial_send_exact_nbytes(initFd, sendBuf, SIZE)) == -1)
			{
				printf("Send failed!\n");
				return (void *)-1;
			}
			printf("send success!\n");
		}
		else if(strstr("BEEP_ON",RecvBuffer)!=NULL)
		{
			sendBuf[4] = 0x02;
			printf("recBuf received: %s \n", recBuf);
			printf("RecvBuffer received: %s\n",RecvBuffer);
			if((sendFd = serial_send_exact_nbytes(initFd, sendBuf, SIZE)) == -1)
			{
				printf("Send failed!\n");
				return (void *)-1;
			}
			printf("send success!\n");
		}
		else if(strstr("BEEP_OFF",RecvBuffer)!=NULL)
		{
			sendBuf[4] = 0x03;
			printf("recBuf received: %s \n", recBuf);
			printf("RecvBuffer received: %s\n",RecvBuffer);
			if((sendFd = serial_send_exact_nbytes(initFd, sendBuf, SIZE)) == -1)
			{
				printf("Send failed!\n");
				return (void *)-1;
			}
			printf("send success!\n");
		}
		
		

		pthread_testcancel();
	}
	int exitFd;
	if((exitFd = serial_exit(initFd)) == -1){
		printf("exit failed~\n");
		return 0;
	}
	return (void *)0;
}
