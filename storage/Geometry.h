/*
 * Copyright (c) [2004-2010] Novell, Inc.
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "storage/XmlFile.h"


namespace storage
{

    struct Geometry
    {
	Geometry();
	Geometry(unsigned long cylinders, unsigned int heads, unsigned int sectors,
		 unsigned int logical_sector_size);

	unsigned long cylinders;
	unsigned int heads;
	unsigned int sectors;
	unsigned int logical_sector_size;

	unsigned long cylinderSize() const { return heads * sectors * logical_sector_size; }

	friend std::ostream& operator<<(std::ostream& s, const Geometry& geo);

	friend bool getChildValue(const xmlNode* node, const char* name, Geometry& value);
	friend void setChildValue(xmlNode* node, const char* name, const Geometry& value);
    };


    bool detectGeometry(const string& device, Geometry& geometry);

}


#endif
