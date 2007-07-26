/*
  Textdomain    "storage"
*/

#include <iostream>
#include <sstream>

#include "y2storage/NfsCo.h"
#include "y2storage/Nfs.h"
#include "y2storage/AppUtil.h"
#include "y2storage/ProcMounts.h"
#include "y2storage/Storage.h"
#include "y2storage/EtcFstab.h"

using namespace storage;
using namespace std;

NfsCo::NfsCo( Storage * const s, ProcMounts& mounts ) :
    Container(s,"nfs",staticType())
    {
    y2debug( "constructing NfsCo detect" );
    init();
    getNfsData( mounts );
    }

NfsCo::NfsCo( Storage * const s ) :
    Container(s,"nfs",staticType())
    {
    y2debug( "constructing NfsCo" );
    init();
    }

NfsCo::NfsCo( Storage * const s, const string& file ) :
    Container(s,"nfs",staticType())
    {
    y2debug( "constructing NfsCo file:%s", file.c_str() );
    init();
    }

NfsCo::~NfsCo()
    {
    y2debug( "destructed NfsCo" );
    }

void
NfsCo::init()
    {
    }

bool NfsCo::isNfsDev( const string& dev )
    {
    bool ret = dev.find( ":/" )<dev.find( '/' );
    y2mil( "dev:" << dev << " ret:" << ret );
    return( ret );
    }

int 
NfsCo::removeVolume( Volume* v )
    {
    y2mil( "v:" << *v );
    int ret = 0;
    NfsIter nfs;
    if( !findNfs( v->device(), nfs ))
	{
	ret = NFS_VOLUME_NOT_FOUND;
	}
    if( readonly() )
	{
	ret = NFS_CHANGE_READONLY;
	}
    if( ret==0 )
	{
	if( nfs->created() )
	    {
	    if( !removeFromList( &(*nfs) ))
		ret = NFS_REMOVE_VOLUME_CREATE_NOT_FOUND;
	    }
	else
	    nfs->setDeleted();
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int
NfsCo::doRemove( Volume* v )
    {
    Nfs * p = dynamic_cast<Nfs *>(v);
    int ret = 0;
    if( p != NULL )
	{
	if( !silent )
	    {
	    getStorage()->showInfoCb( p->removeText(true) );
	    }
	y2milestone( "doRemove container %s name %s", name().c_str(),
		     p->name().c_str() );
	ret = v->prepareRemove();
	if( ret==0 )
	    {
	    if( !removeFromList( p ) )
		ret = NFS_REMOVE_VOLUME_LIST_ERASE;
	    }
	}
    else
	{
	ret = NFS_REMOVE_INVALID_VOLUME;
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int 
NfsCo::addNfs( const string& nfsDev, unsigned long long sizeK,
               const string& mp )
    {
    y2mil( "nfsDev:" << nfsDev << " sizeK:" << sizeK << " mp:" << mp );
    Nfs *n = new Nfs( *this, nfsDev );
    n->changeMount( mp );
    n->setSize( sizeK );
    addToList( n );
    return( 0 );
    }

void
NfsCo::getNfsData( ProcMounts& mounts )
    {
    y2mil( "begin fstab:" << getStorage()->getFstab() );
    list<FstabEntry> l;
    getStorage()->getFstab()->getEntries(l);
    for( list<FstabEntry>::const_iterator i=l.begin(); i!=l.end(); ++i )
	{
	if( i->fs == "nfs" )
	    {
	    Nfs *n = new Nfs( *this, i->device );
	    n->setMount( i->mount );
	    string op = mergeString(i->opts, "," );
	    if( op != "defaults" )
		n->setFstabOption( op );
	    addToList( n );
	    }
	}
    l.clear();
    mounts.getEntries(l);
    const char * ign_opt[] = { "hard", "rw", "v3", "v2", "lock" };
    const char * ign_beg[] = { "proto=", "addr=", "vers=" };
    for( list<FstabEntry>::iterator i=l.begin(); i!=l.end(); ++i )
	{
	if( i->fs == "nfs" )
	    {
	    Nfs *n = NULL;
	    NfsIter nfs;
	    if( findNfs( i->device, nfs ))
		{
		n = &(*nfs);
		}
	    else
		{
		n = new Nfs( *this, i->device );
		list<string>::iterator si = i->opts.begin();
		unsigned pos = 0;
		while( si!=i->opts.end() )
		    {
		    pos=0; 
		    while( pos<lengthof(ign_opt) && *si!=ign_opt[pos] )
			++pos;
		    if( pos<lengthof(ign_opt) )
			si = i->opts.erase(si);
		    else
			{
			pos=0;
			while( pos<lengthof(ign_beg) && si->find(ign_beg[pos])!=0 )
			    ++pos;
			if( pos<lengthof(ign_beg) )
			    si = i->opts.erase(si);
			else
			    ++si;
			}
		    }
		n->setIgnoreFstab();
		n->setFstabOption( mergeString(i->opts, "," ) );
		addToList( n );
		}
	    unsigned long long sz = getStorage()->getDfSize( i->mount );
	    n->setSize( sz );
	    }
	}
    l.clear();
    }

bool
NfsCo::findNfs( const string& dev, NfsIter& i )
    {
    NfsPair p=nfsPair();
    i=p.begin();
    while( i!=p.end() && i->device()!=dev )
	++i;
    return( i!=p.end() );
    }

bool
NfsCo::findNfs( const string& dev )
    {
    NfsIter i;
    return( findNfs( dev, i ));
    }


void NfsCo::logData( const string& Dir ) {;}

namespace storage
{

inline std::ostream& operator<< (std::ostream& s, const NfsCo& d )
    {
    s << *((Container*)&d);
    return( s );
    }

}

void NfsCo::logDifference( const Container& d ) const
    {
    y2mil( "" << Container::getDiffString( d ));
    const NfsCo* p = dynamic_cast<const NfsCo*>(&d);
    if( p != NULL )
	{
	ConstNfsPair pp=nfsPair();
	ConstNfsIter i=pp.begin();
	while( i!=pp.end() )
	    {
	    ConstNfsPair pc=p->nfsPair();
	    ConstNfsIter j = pc.begin();
	    while( j!=pc.end() &&
		   (i->device()!=j->device() || i->created()!=j->created()) )
		++j;
	    if( j!=pc.end() )
		{
		if( !i->equalContent( *j ) )
		    i->logDifference( *j );
		}
	    else
		y2mil( "  -->" << *i );
	    ++i;
	    }
	pp=p->nfsPair();
	i=pp.begin();
	while( i!=pp.end() )
	    {
	    ConstNfsPair pc=nfsPair();
	    ConstNfsIter j = pc.begin();
	    while( j!=pc.end() &&
		   (i->device()!=j->device() || i->created()!=j->created()) )
		++j;
	    if( j==pc.end() )
		y2mil( "  <--" << *i );
	    ++i;
	    }
	}
    }

bool NfsCo::equalContent( const Container& rhs ) const
    {
    const NfsCo* p = NULL;
    bool ret = Container::equalContent(rhs);
    if( ret )
	p = dynamic_cast<const NfsCo*>(&rhs);
    if( ret && p )
	{
	ConstNfsPair pp = nfsPair();
	ConstNfsPair pc = p->nfsPair();
	ConstNfsIter i = pp.begin();
	ConstNfsIter j = pc.begin();
	while( ret && i!=pp.end() && j!=pc.end() )
	    {
	    ret = ret && i->equalContent( *j );
	    ++i;
	    ++j;
	    }
	ret = ret && i==pp.end() && j==pc.end();
	}
    return( ret );
    }

NfsCo::NfsCo( const NfsCo& rhs ) : Container(rhs)
    {
    y2debug( "constructed NfsCo by copy constructor from %s", rhs.nm.c_str() );
    *this = rhs;
    ConstNfsPair p = rhs.nfsPair();
    for( ConstNfsIter i=p.begin(); i!=p.end(); ++i )
         {
         Nfs * p = new Nfs( *this, *i );
         vols.push_back( p );
         }
    }


