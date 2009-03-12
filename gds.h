/*
 * File: gds.h
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

#ifndef GDS_H
#define GDS_H

#include <stdio.h>

void write_gds_all_header(FILE *optr);
void write_gds_bgnlib(FILE *optr);
void write_gds_bgnstr(FILE *optr);
void write_gds_pixels(FILE *optr, unsigned char layer, unsigned long x1, unsigned long y1, unsigned long x2, unsigned long y2);
void write_gds_endlib(FILE *optr);
void write_gds_endstr(FILE *optr);
void write_gds_header(FILE *optr);
void write_gds_libname(FILE *optr);
void write_gds_strname(FILE *optr);
void write_gds_ulong(FILE *optr, unsigned long num);

#endif
