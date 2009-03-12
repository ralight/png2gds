/*
 * File: png2gds
 * Author: Roger Light
 * Project: png2gds
 *
 * This is the main body of the png2gds program.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "gds.h"
#include "readpng.h"

#define ERROR 1
#define PNG2GDS_VERSION "20070827"
#define DBUNITS 1000

png_uint_32 width, height;

png_byte *read_png(const char *infile)
{
	png_byte bg_red, bg_green, bg_blue;
	int channels;
	png_uint_32 row_bytes;
	png_byte *image_data = NULL;

	FILE *fp = fopen(infile, "rb");

	if(!fp){
		return NULL;
	}

	if(readpng_init(fp, &width, &height)){
		fclose(fp);
		return NULL;
	}

#if 0
	if(readpng_get_bgcolor(&bg_red, &bg_green, &bg_blue)){
		/* do something! */
		printf("Background colour not found.\n");
	}else{
		printf("Background: %d %d %d\n", bg_red, bg_green, bg_blue);
	}
#endif

	image_data = readpng_get_image(1.0, &channels, &row_bytes);
	if(!image_data){
		printf("Invalid png file.\n");
		fclose(fp);
		return NULL;
	}
	//printf("Channels: %d\nRow Bytes: %ld\n", channels, (long)row_bytes);

	fclose(fp);
	return image_data;
}


int write_output(png_byte *image_data, const char *outfile, float grid)
{
	FILE *optr;

	if(!strcmp(outfile, "stdout")){
		optr = stdout;
	}else{
		optr = fopen(outfile, "wb");
		if(!optr){
			readpng_cleanup(TRUE);

			return -1;
		}
	}


	unsigned long x, y;
	unsigned long x1, x2, y1, y2;
	char first = 1;
	png_byte lastlayer, thislayer;
	char in_el = 0;

	for(y = 0; y < height; y++){
		first = 1;
		for(x = 0; x < width; x++){
			thislayer = image_data[(height - y - 1)*width + x];

			if(!first && thislayer != lastlayer){
				if(lastlayer != 255){
					x2 = x * DBUNITS * grid;
					if(optr == stdout){
						if(fabs(grid - 1.0) < 0.01){
							fprintf(optr, "%d %ld %ld %ld %ld\n", lastlayer, 
									x1/DBUNITS, y1/DBUNITS, x2/DBUNITS, y2/DBUNITS);
						}else{
							fprintf(optr, "%d %f %f %f %f\n", lastlayer, 
									((float)x1)/DBUNITS, ((float)y1)/DBUNITS, ((float)x2)/DBUNITS, ((float)y2)/DBUNITS);
						}
						fflush(optr);

					}else{
						write_gds_pixels(optr, lastlayer, x1, y1, x2, y2);
					}
					in_el = 0;
				}
				x1 = (x + 0) * DBUNITS * grid;
				y1 = (y + 0) * DBUNITS * grid;
				y2 = (y + 1) * DBUNITS * grid;

				if(thislayer != 255){
					in_el = 1;
				}
			}
			if(first && thislayer != 255){
				in_el = 1;
				first = 0;
				x1 = (x + 0) * DBUNITS * grid;
				y1 = (y + 0) * DBUNITS * grid;
				y2 = (y + 1) * DBUNITS * grid;
			}
			lastlayer = thislayer;
		}
		if(in_el){
			x2 = x * DBUNITS * grid;
			if(optr == stdout){
				if(fabs(grid - 1.0) < 0.01){
					fprintf(optr, "%d %ld %ld %ld %ld\n", lastlayer, 
							x1/DBUNITS, y1/DBUNITS, x2/DBUNITS, y2/DBUNITS);
				}else{
					fprintf(optr, "%d %f %f %f %f\n", lastlayer, 
							((float)x1)/DBUNITS, ((float)y1)/DBUNITS, ((float)x2)/DBUNITS, ((float)y2)/DBUNITS);
				}
				fflush(optr);
			}else{
				write_gds_pixels(optr, lastlayer, x1, y1, x2, y2);
			}
		}
	}

	if(optr != stdout){
		write_gds_endstr(optr);
		write_gds_endlib(optr);
	}

	fclose(optr);

	readpng_cleanup(TRUE);

	return 0;
}


void printusage()
{
	printf("png2gds  version %s\n", VERSION);
	printf("Copyright (C) 2007 by Roger Light\nhttp://atchoo.org/tools/png2gds/\n\n");
	printf("png2gds comes with ABSOLUTELY NO WARRANTY.  You may distribute png2gds freely\nas described in the readme.txt distributed with this file.\n\n");
	printf("png2gds is a program for converting a PNG image file to a GDS2 file. Each pixel in the image that is not the background colour will be represented as a square in the GDS2 file which means that the resulting file will be quite large. Merging the squares into a single shape in your CAD software will remove this issue.\n\n");
	printf("Usage: png2gds input.png output.gds grid\n\n");
	printf("Arguments\n");
	printf(" input.png\tThe file to convert into GDS2 format. Indexed palettes are best!\n");
	printf(" output.gds\tThe output file.\n");
	printf(" grid\t\tFloating point number giving the size in microns that each pixel on the image will be.\n");
	printf("\nSee http://atchoo.org/tools/png2gds/ for updates.\n");
}


int main(int argc, char* argv[])
{
	png_byte *image_data = NULL;

	if(argc!=4){
		printusage();
		return 1;
	}
	image_data = read_png(argv[1]);
	if(image_data){
		return write_output(image_data, argv[2], atof(argv[3]));
	}

	return 1;
}
