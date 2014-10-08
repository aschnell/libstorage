/*
 * Copyright (c) [2004-2014] Novell, Inc.
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


#ifndef CMD_BLKID_H
#define CMD_BLKID_H

#include <string>
#include <map>
#include <vector>

#include "storage/StorageInterface.h"


namespace storage
{
    using std::map;
    using std::vector;


    class Blkid
    {
    public:

	Blkid(bool do_probe = true);
	Blkid(const string& device);

	void probe();

	struct Entry
	{
	    Entry() : is_fs(false), fs_type(FSUNKNOWN), fs_uuid(), fs_label(),
		      is_lvm(false), is_luks(false), luks_uuid() {}

	    bool is_fs;
	    FsType fs_type;
	    string fs_uuid;
	    string fs_label;

	    bool is_lvm;

	    bool is_luks;
	    string luks_uuid;
	};

	friend std::ostream& operator<<(std::ostream& s, const Blkid& blkid);
	friend std::ostream& operator<<(std::ostream& s, const Entry& entry);

	bool getEntry(const string& device, Entry& entry) const;

	void parse(const vector<string>& lines);

    private:

	typedef map<string, Entry>::const_iterator const_iterator;

	map<string, Entry> data;

    };

}


#endif
