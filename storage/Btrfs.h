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


#ifndef BTRFS_H
#define BTRFS_H

#include "storage/Volume.h"

namespace storage
{
class BtrfsCo;
class SystemInfo;


class Btrfs : public Volume
    {
    public:

	Btrfs( const BtrfsCo& d, const Volume& v, unsigned long long sz, 
	       const list<string>& devices );
	Btrfs( const BtrfsCo& d, const Volume& v );
	Btrfs( const BtrfsCo& d, const xmlNode* node );
	Btrfs( const BtrfsCo& c, const Btrfs& v);
	virtual ~Btrfs();

	void clearSubvol() { subvol.clear(); }
	void addSubvol( const string& path );
	const list<string>& getDevices() const { return devices; }
	void getDevices( list<string>& devs ) const { devs=devices; }
	void getSubvolumes( list<Subvolume>& sv ) const { sv = subvol; }

	int createSubvolume( const string& name );
	int deleteSubvolume( const string& name );

	void getCommitActions(list<commitAction>& l) const;
	int doDeleteSubvol();
	int doCreateSubvol();
	Text createSubvolText(bool doing) const;
	Text deleteSubvolText(bool doing) const;
	void countSubvolAddDel( unsigned& add, unsigned& rem ) const;
	Text removeText( bool doing ) const;

	void saveData(xmlNode* node) const;
	friend std::ostream& operator<< (std::ostream& s, const Btrfs& l );
	virtual void print( std::ostream& s ) const { s << *this; }
	void getInfo( storage::BtrfsInfo& info ) const;
	bool equalContent( const Btrfs& rhs ) const;
	void logDifference(std::ostream& log, const Btrfs& rhs) const;
	void unuseDev() const;
	int clearSignature();


	static bool notDeleted( const Btrfs& l ) { return( !l.deleted() ); }
	static bool needCreateSubvol( const Btrfs& v );
	static bool needDeleteSubvol( const Btrfs& v );

    protected:
	string subvolNames( bool added ) const; 
	list<string> devices;
	list<string> dev_add;
	list<Subvolume> subvol;

	mutable storage::BtrfsInfo info; // workaround for broken ycp bindings

    private:

	Btrfs& operator=(const Btrfs& v); // disallow
	Btrfs(const Btrfs&);	          // disallow

    };

}

#endif
