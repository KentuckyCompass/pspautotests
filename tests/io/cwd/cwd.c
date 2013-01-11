/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - Basic sample to demonstrate some fileio functionality.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 * Copyright (c) 2005 Jim Paris <jim@jtan.com>
 *
 * $Id: main.c 1175 2005-10-20 15:41:33Z chip $
 */
#include <common.h>

#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <psptypes.h>
#include <pspiofilemgr.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include "sysmem-imports.h"

void list_cwd()
{
	SceIoDirent file;
	int fd;

	fd = sceIoDopen(".");
	if (fd < 0)
	{
		printf("Failed to open \".\" with error: %X\n", fd);
		return;
	}

	while (1) {
		if (sceIoDread(fd, &file) <= 0) break;
		printf(
			//"'%s':%lld:%d:%d\n",
			"'%s'\n",
			file.d_name//,
			//file.d_stat.st_size,
			//file.d_stat.st_attr,
			//file.d_stat.st_mode
		);
	}
	sceIoDclose(fd);

}

static int thread3Func (SceSize args, void* argp)
{
	printf("\n%s\n", "Thread 3 starting, current directory:");
	list_cwd();

	printf("\n%s\n", "Thread 3 exiting");

	return 0;
}

static int thread2Func (SceSize args, void* argp)
{
	printf("\n%s\n", "Thread 2 starting, current directory:");
	list_cwd();

	printf("\n%s\n", "Thread 2 exiting");

	return 0;
}

static int thread1Func (SceSize args, void* argp)
{
	printf("\n%s\n", "Thread 1 changing to host0:/");
	sceIoChdir("host0:/");

	printf("\n%s\n", "Thread 1 listing current directory");
	list_cwd();

	int thread2 = sceKernelCreateThread("thread2Func", &thread2Func, 0x11, 0x10000, 0, NULL);
	sceKernelStartThread(thread2, 0, NULL);

	sceKernelDelayThread(100 * 1000);

	printf("\n%s\n", "Thread 1 exiting (here's another listing)");
	list_cwd();

	//sceKernelWaitThreadEnd(thread2, NULL);

	return 0;
}

int main(int argc, char *argv[])
{
	//sceKernelSetCompiledSdkVersion606(0x6060010);

	int thread1 = sceKernelCreateThread("thread1Func", &thread1Func, 0x12, 0x10000, 0, NULL);

	printf("\n%s\n", "main() listing current directory");
	list_cwd();

	sceKernelStartThread(thread1, 0, NULL);

	// Give Thread1 a chance to change current working directory
	sceKernelDelayThread(25 * 1000);

	printf("\n%s\n", "main() listing current directory");
	list_cwd();

	int thread3 = sceKernelCreateThread("thread3Func", &thread3Func, 0x12, 0x10000, 0, NULL);
	sceKernelStartThread(thread3, 0, NULL);

	printf("\n%s\n", "main() is finished - now waiting for threads");
	
	sceKernelWaitThreadEnd(thread3, NULL);
	sceKernelWaitThreadEnd(thread1, NULL);

	printf("\nAll done!\n");

	return 0;
}
