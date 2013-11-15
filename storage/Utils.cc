/*
 * Copyright (c) 2013 Novell, Inc.
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


#include <string>

#include "storage/Storage.h"
#include "storage/SystemInfo.h"


namespace storage
{
    using namespace std;


    list<string>
    getPresentDisks()
    {
	list<string> ret;

	list<pair<string, Disk::SysfsInfo>> dlist;
	Storage::getDiskList(dlist);

	for (list<pair<string, Disk::SysfsInfo>>::const_iterator it = dlist.begin(); it != dlist.end(); ++it)
	    ret.push_back("/dev/" + Disk::sysfsToDev(it->first));

	y2mil("ret:" << ret);
	return ret;
    }


    map<string, string>
    DmraidToMdadm()
    {
	SystemInfo systeminfo;

	map<string, string> ret;

	const CmdDmraid& cmd_dmraid = systeminfo.getCmdDmraid();
	for (CmdDmraid::const_iterator it1 = cmd_dmraid.begin(); it1 != cmd_dmraid.end(); ++it1)
	{
	    // The name from dmraid is something like "ddf1_foo" or "isw_chadfejhhc_foo".
	    string::size_type pos = it1->first.rfind('_');
	    if (pos == string::npos)
	    {
		y2err("unexpected input");
		continue;
	    }
	    string name = string(it1->first, pos + 1);

	    const MdadmExamine& examine = systeminfo.getMdadmExamine(it1->second.devices);
	    MdadmExamine::const_iterator it2 = examine.find(name);
	    if (it2 == examine.end())
	    {
		y2err("failed to find mdadm");
		continue;
	    }

	    ret[it1->first] = it2->second.uuid;
	}

	y2mil("ret:" << ret);

	return ret;
    }

}
