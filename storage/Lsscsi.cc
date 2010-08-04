/*
 * Copyright (c) 2010 Novell, Inc.
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


#include "storage/AppUtil.h"
#include "storage/SystemCmd.h"
#include "storage/StorageTmpl.h"
#include "storage/StorageDefines.h"
#include "storage/Lsscsi.h"
#include "storage/Volume.h"


namespace storage
{
    using namespace std;


    Lsscsi::Lsscsi()
    {
	SystemCmd cmd(LSSCSIBIN " --transport");
	if (cmd.retcode() == 0)
	    parse(cmd.stdout());
    }


    void
    Lsscsi::parse(const vector<string>& lines)
    {
	data.clear();

	for (vector<string>::const_iterator it = lines.begin(); it != lines.end(); ++it)
	{
	    string transport = extractNthWord(2, *it);
	    string device = extractNthWord(3, *it);

	    if (device.empty() || device == "-")
		continue;

	    Entry entry;

	    if (boost::starts_with(transport, "sbp:"))
		entry.transport = SBP;
	    else if (boost::starts_with(transport, "ata:"))
		entry.transport = ATA;
	    else if (boost::starts_with(transport, "fc:"))
		entry.transport = FC;
	    else if (boost::starts_with(transport, "iscsi:"))
		entry.transport = ISCSI;
	    else if (boost::starts_with(transport, "sas:"))
		entry.transport = SAS;
	    else if (boost::starts_with(transport, "sata:"))
		entry.transport = SATA;
	    else if (boost::starts_with(transport, "spi:"))
		entry.transport = SPI;
	    else if (boost::starts_with(transport, "usb:"))
		entry.transport = USB;

	    data[device] = entry;
	}

	for (const_iterator it = data.begin(); it != data.end(); ++it)
	    y2mil("data[" << it->first << "] -> " << it->second);
    }


    bool
    Lsscsi::getEntry(const string& device, Entry& entry) const
    {
	const_iterator i = data.find(device);
	if (i == data.end())
	    return false;

	entry = i->second;
	return true;
    }


    std::ostream& operator<<(std::ostream& s, const Lsscsi::Entry& entry)
    {
	s << "transport:" << Lsscsi::transport_names[entry.transport];

	return s;
    }


    const string Lsscsi::transport_names[] = { "UNKNOWN", "SBP", "ATA", "FC", "iSCSI", "SAS",
					       "SATA", "SPI", "USB" };

}
