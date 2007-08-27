#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#define ERROR 1
#define PNG2GDS_VERSION "20070827"

void write_startel(FILE *optr, unsigned char layer)
{
	fputc(0x00, optr);
	fputc(0x04, optr); // 4 bytes long
	fputc(0x08, optr); // BOUNDARY
	fputc(0x00, optr); // no data

	fputc(0x00, optr);
	fputc(0x06, optr); // 6 bytes long
	fputc(0x0D, optr); // LAYER
	fputc(0x02, optr); // two byte int
	fputc(0x00, optr);
	fputc(layer, optr);

	fputc(0x00, optr);
	fputc(0x06, optr); // 6 bytes long
	fputc(0x0E, optr); // DATATYPE
	fputc(0x02, optr); // two byte int
	fputc(0x00, optr);
	fputc(0x00, optr);

	fputc(0x00, optr);
	fputc(44, optr); // 44 bytes long
	fputc(0x10, optr); // XY
	fputc(0x03, optr); // four byte int
}


void write_endel(FILE *optr, unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2)
{
	unsigned char x1a, x1b, x1c, x1d;
	unsigned char x2a, x2b, x2c, x2d;
	unsigned char y1a, y1b, y1c, y1d;
	unsigned char y2a, y2b, y2c, y2d;

	x1a = (unsigned char)((x1 >> 0) & 0xFF);
	x1b = (unsigned char)((x1 >> 8) & 0xFF);
	x1c = (unsigned char)((x1 >> 16) & 0xFF);
	x1d = (unsigned char)((x1 >> 24) & 0xFF);

	x2a = (unsigned char)((x2 >> 0) & 0xFF);
	x2b = (unsigned char)((x2 >> 8) & 0xFF);
	x2c = (unsigned char)((x2 >> 16) & 0xFF);
	x2d = (unsigned char)((x2 >> 24) & 0xFF);

	y1a = (unsigned char)((y1 >> 0) & 0xFF);
	y1b = (unsigned char)((y1 >> 8) & 0xFF);
	y1c = (unsigned char)((y1 >> 16) & 0xFF);
	y1d = (unsigned char)((y1 >> 24) & 0xFF);

	y2a = (unsigned char)((y2 >> 0) & 0xFF);
	y2b = (unsigned char)((y2 >> 8) & 0xFF);
	y2c = (unsigned char)((y2 >> 16) & 0xFF);
	y2d = (unsigned char)((y2 >> 24) & 0xFF);

	fputc(x1d, optr); // X1
	fputc(x1c, optr);
	fputc(x1b, optr);
	fputc(x1a, optr);
	fputc(y1d, optr); // Y1
	fputc(y1c, optr);
	fputc(y1b, optr);
	fputc(y1a, optr);

	fputc(x1d, optr); // X1
	fputc(x1c, optr);
	fputc(x1b, optr);
	fputc(x1a, optr);
	fputc(y2d, optr); // Y2
	fputc(y2c, optr);
	fputc(y2b, optr);
	fputc(y2a, optr);

	fputc(x2d, optr); // X2
	fputc(x2c, optr);
	fputc(x2b, optr);
	fputc(x2a, optr);
	fputc(y2d, optr); // Y2
	fputc(y2c, optr);
	fputc(y2b, optr);
	fputc(y2a, optr);

	fputc(x2d, optr); // X2
	fputc(x2c, optr);
	fputc(x2b, optr);
	fputc(x2a, optr);
	fputc(y1d, optr); // Y1
	fputc(y1c, optr);
	fputc(y1b, optr);
	fputc(y1a, optr);

	fputc(x1d, optr); // X1
	fputc(x1c, optr);
	fputc(x1b, optr);
	fputc(x1a, optr);
	fputc(y1d, optr); // Y1
	fputc(y1c, optr);
	fputc(y1b, optr);
	fputc(y1a, optr);

	fputc(0x00, optr);
	fputc(0x04, optr); // 4 bytes long
	fputc(0x11, optr); // ENDEL
	fputc(0x00, optr); // no data
}

int write_gds(const char *infile, const char *outfile, float grid)
{
	FILE *optr;
	FILE *fp = fopen(infile, "rb");
	png_uint_32 width, height;
	if(!fp){
		return (ERROR);
	}

	optr = fopen(outfile, "wb");
	if(!optr){
		fclose(fp);
		return -1;
	}

	png_structp png_ptr = png_create_read_struct
			(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr){
		fclose(fp);
		return (ERROR);
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr){
		png_destroy_read_struct(&png_ptr,
				(png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return (ERROR);
	}

	png_infop end_info = png_create_info_struct(png_ptr);
	if(!end_info){
		png_destroy_read_struct(&png_ptr, &info_ptr,
				(png_infopp)NULL);
		fclose(fp);
		return (ERROR);
	}

	if(setjmp(png_jmpbuf(png_ptr))){
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return (ERROR);
	}

	rewind(fp);
	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr, 0, NULL);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);

	png_bytepp row_pointers;//[height];
	row_pointers = png_get_rows(png_ptr, info_ptr);

	/* Write gds2 */

	fputc(0x00, optr);
	fputc(0x06, optr); // 6 bytes long
	fputc(0x00, optr); // HEADER
	fputc(0x02, optr); // two byte int
	fputc(0x00, optr);
	fputc(0x07, optr); // version 600

	fputc(0x00, optr);
	fputc(0x1C, optr); // 28 bytes long
	fputc(0x01, optr); // BGNLIB
	fputc(0x02, optr); // two byte int
	fputc(0x07, optr);
	fputc(0xd1, optr);
	fputc(0x00, optr);
	fputc(0x04, optr);
	fputc(0x00, optr);
	fputc(0x13, optr);
	fputc(0x00, optr);
	fputc(0x0f, optr);
	fputc(0x00, optr);
	fputc(0x33, optr);
	fputc(0x00, optr);
	fputc(0x26, optr);
	fputc(0x07, optr);
	fputc(0xd5, optr);
	fputc(0x00, optr);
	fputc(0x02, optr);
	fputc(0x00, optr);
	fputc(0x12, optr);
	fputc(0x00, optr);
	fputc(0x0f, optr);
	fputc(0x00, optr);
	fputc(0x2a, optr);
	fputc(0x00, optr);
	fputc(0x09, optr);

	fputc(0x00, optr);
	fputc(0x0E, optr);
	fputc(0x02, optr); // LIBNAME
	fputc(0x06, optr); // ascii string
	fputc('P', optr);
	fputc('H', optr);
	fputc('I', optr);
	fputc('L', optr);
	fputc('I', optr);
	fputc('P', optr);
	fputc('S', optr);
	fputc('.', optr);
	fputc('D', optr);
	fputc('B', optr);

	/*fputc(0x00, optr);
	fputc(0x0B, optr);
	fputc(0x02, optr); // LIBNAME
	fputc(0x06, optr); // ascii string
	fputc('g', optr);
	fputc('d', optr);
	fputc('s', optr);
	fputc('2', optr);
	fputc('p', optr);
	fputc('n', optr);
	fputc('g', optr);
	//fputs("gds2png", optr); 
	*/

	fputc(0x00, optr);
	fputc(0x14, optr);
	fputc(0x03, optr); // UNITS
	fputc(0x05, optr); // eight byte real

	fputc(0x3E, optr);
	fputc(0x41, optr);
	fputc(0x89, optr);
	fputc(0x37, optr);
	fputc(0x4B, optr);
	fputc(0xC6, optr);
	fputc(0xA7, optr);
	fputc(0xF0, optr);

	fputc(0x39, optr);
	fputc(0x44, optr);
	fputc(0xB8, optr);
	fputc(0x2F, optr);
	fputc(0xA0, optr);
	fputc(0x9B, optr);
	fputc(0x5A, optr);
	fputc(0x54, optr);

	fputc(0x00, optr);
	fputc(0x1C, optr); // 28 bytes long
	fputc(0x05, optr); // BGNSTR
	fputc(0x02, optr); 
	fputc(0x00, optr);
	fputc(0x64, optr);
	fputc(0x00, optr);
	fputc(0x05, optr);
	fputc(0x00, optr);
	fputc(0x02, optr);
	fputc(0x00, optr);
	fputc(0x0f, optr);
	fputc(0x00, optr);
	fputc(0x28, optr);
	fputc(0x00, optr);
	fputc(0x38, optr);
	fputc(0x07, optr);
	fputc(0xD5, optr);
	fputc(0x00, optr);
	fputc(0x02, optr);
	fputc(0x00, optr);
	fputc(0x12, optr);
	fputc(0x00, optr);
	fputc(0x0f, optr);
	fputc(0x00, optr);
	fputc(0x2a, optr);
	fputc(0x00, optr);
	fputc(0x09, optr);

	fputc(0x00, optr);
	fputc(0x0C, optr); // 11 bytes long
	fputc(0x06, optr); // STRNAME
	fputc(0x06, optr); // ascii string
	fputc('p', optr);
	fputc('n', optr);
	fputc('g', optr);
	fputc('2', optr);
	fputc('g', optr);
	fputc('d', optr);
	fputc('s', optr);
	fputc('\0', optr);

	unsigned long x, y;
	unsigned long x1, x2, y1, y2;
	char first = 1;
	png_byte lastlayer, thislayer;
	char in_el = 0;

	for(y = 0; y < height; y++){
		first = 1;
		for(x = 0; x < width; x++){
			thislayer = row_pointers[y][x];
			if(!first && thislayer != lastlayer){
				if(lastlayer != 255){
					x2 = x * 1000 * grid;
					write_endel(optr, x1, y1, x2, y2);
					in_el = 0;
				}
				x1 = (x + 0) * 1000 * grid; // 20 == 0.02 * 1000
				y1 = (y + 0) * 1000 * grid;
				y2 = (y + 1) * 1000 * grid;

				if(thislayer != 255){
					write_startel(optr, thislayer);
					in_el = 1;
				}
			}
			if(first && thislayer != 255){
				write_startel(optr, thislayer);
				in_el = 1;
				first = 0;
				x1 = (x + 0) * 1000 * grid; // 20 == 0.02 * 1000
				y1 = (y + 0) * 1000 * grid;
				y2 = (y + 1) * 1000 * grid;
			}
			lastlayer = thislayer;
		}
		if(in_el){
			x2 = x * 1000 * grid;
			write_endel(optr, x1, y1, x2, y2);
		}
	}

	fputc(0x00, optr);
	fputc(0x04, optr); // 4 bytes long
	fputc(0x07, optr); // ENDSTR
	fputc(0x00, optr); // no data

	fputc(0x00, optr);
	fputc(0x04, optr); // 4 bytes long
	fputc(0x04, optr); // ENDLIB
	fputc(0x00, optr); // no data

	fclose(optr);

	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	fclose(fp);

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
	if(argc!=4){
		printusage();
		return 1;
	}

	return write_gds(argv[1], argv[2], atof(argv[3]));
}
