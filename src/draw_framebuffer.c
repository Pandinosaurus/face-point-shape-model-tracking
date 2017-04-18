/*
 * draw_framebuffer.c
 *
 *  Created on: Dec 24, 2015
 *      Author: Lincoln
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h> /* for frame buffer */
#include <sys/mman.h>
#include <unistd.h>

static int fd = -1;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
static char* fbp = NULL;
static unsigned int screensize = 0;

void free_framebuffer() {
	munmap(fbp, screensize);
	close(fd);
}

void init_framebuffer() {
	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1) {
		perror("Error opening framebuffer device");
		exit(EXIT_FAILURE);
	}
	// Get fixed screen information
	if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
		perror("Error reading fixed information");
		exit(EXIT_FAILURE);
	}
	// Get variable screen information
	if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		perror("Error reading variable information");
		exit(EXIT_FAILURE);
	}
	screensize = (vinfo.xres * vinfo.yres * vinfo.bits_per_pixel) >> 3; /* (>>3): bits to bytes */
	fbp = (char *) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if ((int) fbp == -1) {
		perror("Error mapping framebuffer device to memory");
		exit(EXIT_FAILURE);
	}
}

void draw_framebuffer(unsigned char* src){
	int x, y;
	unsigned int location = 0;
	int i = 0;
	for(y = 0; y < 480; y++){
		for(x = 0; x < 640; x++){
			location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel >> 3) + (y + vinfo.yoffset) * finfo.line_length;
			*(fbp + location) = src[i];		//B
			*(fbp + location + 1) = src[i];	//G
			*(fbp + location + 2) = src[i];	//R
			i++;
		}
	}
}
