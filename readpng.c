/*---------------------------------------------------------------------------

   rpng - simple PNG display program                              readpng.c

  ---------------------------------------------------------------------------

      Copyright (c) 1998-2007 Greg Roelofs.  All rights reserved.

      This software is provided "as is," without warranty of any kind,
      express or implied.  In no event shall the author or contributors
      be held liable for any damages arising in any way from the use of
      this software.

      The contents of this file are DUAL-LICENSED.  You may modify and/or
      redistribute this software according to the terms of one of the
      following two licenses (at your option):


      LICENSE 1 ("BSD-like with advertising clause"):

      Permission is granted to anyone to use this software for any purpose,
      including commercial applications, and to alter it and redistribute
      it freely, subject to the following restrictions:

      1. Redistributions of source code must retain the above copyright
         notice, disclaimer, and this list of conditions.
      2. Redistributions in binary form must reproduce the above copyright
         notice, disclaimer, and this list of conditions in the documenta-
         tion and/or other materials provided with the distribution.
      3. All advertising materials mentioning features or use of this
         software must display the following acknowledgment:

            This product includes software developed by Greg Roelofs
            and contributors for the book, "PNG: The Definitive Guide,"
            published by O'Reilly and Associates.


      LICENSE 2 (GNU GPL v2 or later):

      This program is free software; you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation; either version 2 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program; if not, write to the Free Software Foundation,
      Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "png.h"        /* libpng header; includes zlib.h */
#include "readpng.h"    /* typedefs, common macros, public prototypes */

static png_structp png_ptr = NULL;
static png_infop info_ptr = NULL;

static int  bit_depth, color_type;
static uint8_t  *image_data = NULL;


/* return value = 0 for success, 1 for bad sig, 2 for bad IHDR, 4 for no mem */

int readpng_init(FILE *infile, png_uint_32 *pWidth, png_uint_32 *pHeight)
{
    uint8_t sig[8];


    /* first do a quick check that the file really is a PNG image; could
     * have used slightly more general png_sig_cmp() function instead */

    if(fread(sig, 1, 8, infile) != 8){
		return 1;
	}
    if (!png_check_sig(sig, 8))
        return 1;   /* bad signature */


    /* could pass pointers to user-defined error handlers instead of NULLs: */

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        return 4;   /* out of memory */

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 4;   /* out of memory */
    }


    /* we could create a second info struct here (end_info), but it's only
     * useful if we want to keep pre- and post-IDAT chunk info separated
     * (mainly for PNG-aware image editors and converters) */


    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 2;
    }


    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8);  /* we already read the 8 signature bytes */

    png_read_info(png_ptr, info_ptr);  /* read all PNG info up to image data */


    /* alternatively, could make separate calls to png_get_image_width(),
     * etc., but want bit_depth and color_type for later [don't care about
     * compression_type and filter_type => NULLs] */

    png_get_IHDR(png_ptr, info_ptr, pWidth, pHeight, &bit_depth, &color_type,
      NULL, NULL, NULL);


    /* OK, that's all we need for now; return happy */

    return 0;
}




/* returns 0 if succeeds, 1 if fails due to no bKGD chunk, 2 if libpng error;
 * scales values to 8-bit if necessary */

int readpng_get_bgcolor(uint8_t *red, uint8_t *green, uint8_t *blue)
{
    png_color_16p pBackground;


    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 2;
    }


    if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD))
        return 1;

    /* it is not obvious from the libpng documentation, but this function
     * takes a pointer to a pointer, and it always returns valid red, green
     * and blue values, regardless of color_type: */

    png_get_bKGD(png_ptr, info_ptr, &pBackground);


    /* however, it always returns the raw bKGD data, regardless of any
     * bit-depth transformations, so check depth and adjust if necessary */

    if (bit_depth == 16) {
        *red   = pBackground->red   >> 8;
        *green = pBackground->green >> 8;
        *blue  = pBackground->blue  >> 8;
    } else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        if (bit_depth == 1)
            *red = *green = *blue = pBackground->gray? 255 : 0;
        else if (bit_depth == 2)
            *red = *green = *blue = (255/3) * pBackground->gray;
        else /* bit_depth == 4 */
            *red = *green = *blue = (255/15) * pBackground->gray;
    } else {
        *red   = (uint8_t)pBackground->red;
        *green = (uint8_t)pBackground->green;
        *blue  = (uint8_t)pBackground->blue;
    }

    return 0;
}




uint8_t *readpng_get_image(int *pChannels, png_uint_32 *pRowbytes, png_uint_32 height)
{
    png_uint_32  i, rowbytes;
    png_bytepp  row_pointers = NULL;


    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }


	/* Reject RGB images.
	 * Strip 16-bit-per-sample images to 8 bits per sample.
	 * Pack 1, 2, 4 bit/pixel images to 8 bit/pixel.
	 * Strip alpha.
	 */
    /* expand palette images to RGB, low-bit-depth grayscale images to 8 bits,
     * transparency chunks to full alpha channel; strip 16-bit-per-sample
     * images to 8 bits per sample; and convert grayscale to RGB[A] */

	if(color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA){
		return NULL;
	}

    if (bit_depth == 16){
        png_set_strip_16(png_ptr);
	}
	png_set_strip_alpha(png_ptr);
	png_set_packing(png_ptr);


    /* all transformations have been registered; now update info_ptr data,
     * get rowbytes and channels, and allocate image memory */

    png_read_update_info(png_ptr, info_ptr);

    *pRowbytes = rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    *pChannels = (int)png_get_channels(png_ptr, info_ptr);

    if ((image_data = (uint8_t *)malloc(rowbytes*height)) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        image_data = NULL;
        return NULL;
    }

    /* set the individual row_pointers to point at the correct offsets */

    for (i = 0;  i < height;  ++i)
        row_pointers[i] = image_data + i*rowbytes;


    /* now we can go ahead and just read the whole image */

    png_read_image(png_ptr, row_pointers);


    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */

    free(row_pointers);
    row_pointers = NULL;

    png_read_end(png_ptr, NULL);

    return image_data;
}


void readpng_cleanup(void)
{
	free(image_data);
	image_data = NULL;

    if (png_ptr && info_ptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        png_ptr = NULL;
        info_ptr = NULL;
    }
}
