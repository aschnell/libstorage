/*
 * Copyright (c) [2015] SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/CmdLsattr.h"
#include "storage/Disk.h"
#include "storage/Utils/Enum.h"
#include "storage/Exception.h"


namespace storage
{
    using namespace std;


    CmdLsattr::CmdLsattr(const string& mount_point, const string& path, bool do_probe)
	: path(path)
    {
	if (do_probe)
	    probe(mount_point);
    }


    void
    CmdLsattr::probe(const string& mount_point)
    {
	SystemCmd cmd(LSATTRBIN " -d " + quote(mount_point + "/" + path));
	if (cmd.retcode() != 0)
	    ST_THROW(SystemCmdException(&cmd, "lsattr failed, retcode:" + to_string(cmd.retcode())));

	parse(cmd.stdout());
    }


    void
    CmdLsattr::parse(const vector<string>& lines)
    {
	if (lines.size() != 1)
	    ST_THROW(ParseException("wrong number of lines", to_string(lines.size()), to_string(1)));

	const string& line = lines.front();

	string::size_type pos = line.find(' ');
	if (pos == string::npos)
	    ST_THROW(ParseException("no space found", line, "---------------C /var/lib/mariadb"));

	const string attrs = line.substr(0, pos);

	nocow = attrs.find('C') != string::npos;

	y2mil(*this);
    }


    std::ostream&
    operator<<(std::ostream& s, const CmdLsattr& lsattr)
    {
	s << "path:" << lsattr.path;

	if (lsattr.nocow)
	    s << " nocow:" << lsattr.nocow;

	return s;
    }

}
