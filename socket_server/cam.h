/*===============================================================
*   Copyright (C) 2017 All rights reserved.
*   
*   文件名称：cam.h
*   创 建 者：xuhao
*   创建日期：2018年7月10日
*   描    述：
*
*   更新日志：
*
================================================================*/
#ifndef _CAM_H_
#define _CAM_H_

#define JPG_MAX_SIZE	(1024 * 1024)
#define BUFFER_SIZE 1024

#define W 640
#define H 480

void *pthread_pic(void *arg);

int camera_init(char *devpath, unsigned int *width, unsigned int *height, unsigned int *size, unsigned int *ismjpeg);
int camera_start(int fd);
int camera_dqbuf(int fd, void **buf, unsigned int *size, unsigned int *index);
int camera_eqbuf(int fd, unsigned int index);
int camera_stop(int fd);
int camera_exit(int fd);
int camera_on();

void convert_yuv_to_rgb(void *yuv, void *rgb, unsigned int width, unsigned int height, unsigned int bps);
void convert_rgb_to_jpg_init(void);
int convert_rgb_to_jpg_work(void *rgb, void *jpeg, unsigned int width, unsigned int height, unsigned int bpp, int quality);
void convert_rgb_to_jpg_exit(void);


struct jpg_buf_t {
	char jpg_buf[JPG_MAX_SIZE];
	unsigned int jpg_size;
};
#endif
