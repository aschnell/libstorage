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


#ifndef ARCH_INFO_H
#define ARCH_INFO_H


#include "storage/Utils/XmlFile.h"


namespace storage
{

    class ArchInfo
    {
    public:

	ArchInfo();

	void readData(const xmlNode* node);
	void saveData(xmlNode* node) const;

	void probe();

	bool is_ia64() const;
	bool is_ppc() const;
	bool is_ppc64le() const;
	bool is_s390() const;
	bool is_sparc() const;
	bool is_x86() const;

	bool is_ppc_mac() const { return ppc_mac; }
	bool is_ppc_pegasos() const { return ppc_pegasos; }
	bool is_ppc_p8() const { return ppc_p8; }
	bool is_ppc_powernv() const { return ppc_powernv; }

	bool is_efiboot() const { return efiboot; }

	friend std::ostream& operator<<(std::ostream& s, const ArchInfo& archinfo);

    private:

	string arch;
	bool ppc_mac;
	bool ppc_pegasos;
	bool efiboot;
	bool ppc_p8;
	bool ppc_powernv;

    };


    std::ostream& operator<<(std::ostream& s, const ArchInfo& archinfo);

}


#endif
