/*
  jpegtoavi

  A simple converter of JPEGs to an AVI/MJPEG animation.

  Copyright (C) 2003 Phillip Bruce <dI77IHd@yahoo.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE


#include "byteswap.h"
#include <stdio.h>
#include "avifmt.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "llist.h"
#include <jni.h>
#include <android/log.h>


/*
  containing name of jpeg file, size in
  bytes, and offset in pending avi file;

  we create a list of these for later
  production of the jpeg data and indices
  in the avi
 */

typedef struct _Jpeg_Data Jpeg_Data;

struct _Jpeg_Data
{
	DWORD size;
	DWORD offset;
	char name[0]; /* i.e. variable length structure */
};


#define JPEG_DATA_SZ (sizeof(DWORD) * 2)


/*
  spc: indicating file sz in bytes, -1 on error
 */

off_t file_sz(char *fn)
{
	struct stat s;
	if (stat(fn, &s) == -1)
		return -1;
	return s.st_size;
}


/*
  spc: returning sum of sizes of named JPEGs, -1 on error;
       file sizes adjusted to multiple of 4-bytes
  pos: l->size fields set to true file size
 */

off_t get_file_sz(List *l)
{
	off_t tmp, ret = 0;

	for(; (l); l = l->next) {
#if VERBOSE >= 2
		printf("determining file size for file == %s\n",
				((Jpeg_Data *) l->data)->name);
#endif

		if((tmp = file_sz(((Jpeg_Data *)l->data)->name)) == -1)
			return -1;
		((Jpeg_Data *)l->data)->size = (DWORD) tmp;
		tmp += ((4-(tmp%4)) % 4);
		ret += tmp;
	}

	return ret;
}


/* 
   spc: for obtaining list of file names from STDIN 
 */

List *get_file_list_stdin(void)
{
	char fn[PATH_MAX];
	Jpeg_Data *tmp;
	List *ret = (List *) malloc(sizeof(List)),
			*l = ret;

	ret->data = 0;
	ret->prev = 0;
	ret->next = 0;

	while (scanf("%s", fn) == 1) {
#if VERBOSE >= 2
		printf("read file name %s on stdin\n", fn);
#endif

		tmp = (Jpeg_Data *) malloc(strlen(fn) + 1 + JPEG_DATA_SZ);
		tmp->offset = 0;
		tmp->size = 0;
		strcpy(tmp->name, fn);
		if (l->data == 0)
			l->data = tmp;
		else
			l = list_push_back(l, tmp);
	}

#if VERBOSE >= 2
	printf("list size == %d\n", list_size(ret));
#endif

	return ret;
}


/*
  spc: for obtaining list of file names from argv
 */

List *get_file_list_argv(int argc, char **argv)
{
	List *ret = (List *) malloc(sizeof(List)),
			*l = ret;
	Jpeg_Data *tmp;
	int i;

	ret->data = 0;
	ret->prev = 0;
	ret->next = 0;

	for (i = 0; i < argc; ++i) {
		tmp = (Jpeg_Data *) malloc(strlen(argv[i]) + 1 + JPEG_DATA_SZ);
		strcpy(tmp->name, argv[i]);
		if (l->data == 0)
			l->data = tmp;
		else
			l = list_push_back(l, tmp);
	}

	return ret;
}


/*
  spc: printing 4 byte word in little-endian fmt
 */

void print_quartet(unsigned int i, FILE *out_fp)
{
	putc(i % 0x100, out_fp);  i /= 0x100;
	putc(i % 0x100, out_fp);  i /= 0x100;
	putc(i % 0x100, out_fp);  i /= 0x100;
	putc(i % 0x100, out_fp);
}


void show_help(int argc, char **argv)
{
	__android_log_print(ANDROID_LOG_DEBUG, "JNI", "jpegtoavi v%d.%d\n\
			Copyright (C) 2003 Phillip Bruce\n\
			\n\
			This is free software; see the source for copying conditions.  There is NO\n\
			warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n\
			USAGE: jpegtoavi {usec per img} {img width} {img height} {img1 .. imgN}\n\
			jpegtoavi -f {fps} {img width} {img height} {img1 .. imgN}\n\
			jpegtoavi -fsz {img1 .. imgN}\n\n\
			Creating an AVI/MJPEG on stdout.\n", 1, 5);
}

int
Java_org_orzlabs_java_media_AviWriter_jpegToAviJNI( JNIEnv* env,
		jobject thiz)
//int main(int argc, char **argv)
{
	int called_no = 0;
	__android_log_print(ANDROID_LOG_DEBUG, "JNI", "%s:%d\n", "called.", called_no++);
	DWORD per_usec = 1;
	DWORD width;
	DWORD height;
	DWORD frames = 1;
	unsigned int fps;
	unsigned short img0;
	off64_t jpg_sz_64, riff_sz_64;
	long jpg_sz = 1;
	const off64_t MAX_RIFF_SZ = 2147483648LL; /* 2 GB limit */
	DWORD riff_sz;
	__android_log_print(ANDROID_LOG_DEBUG, "JNI", "%s:%d\n", "called.", called_no++);

	int fd;
	long nbr, nbw, tnbw = 0;
	char buff[512];
	off_t mfsz, remnant;
	List *frlst = 0,
			*f = 0;
	__android_log_print(ANDROID_LOG_DEBUG, "JNI", "%s:%d\n", "called.", called_no++);

	jclass clazz = (*env)->GetObjectClass(env, thiz);
	jfieldID fid = (*env)->GetFieldID(env ,clazz, "args", "[Ljava/lang/String;");
	jobjectArray *objStrings = (*env)->GetObjectField(env, thiz, fid);
	__android_log_print(ANDROID_LOG_DEBUG, "JNI", "%s:%d\n", "called.", called_no++);

	int argc = (*env)->GetArrayLength(env, objStrings);
	char *argv[argc];
	int i;
	__android_log_print(ANDROID_LOG_DEBUG, "JNI", "%s:%d, argc:%d\n", "called.", called_no++, argc);
	for (i = 0; i < argc; i++) {
		jstring text = (*env)->GetObjectArrayElement(env, objStrings, i);
		argv[i] = (char*) (*env)->GetStringUTFChars(env, text, NULL);
		__android_log_print(ANDROID_LOG_DEBUG, "JNI", "argv[%d]:%s", i, argv[i]);
	}
	__android_log_print(ANDROID_LOG_DEBUG, "JNI", "%s:%d\n", "called.", called_no++);

	struct AVI_list_hdrl hdrl = {
			/* header */
			{
					{'L', 'I', 'S', 'T'},
					LILEND4(sizeof(struct AVI_list_hdrl) - 8),
					{'h', 'd', 'r', 'l'}
			},

			/* chunk avih */
			{'a', 'v', 'i', 'h'},
			LILEND4(sizeof(struct AVI_avih)),
			{
					LILEND4(per_usec),
					LILEND4(1000000 * (jpg_sz/frames) / per_usec),
					LILEND4(0),
					LILEND4(AVIF_HASINDEX),
					LILEND4(frames),
					LILEND4(0),
					LILEND4(1),
					LILEND4(0),
					LILEND4(width),
					LILEND4(height),
					{LILEND4(0), LILEND4(0), LILEND4(0), LILEND4(0)}
			},

			/* list strl */
			{
					{
							{'L', 'I', 'S', 'T'},
							LILEND4(sizeof(struct AVI_list_strl) - 8),
							{'s', 't', 'r', 'l'}
					},

					/* chunk strh */
					{'s', 't', 'r', 'h'},
					LILEND4(sizeof(struct AVI_strh)),
					{
							{'v', 'i', 'd', 's'},
							{'M', 'J', 'P', 'G'},
							LILEND4(0),
							LILEND4(0),
							LILEND4(0),
							LILEND4(per_usec),
							LILEND4(1000000),
							LILEND4(0),
							LILEND4(frames),
							LILEND4(0),
							LILEND4(0),
							LILEND4(0)
					},

					/* chunk strf */
					{'s', 't', 'r', 'f'},
					sizeof(struct AVI_strf),
					{
							LILEND4(sizeof(struct AVI_strf)),
							LILEND4(width),
							LILEND4(height),
							LILEND4(1 + 24*256*256),
							{'M', 'J', 'P', 'G'},
							LILEND4(width * height * 3),
							LILEND4(0),
							LILEND4(0),
							LILEND4(0),
							LILEND4(0)
					},

					/* list odml */
					{
							{
									{'L', 'I', 'S', 'T'},
									LILEND4(16),
									{'o', 'd', 'm', 'l'}
							},
							{'d', 'm', 'l', 'h'},
							LILEND4(4),
							LILEND4(frames)
					}
			}
	};

	/* parsing command line arguments */
	if (argc < 2) {
		show_help(argc, argv);
		__android_log_print(ANDROID_LOG_DEBUG, "JNI", "argc < 2");

		return -1;

	} else if (argc == 2 && strcmp(argv[1], "--version") == 0) {
		printf("jpegtoavi v%d.%d\n", 1, 5);
		return 0;

	} else if (argc < 3) {
		show_help(argc, argv);
		__android_log_print(ANDROID_LOG_DEBUG, "JNI", "argc < 3");

		return -1;

	} else if (strcmp(argv[1], "-fsz") == 0) {
		img0 = 2;

	} else if (strcmp(argv[1], "-f") == 0) {
		if (argc < 5 || sscanf(argv[2], "%u", &fps) != 1
				|| fps == 0
				|| sscanf(argv[3], "%u", &width) != 1
				|| sscanf(argv[4], "%u", &height) != 1) {
			show_help(argc, argv);
			__android_log_print(ANDROID_LOG_DEBUG, "JNI", "argc < 5, other");

			return -1;

		} else {
			per_usec = 1000000 / fps;
			img0 = 5;
		}

	} else if(argc < 4 || sscanf(argv[1], "%u", &per_usec) != 1
			|| sscanf(argv[2], "%u", &width) != 1
			|| sscanf(argv[3], "%u", &height) != 1) {
		show_help(argc, argv);
		__android_log_print(ANDROID_LOG_DEBUG, "JNI", "argc < 4, other");

		return -1;

	} else {
		img0 = 4;
	}

	if (img0 >= argc)
		frlst = get_file_list_stdin();
	else
		frlst = get_file_list_argv(argc - img0 , &argv[img0]);

	frames = list_size(frlst);

	/* getting image, and hence, riff sizes */
	jpg_sz_64 = get_file_sz(frlst);

	if (jpg_sz_64 == -1) {
		fprintf(stderr, "couldn't determine size of images\n");
		return -2;
	}

	riff_sz_64 = sizeof(struct AVI_list_hdrl) + 4 + 4 + jpg_sz_64
			+ 8*frames + 8 + 8 + 16*frames;

	if (riff_sz_64 >= MAX_RIFF_SZ) {
		fprintf(stderr,"RIFF would exceed 2 Gb limit\n");
		return -3;
	}

	jpg_sz = (long) jpg_sz_64;
	riff_sz = (DWORD) riff_sz_64;

	/* printing predicted file size and quitting */
	if (img0 == 2) {
		__android_log_print(ANDROID_LOG_DEBUG, "JNI", "%lu\n", (unsigned long) riff_sz + 8UL);
		return 0;
	}
	FILE *out_fp;
	char *out_file_path = "/sdcard/DCIM/100ANDRO/test.avi";
	out_fp = fopen(out_file_path, "w");
	if (out_fp == NULL) {
		__android_log_print(ANDROID_LOG_ERROR, "JNI", "file open error: %s\n", out_file_path);
		return -1;
	}

	/* printing AVI riff hdr */
	fprintf(out_fp, "RIFF");
	print_quartet(riff_sz, out_fp);
	fprintf(out_fp, "AVI ");

	/* list hdrl */
	hdrl.avih.us_per_frame = LILEND4(per_usec);
	hdrl.avih.max_bytes_per_sec = LILEND4(1000000 * (jpg_sz/frames)
			/ per_usec);
	hdrl.avih.tot_frames = LILEND4(frames);
	hdrl.avih.width = LILEND4(width);
	hdrl.avih.height = LILEND4(height);
	hdrl.strl.strh.scale = LILEND4(per_usec);
	hdrl.strl.strh.rate = LILEND4(1000000);
	hdrl.strl.strh.length = LILEND4(frames);
	hdrl.strl.strf.width = LILEND4(width);
	hdrl.strl.strf.height = LILEND4(height);
	hdrl.strl.strf.image_sz = LILEND4(width * height * 3);
	hdrl.strl.list_odml.frames = LILEND4(frames);	/*  */
	fwrite(&hdrl, sizeof(hdrl), 1, out_fp);

	/* list movi */
	fprintf(out_fp, "LIST");
	print_quartet(jpg_sz + 8*frames + 4, out_fp);
	fprintf(out_fp, "movi");

	/* list movi .. frames */
	for (f = frlst; (f); f = f->next) {
		fprintf(out_fp, "00db");
		mfsz = ((Jpeg_Data *) f->data)->size;
		remnant = (4-(mfsz%4)) % 4;
		print_quartet(mfsz + remnant, out_fp);
		((Jpeg_Data *) f->data)->size += remnant;

		if (f == frlst) {
			((Jpeg_Data *) f->data)->offset = 4;

		} else {
			((Jpeg_Data *) f->data)->offset =
					((Jpeg_Data *) f->prev->data)->offset +
					((Jpeg_Data *) f->prev->data)->size + 8;
		}

		if ((fd = open(((Jpeg_Data *) f->data)->name, O_RDONLY)) < 0) {
			fprintf(stderr, "couldn't open file!\n");
			list_rerased(frlst);
			return -6;
		}
		nbw = 0;

		if ((nbr = read(fd, buff, 6)) != 6) {
			fprintf(stderr, "error\n");
			list_rerased(frlst);
			return -7;
		}
		fwrite(buff, nbr, 1, out_fp);
		read(fd, buff, 4);
		fwrite("AVI1", 4, 1, out_fp);
		nbw = 10;

		while ((nbr = read(fd, buff, 512)) > 0) {
			fwrite(buff, nbr, 1, out_fp);
			nbw += nbr;
		}
		if (remnant > 0) {
			fwrite(buff, remnant, 1, out_fp);
			nbw += remnant;
		}
		tnbw += nbw;
		close(fd);
	}

	if (tnbw != jpg_sz) {
		fprintf(stderr, "error writing images (wrote %ld bytes, expected %ld bytes)\n",
				tnbw, jpg_sz);
		list_rerased(frlst);
		return -8;
	}

	/* indices */
	fprintf(out_fp, "idx1");
	print_quartet(16 * frames, out_fp);
	for (f = frlst; (f); f = f->next) {
		fprintf(out_fp, "00db");
		print_quartet(18, out_fp);
		print_quartet(((Jpeg_Data *) f->data)->offset, out_fp);
		print_quartet(((Jpeg_Data *) f->data)->size, out_fp);
	}

	close(fd);
	fclose(out_fp);
	list_rerased(frlst);
	__android_log_print(ANDROID_LOG_DEBUG, "JNI", "%s:%d\n", "success.", called_no++);

	return 0;
}
