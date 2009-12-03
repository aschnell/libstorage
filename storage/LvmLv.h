/*
 * Copyright (c) [2004-2009] Novell, Inc.
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


#ifndef LVM_LV_H
#define LVM_LV_H

#include <fstream>

#include "storage/Dm.h"


namespace storage
{

class LvmVg;

class LvmLv : public Dm
    {
    public:

	LvmLv(const LvmVg& d, const string& name, const string& origin, unsigned long le, 
	      const string& uuid, const string& status, const string& alloc);
	LvmLv(const LvmVg& d, const string& name, const string& origin, unsigned long le, 
	      unsigned stripe);
	LvmLv(const LvmVg& c, const string& data);
	LvmLv(const LvmVg& c, const LvmLv& v);
	virtual ~LvmLv();

	const LvmVg* vg() const;

	void calcSize();

	void getState(LvmLvSnapshotStateInfo& info);

	void setOrigin( const string& o ) { origin=o; }
	string getOrigin() const { return origin; }

	bool isSnapshot() const { return !origin.empty(); }
	bool hasSnapshots() const;	

	void setUuid( const string& uuid ) { vol_uuid=uuid; }
	void setStatus( const string& s ) { status=s; }
	void setAlloc( const string& a ) { allocation=a; }
	friend std::ostream& operator<< (std::ostream& s, const LvmLv &p );
	virtual void print( std::ostream& s ) const { s << *this; }
	Text removeText( bool doing ) const;
	Text createText( bool doing ) const;
	Text formatText( bool doing ) const;
	Text resizeText( bool doing ) const;
	void getInfo( storage::LvmLvInfo& info ) const;
	bool equalContent( const LvmLv& rhs ) const;
	void logDifference( const LvmLv& d ) const;
	std::ostream& logData(std::ostream& file) const;

    protected:
	static string makeDmTableName(const string& vg_name, const string& lv_name);
	void init( const string& name );
	virtual const string shortPrintedName() const { return "Lv"; }

	string origin;		// only for snapshots, empty otherwise

	string vol_uuid;
	string status;
	string allocation;

	mutable storage::LvmLvInfo info; // workaround for broken ycp bindings

    private:

	LvmLv(const LvmLv&);		// disallow
	LvmLv& operator=(const LvmLv&); // disallow

    };

}

#endif
