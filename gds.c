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
#include <stdint.h>
#include "gds.h"

static void gds_write_ulong(FILE *optr, unsigned long num)
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

void gds_write_start(FILE *optr)
{
	const uint8_t gds_header[] = {
		0x00, 0x06, // 6 bytes long
		0x00, // HEADER
		0x02, // two byte int
		0x00, 0x07, // version 600
	};
	const uint8_t gds_bgnlib[] = {
		0x00, 0x1C, // 28 bytes long
		0x01, // BGNLIB
		0x02, // two byte int
		0x07, 0xd1, 0x00, 0x04, 0x00, 0x13, 0x00, 0x0f,
		0x00, 0x33, 0x00, 0x26, 0x07, 0xd5, 0x00, 0x02,
		0x00, 0x12, 0x00, 0x0f, 0x00, 0x2a, 0x00, 0x09
	};
	const uint8_t gds_libname[] = {
		0x00, 0x0E, // 14 bytes long
		0x02, // LIBNAME
		0x06, // ascii string
		'P', 'N', 'G', '2', 'G', 'D', 'S', '.', 'D', 'B'
	};
	const uint8_t gds_units[] = {
		0x00, 0x14, // 14 bytes long
		0x03, // UNITS
		0x05, // eight byte real
		0x3E, 0x41, 0x89, 0x37, 0x4B, 0xC6, 0xA7, 0xF0,
		0x39, 0x44, 0xB8, 0x2F, 0xA0, 0x9B, 0x5A, 0x54,
	};
	const uint8_t gds_bgnstr[] = {
		0x00, 0x1C, // 28 bytes long
		0x05, // BGNSTR
		0x02, 0x00, 0x64, 0x00, 0x05, 0x00, 0x02, 0x00, 0x0f,
		0x00, 0x28, 0x00, 0x38, 0x07, 0xD5, 0x00, 0x02, 0x00,
		0x12, 0x00, 0x0f, 0x00, 0x2a, 0x00, 0x09
	};
	const uint8_t gds_strname[] = {
		0x00, 0x0C, // 11 bytes long
		0x06, // STRNAME
		0x06, // ascii string
		'p', 'n', 'g', '2', 'g', 'd', 's', '\0',
	};

	fwrite(gds_header, sizeof(gds_header), 1, optr);
	fwrite(gds_bgnlib, sizeof(gds_bgnlib), 1, optr);
	fwrite(gds_libname, sizeof(gds_libname), 1, optr);
	fwrite(gds_units, sizeof(gds_units), 1, optr);
	fwrite(gds_bgnstr, sizeof(gds_bgnstr), 1, optr);
	fwrite(gds_strname, sizeof(gds_strname), 1, optr);
}

void gds_write_pixel(FILE *optr, uint8_t layer, unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2)
{
	const uint8_t gds_boundary[] = {
		0x00, 0x04, // 4 bytes long
		0x08, // BOUNDARY
		0x00, // no data
	};
	const uint8_t gds_layer[] = {
		0x00, 0x06, // 6 bytes long
		0x0D, // LAYER
		0x02, // two byte int
		0x00, // first byte only, second is 'layer'
	};
	const uint8_t gds_datatype[] = {
		0x00, 0x06, // 6 bytes long
		0x0E, // DATATYPE
		0x02, // two byte int
		0x00, 0x00 // 0
	};
	const uint8_t gds_xy[] = {
		0x00, 0x2C, // 44 bytes long
		0x10, // XY
		0x03, // four byte int
		// Remaining data written after
	};
	const uint8_t gds_endel[] = {
		0x00, 0x04, // 4 bytes long
		0x11, // ENDEL
		0x00, // no data
	};


	fwrite(gds_boundary, sizeof(gds_boundary), 1, optr);
	fwrite(gds_layer, sizeof(gds_layer), 1, optr);
	fputc(layer, optr); // second byte of layer 2 byte int
	fwrite(gds_datatype, sizeof(gds_datatype), 1, optr);
	fwrite(gds_xy, sizeof(gds_xy), 1, optr);

	gds_write_ulong(optr, x1);
	gds_write_ulong(optr, y1);

	gds_write_ulong(optr, x1);
	gds_write_ulong(optr, y2);

	gds_write_ulong(optr, x2);
	gds_write_ulong(optr, y2);

	gds_write_ulong(optr, x2);
	gds_write_ulong(optr, y1);

	gds_write_ulong(optr, x1);
	gds_write_ulong(optr, y1);

	fwrite(gds_endel, sizeof(gds_endel), 1, optr);
}

void gds_write_end(FILE *optr)
{
	const uint8_t gds_endstr[] = {
		0x00, 0x04, // 4 bytes long
		0x07, // ENDSTR
		0x00, // no data
	};
	const uint8_t gds_endlib[] = {
		0x00, 0x04, // 4 bytes long
		0x04, // ENDLIB
		0x00, // no data
	};

	fwrite(gds_endstr, sizeof(gds_endstr), 1, optr);
	fwrite(gds_endlib, sizeof(gds_endlib), 1, optr);
}
