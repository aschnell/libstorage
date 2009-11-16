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


#ifndef MD_H
#define MD_H

#include "storage/StorageInterface.h"
#include "storage/Volume.h"

namespace storage
{

class MdCo;

class Md : public Volume
    {
    public:
	Md( const MdCo& d, unsigned Pnr, storage::MdType Type, 
	    const std::list<string>& devs );
	Md( const MdCo& d, const string& line, const string& line2 );
	Md(const MdCo& c, const Md& v);
	virtual ~Md();

	storage::MdType personality() const { return md_type; }
	void setPersonality( storage::MdType val ); 
	storage::MdParity parity() const { return md_parity; }
	void setParity( storage::MdParity val ) { md_parity=val; }
	unsigned long chunkSize() const { return chunk; }
	void setChunkSize( unsigned long val ) { chunk=val; }
	void setMdUuid( const string&val ) { md_uuid=val; }
	bool destroySb() const { return( destrSb ); }
	void setDestroySb( bool val=true ) { destrSb=val; }
	const string& getMdUuid() const { return(md_uuid); }
	const string& pName() const { return md_names[md_type]; }
	const string& ptName() const { return par_names[md_parity]; }
	void getDevs( std::list<string>& devices, bool all=true, bool spare=false ) const; 
	void addSpareDevice( const string& dev );
	int checkDevices();
	int addDevice( const string& dev, bool spare=false );
	int removeDevice( const string& dev );
	string mdadmLine() const; 
	string createCmd() const;
	static bool matchRegex( const string& dev );
	static unsigned mdMajor();

	static const string& pName( storage::MdType t ) { return md_names[t]; }
	static bool mdStringNum( const string& name, unsigned& num ); 
	friend std::ostream& operator<< (std::ostream& s, const Md& m );
	virtual void print( std::ostream& s ) const { s << *this; }
	string removeText( bool doing ) const;
	string createText( bool doing ) const;
	string formatText( bool doing ) const;
	static bool notDeleted( const Md& l ) { return( !l.deleted() ); }
	void changeDeviceName( const string& old, const string& nw );

	void getInfo( storage::MdInfo& info ) const;
	bool equalContent( const Md& rhs ) const;
	void logDifference( const Md& d ) const;
	void getState(MdStateInfo& info) const;

    protected:
	void init();
	void computeSize();

	static void getMdMajor();
	static storage::MdType toMdType( const string& val );
	static storage::MdParity toMdParity( const string& val );

	storage::MdType md_type;
	storage::MdParity md_parity;
	unsigned long chunk;
	string md_uuid;
	string sb_ver;
	bool destrSb;
	std::list<string> devs;
	std::list<string> spare;

	static const string md_names[storage::MULTIPATH + 1];
	static const string par_names[storage::RIGHT_SYMMETRIC + 1];

	static unsigned md_major;

	mutable storage::MdInfo info; // workaround for broken ycp bindings

    private:

	Md(const Md&);		      // disallow
	Md& operator=(const Md&);     // disallow

    };

}

#endif
