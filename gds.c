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

