/*
 * File: gds.c
 * Author: Roger Light
 * Project: png2gds
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
#include <stdio.h>
#include <assert.h>
#include "gds.h"

void write_gds_all_header(FILE *optr)
{
	assert(optr);

	write_gds_header(optr);
	write_gds_bgnlib(optr);
	write_gds_libname(optr);

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

	write_gds_bgnstr(optr);
	write_gds_strname(optr);
}


void write_gds_bgnlib(FILE *optr)
{
	assert(optr);

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
}

void write_gds_bgnstr(FILE *optr)
{
	assert(optr);

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
}

void write_gds_pixels(FILE *optr, unsigned char layer, unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2)
{
	assert(optr);

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

	write_gds_ulong(optr, x1);
	write_gds_ulong(optr, y1);

	write_gds_ulong(optr, x1);
	write_gds_ulong(optr, y2);

	write_gds_ulong(optr, x2);
	write_gds_ulong(optr, y2);

	write_gds_ulong(optr, x2);
	write_gds_ulong(optr, y1);

	write_gds_ulong(optr, x1);
	write_gds_ulong(optr, y1);

	fputc(0x00, optr);
	fputc(0x04, optr); // 4 bytes long
	fputc(0x11, optr); // ENDEL
	fputc(0x00, optr); // no data
}

void write_gds_endlib(FILE *optr)
{
	assert(optr);

	fputc(0x00, optr);
	fputc(0x04, optr); // 4 bytes long
	fputc(0x04, optr); // ENDLIB
	fputc(0x00, optr); // no data
}

void write_gds_endstr(FILE *optr)
{
	assert(optr);

	fputc(0x00, optr);
	fputc(0x04, optr); // 4 bytes long
	fputc(0x07, optr); // ENDSTR
	fputc(0x00, optr); // no data
}

void write_gds_header(FILE *optr)
{
	assert(optr);

	fputc(0x00, optr);
	fputc(0x06, optr); // 6 bytes long
	fputc(0x00, optr); // HEADER
	fputc(0x02, optr); // two byte int
	fputc(0x00, optr);
	fputc(0x07, optr); // version 600
}

void write_gds_libname(FILE *optr)
{
	assert(optr);

	fputc(0x00, optr);
	fputc(0x0E, optr);
	fputc(0x02, optr); // LIBNAME
	fputc(0x06, optr); // ascii string
	fputc('P', optr);
	fputc('N', optr);
	fputc('G', optr);
	fputc('2', optr);
	fputc('G', optr);
	fputc('D', optr);
	fputc('S', optr);
	fputc('.', optr);
	fputc('D', optr);
	fputc('B', optr);
}

void write_gds_strname(FILE *optr)
{
	assert(optr);

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
}

void write_gds_ulong(FILE *optr, unsigned long num)
{
	unsigned char a, b, c, d;

	assert(optr);

	a = (unsigned char)((num >> 0) & 0xFF);
	b = (unsigned char)((num >> 8) & 0xFF);
	c = (unsigned char)((num >> 16) & 0xFF);
	d = (unsigned char)((num >> 24) & 0xFF);

	fputc(d, optr);
	fputc(c, optr);
	fputc(b, optr);
	fputc(a, optr);
}

