#ifndef DMPART_H
#define DMPART_H

#include "y2storage/Dm.h"
#include "y2storage/Partition.h"

namespace storage
{

class DmPartCo;
class ProcPart;

class DmPart : public Dm
    {
    public:
	DmPart( const DmPartCo& d, unsigned nr, Partition* p=NULL );
	DmPart( const DmPartCo& d, const DmPart& rd );
	DmPart& operator=( const DmPart& );

	virtual ~DmPart();
	friend std::ostream& operator<< (std::ostream& s, const DmPart &p );
	virtual void print( std::ostream& s ) const { s << *this; }
	void getInfo( storage::DmPartInfo& info ) const;
	bool equalContent( const DmPart& rhs ) const;
	void logDifference( const DmPart& d ) const;
	void setPtr( Partition* pa ) { p=pa; };
	Partition* getPtr() const { return p; };
	unsigned id() const { return p?p->id():0; }
	void updateName();
	void updateMinor();
	void updateSize( ProcPart& pp );
	void updateSize();
	void getCommitActions( std::list<storage::commitAction*>& l ) const;
	void addUdevData();
	virtual string setTypeText( bool doing=true ) const;
	static bool notDeleted( const DmPart& l ) { return( !l.deleted() ); }

    protected:
	void init( const string& name );
	void dataFromPart( const Partition* p );
	virtual const string shortPrintedName() const { return( "DmPart" ); }
	const DmPartCo* co() const; 
	void addAltUdevId( unsigned num );
	Partition* p;

	mutable storage::DmPartInfo info;
    };

}

#endif
