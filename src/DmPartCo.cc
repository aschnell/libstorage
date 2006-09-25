/*
  Textdomain    "storage"
*/

#include <iostream> 
#include <sstream> 

#include "y2storage/DmPartCo.h"
#include "y2storage/DmPart.h"
#include "y2storage/ProcPart.h"
#include "y2storage/Partition.h"
#include "y2storage/SystemCmd.h"
#include "y2storage/AppUtil.h"
#include "y2storage/Storage.h"

using namespace std;
using namespace storage;

DmPartCo::DmPartCo( Storage * const s, const string& name, storage::CType t,
                    ProcPart& ppart ) :
    PeContainer(s,t)
    {
    y2debug( "constructing DmPart co %s", name.c_str() );
    dev = name;
    nm = undevName(name);
    num_part = num_pe = free_pe = 0;
    active = valid = del_ptable = false;
    disk = NULL;
    init( ppart );
    }

DmPartCo::~DmPartCo()
    {
    if( disk )
	{
	delete disk;
	disk = NULL;
	}
    y2debug( "destructed DmPart co %s", dev.c_str() );
    }

int DmPartCo::addNewDev( string& device )
    {
    int ret = 0;
    std::pair<string,long> dp = Disk::getDiskPartition( device );
    y2mil( "dp:" << dp );
    device = "/dev/mapper/" + numToName(dp.second);
    Partition *p = getPartition( dp.second, false );
    if( p==NULL )
	ret = DMPART_PARTITION_NOT_FOUND;
    else
	{
	y2mil( "*p:" << *p );
	DmPart * dm = NULL;
	newP( dm, p->nr(), p );
	dm->getFsInfo( p );
	dm->setCreated();
	addToList( dm );
	}
    handleWholeDevice();
    y2mil( "device:" << device << " ret:" << ret );
    return( ret );
    }

int 
DmPartCo::createPartition( storage::PartitionType type, long unsigned start,
			   long unsigned len, string& device, bool checkRelaxed )
    {
    y2milestone( "begin type %d at %ld len %ld relaxed:%d", type, start, len,
		 checkRelaxed );
    int ret = disk ? 0 : DMPART_INTERNAL_ERR;
    if( ret==0 && readonly() )
	ret = DMPART_CHANGE_READONLY;
    if( ret==0 )
	ret = disk->createPartition( type, start, len, device, checkRelaxed );
    if( ret==0 )
	ret = addNewDev( device );
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int 
DmPartCo::createPartition( long unsigned len, string& device, bool checkRelaxed )
    {
    y2milestone( "len %ld relaxed:%d", len, checkRelaxed );
    int ret = disk ? 0 : DMPART_INTERNAL_ERR;
    if( ret==0 && readonly() )
	ret = DMPART_CHANGE_READONLY;
    if( ret==0 )
	ret = disk->createPartition( len, device, checkRelaxed );
    if( ret==0 )
	ret = addNewDev( device );
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int 
DmPartCo::createPartition( storage::PartitionType type, string& device )
    {
    y2milestone( "type %u", type );
    int ret = disk ? 0 : DMPART_INTERNAL_ERR;
    if( ret==0 && readonly() )
	ret = DMPART_CHANGE_READONLY;
    if( ret==0 )
	ret = disk->createPartition( type, device );
    if( ret==0 )
	ret = addNewDev( device );
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int DmPartCo::updateDelDev()
    {
    int ret = 0;
    list<Volume*> l;
    DmPartPair p=dmpartPair();
    DmPartIter i=p.begin();
    while( i!=p.end() )
	{
	Partition *p = i->getPtr();
	if( p )
	if( validPartition( p ) )
	    {
	    if( i->nr()!=p->nr() )
		{
		i->updateName();
		y2mil( "updated name dm:" << *i );
		}
	    if( i->deleted() != p->deleted() )
		{
		i->setDeleted( p->deleted() );
		y2mil( "updated del dm:" << *i );
		}
	    }
	else
	    l.push_back( &(*i) );
	++i;
	}
    list<Volume*>::iterator vi = l.begin();
    while( ret==0 && vi!=l.end() )
	{
	if( !removeFromList( *vi ))
	    ret = DMPART_PARTITION_NOT_FOUND;
	++vi;
	}
    handleWholeDevice();
    return( ret );
    }

int 
DmPartCo::removePartition( unsigned nr )
    {
    int ret = disk ? 0 : DMPART_INTERNAL_ERR;
    if( ret==0 && readonly() )
	ret = DMPART_CHANGE_READONLY;
    if( ret==0 )
	{
	if( nr>0 )
	    ret = disk->removePartition( nr );
	else
	    ret = DMPART_PARTITION_NOT_FOUND;
	}
    if( ret==0 )
	ret = updateDelDev();
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int
DmPartCo::removeVolume( Volume* v )
    {
    int ret = disk ? 0 : DMPART_INTERNAL_ERR;
    if( ret==0 && readonly() )
	ret = DMPART_CHANGE_READONLY;
    if( ret==0 )
	{
	unsigned num = v->nr();
	if( num>0 )
	    ret = disk->removePartition( v->nr() );
	}
    if( ret==0 )
	ret = updateDelDev();
    getStorage()->logCo( this );
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int DmPartCo::resizePartition( DmPart* dm, unsigned long newCyl )
    {
    Partition * p = dm->getPtr();
    int ret = p?0:DMPART_PARTITION_NOT_FOUND;
    if( ret==0 )
	{
	p->getFsInfo( dm );
	ret = disk->resizePartition( p, newCyl );
	dm->updateSize();
	}
    y2mil( "dm:" << *dm );
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int
DmPartCo::resizeVolume( Volume* v, unsigned long long newSize )
    {
    int ret = disk ? 0 : DMPART_INTERNAL_ERR;
    if( ret==0 && readonly() )
	ret = DMPART_CHANGE_READONLY;
    DmPart * l = dynamic_cast<DmPart *>(v);
    if( ret==0 && l==NULL )
	ret = DMPART_INVALID_VOLUME;
    if( ret==0 )
	{
	Partition *p = l->getPtr();
	unsigned num = v->nr();
	if( num>0 && p!=NULL )
	    {
	    p->getFsInfo( v );
	    ret = disk->resizeVolume( p, newSize );
	    }
	else
	    ret = DMPART_PARTITION_NOT_FOUND;
	}
    if( ret==0 )
	{
	l->updateSize();
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

void 
DmPartCo::init( ProcPart& ppart )
    {
    SystemCmd c( "dmsetup table " + nm );
    if( c.retcode()==0 && c.numLines()>=1 && isdigit( c.stdout()[0] ))
	{
	mnr = Dm::dmNumber( nm );
	ppart.getSize( "dm-"+decString(mnr), size_k );
	y2mil( "mnr:" << mnr << " nm:" << nm );
	y2mil( "pe_size:" << pe_size << " size_k:" << size_k );
	if( size_k>0 )
	    pe_size = size_k;
	else
	    y2war( "size_k zero for dm minor " << mnr );
	num_pe = 1;
	createDisk( ppart );
	if( disk->numPartitions()>0 )
	    {
	    string pat = numToName(1);
	    pat.erase( pat.length()-1, 1 );
	    c.execute( "dmsetup ls | grep -w ^" + pat + "[0-9]\\\\+" );
	    if( c.numLines()==0 )
		activate_part(true);
	    }
	getVolumes( ppart );
	active = valid = true;
	}
    }

void
DmPartCo::createDisk( ProcPart& ppart )
    {
    if( disk )
	delete disk;
    disk = new Disk( getStorage(), dev, size_k );
    disk->setNumMinor( 64 );
    disk->setSilent();
    disk->setSlave();
    disk->detect(ppart);
    }

void 
DmPartCo::newP( DmPart*& dm, unsigned num, Partition* p )
    {
    y2mil( "num:" << num );
    dm = new DmPart( *this, num, p );
    }

void
DmPartCo::getVolumes( ProcPart& ppart )
    {
    vols.clear();
    num_part = 0;
    Disk::PartPair pp = disk->partPair();
    Disk::PartIter i = pp.begin();
    DmPart * p = NULL;
    while( i!=pp.end() )
	{
	newP( p, i->nr(), &(*i) );
	p->updateSize( ppart );
	num_part++;
	addToList( p );
	++i;
	}
    handleWholeDevice();
    }

void DmPartCo::handleWholeDevice()
    {
    Disk::PartPair pp = disk->partPair( Partition::notDeleted );
    y2milestone( "empty:%d", pp.empty() );
    if( pp.empty() )
	{
	DmPart * p = NULL;
	newP( p, 0, NULL );
	p->setSize( size_k );
	addToList( p );
	}
    else
	{
	DmPartIter i;
	if( findDm( 0, i ))
	    {
	    DmPart* dm = &(*i);
	    if( !removeFromList( dm ))
		y2err( "not found:" << *i );
	    }
	}
    }
    
Partition* 
DmPartCo::getPartition( unsigned nr, bool del )
    {
    Partition* ret = NULL;
    Disk::PartPair pp = disk->partPair();
    Disk::PartIter i = pp.begin();
    while( i!=pp.end() && (nr!=i->nr() || del!=i->deleted()) )
	++i;
    if( i!=pp.end() )
	ret = &(*i);
    if( ret )
	y2mil( "nr:" << nr << " del:" << del << " *p:" << *ret )
    else
	y2mil( "nr:" << nr << " del:" << del << " p:NULL" )
    return( ret );
    }

bool 
DmPartCo::validPartition( const Partition* p )
    {
    bool ret = false;
    Disk::PartPair pp = disk->partPair();
    Disk::PartIter i = pp.begin();
    while( i!=pp.end() && p != &(*i) )
	++i;
    ret = i!=pp.end();
    y2mil( "nr:" << p->nr() << " ret:" << ret )
    return( ret );
    }

void DmPartCo::updatePointers( bool invalid )
    {
    DmPartPair p=dmpartPair();
    DmPartIter i=p.begin();
    while( i!=p.end() )
	{
	if( invalid )
	    i->setPtr( getPartition( i->nr(), i->deleted() ));
	i->updateName();
	++i;
	}
    }

void DmPartCo::updateMinor()
    {
    DmPartPair p=dmpartPair();
    DmPartIter i=p.begin();
    while( i!=p.end() )
	{
	i->updateMinor();
	++i;
	}
    }

string DmPartCo::numToName( unsigned num ) const
    {
    string ret = nm;
    if( num>0 )
	{
	ret += "_part";
	ret += decString(num);
	}
    y2mil( "num:" << num << " ret:" << ret );
    return( ret );
    }

string DmPartCo::undevName( const string& name )
    {
    string ret = name;
    if( ret.find( "/dev/" ) == 0 )
	ret.erase( 0, 5 );
    if( ret.find( "mapper/" ) == 0 )
	ret.erase( 0, 7 );
    return( ret );
    }

int DmPartCo::destroyPartitionTable( const string& new_label )
    {
    y2milestone( "begin" );
    int ret = disk->destroyPartitionTable( new_label );
    if( ret==0 )
	{
	VIter j = vols.begin();
	while( j!=vols.end() )
	    {
	    if( (*j)->created() )
		{
		delete( *j );
		j = vols.erase( j );
		}
	    else
		++j;
	    }
	bool save = getStorage()->getRecursiveRemoval();
	getStorage()->setRecursiveRemoval(true);
	if( getUsedByType() != UB_NONE )
	    {
	    getStorage()->removeUsing( device(), getUsedBy() );
	    }
	ronly = false;
	RVIter i = vols.rbegin();
	while( i!=vols.rend() )
	    {
	    if( !(*i)->deleted() )
		getStorage()->removeVolume( (*i)->device() );
	    ++i;
	    }
	getStorage()->setRecursiveRemoval(save);
	del_ptable = true;
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int DmPartCo::changePartitionId( unsigned nr, unsigned id )
    {
    int ret = nr>0?0:DMPART_PARTITION_NOT_FOUND;
    if( ret==0 )
	{
	ret = disk->changePartitionId( nr, id );
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int DmPartCo::forgetChangePartitionId( unsigned nr )
    {
    int ret = nr>0?0:DMPART_PARTITION_NOT_FOUND;
    if( ret==0 )
	{
	ret = disk->forgetChangePartitionId( nr );
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int DmPartCo::nextFreePartition( PartitionType type, unsigned& nr, string& device )
    {
    int ret = disk->nextFreePartition( type, nr, device );
    if( ret==0 )
	{
	device = "/dev/mapper/" + numToName(nr);
	}
    y2milestone( "ret:%d nr:%d device:%s", ret, nr, device.c_str() );
    return( ret );
    }

int DmPartCo::changePartitionArea( unsigned nr, unsigned long start,
				   unsigned long len, bool checkRelaxed )
    {
    int ret = nr>0?0:DMPART_PARTITION_NOT_FOUND;
    if( ret==0 )
	{
	ret = disk->changePartitionArea( nr, start, len, checkRelaxed );
	DmPartIter i;
	if( findDm( nr, i ))
	    i->updateSize();
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

bool DmPartCo::findDm( unsigned nr, DmPartIter& i )
    {
    DmPartPair p = dmpartPair( DmPart::notDeleted );
    i=p.begin();
    while( i!=p.end() && i->nr()!=nr )
	++i;
    return( i!=p.end() );
    }

void DmPartCo::activate_part( bool val )
    {
    y2milestone( "old active:%d val:%d", active, val );
    if( active != val )
	{
	SystemCmd c;
	if( val )
	    {
	    Dm::activate(true);
	    c.execute( "kpartx -a -p _part " + dev );
	    }
	else
	    {
	    c.execute( "kpartx -d -p _part " + dev );
	    }
	active = val;
	}
    }

int DmPartCo::doSetType( DmPart* dm )
    {
    y2milestone( "doSetType container %s name %s", name().c_str(),
		 dm->name().c_str() );
    Partition * p = dm->getPtr();
    int ret = p?0:DMPART_PARTITION_NOT_FOUND;
    if( ret==0 )
	{
	if( !silent )
	    {
	    getStorage()->showInfoCb( dm->setTypeText(true) );
	    }
	ret = disk->doSetType( p );
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int DmPartCo::doCreateLabel()
    {
    y2milestone( "label:%s", labelName().c_str() );
    int ret = 0;
    if( !silent )
	{
	getStorage()->showInfoCb( setDiskLabelText(true) );
	}
    getStorage()->removeDmMapsTo( device() );
    removePresentPartitions();
    ret = disk->doCreateLabel();
    if( ret==0 )
	{
	del_ptable = false;
	removeFromMemory();
	handleWholeDevice();
	getStorage()->waitForDevice();
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int
DmPartCo::removeDmPart()
    {
    int ret = 0;
    y2milestone( "begin" );
    if( readonly() )
	{
	ret = DMPART_CHANGE_READONLY;
	}
    if( ret==0 && !created() )
	{
	DmPartPair p=dmpartPair(DmPart::notDeleted);
	for( DmPartIter i=p.begin(); i!=p.end(); ++i )
	    {
	    if( i->nr()>0 )
		ret = removePartition( i->nr() );
	    }
	p=dmpartPair(DmPart::notDeleted);
	if( p.begin()!=p.end() && p.begin()->nr()==0 )
	    {
	    if( !removeFromList( &(*p.begin()) ))
		y2err( "not found:" << *p.begin() );
	    }
	setDeleted( true );
	}
    if( ret==0 )
	{
	unuseDev();
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

void DmPartCo::removePresentPartitions()
    {
    VolPair p = volPair();
    if( !p.empty() )
	{
	bool save=silent;
	setSilent( true );
	list<VolIterator> l;
	for( VolIterator i=p.begin(); i!=p.end(); ++i )
	    {
	    y2mil( "rem:" << *i );
	    if( !i->created() )
		l.push_front( i );
	    }
	for( list<VolIterator>::const_iterator i=l.begin(); i!=l.end(); ++i )
	    {
	    doRemove( &(**i) );
	    }
	setSilent( save );
	}
    }

void DmPartCo::removeFromMemory()
    {
    VIter i = vols.begin();
    while( i!=vols.end() )
	{
	y2mil( "rem:" << *i );
	if( !(*i)->created() )
	    {
	    i = vols.erase( i );
	    }
	else
	    ++i;
	}
    }

static bool toChangeId( const DmPart&d ) 
    { 
    Partition* p = d.getPtr();
    return( p!=NULL && !d.deleted() && Partition::toChangeId(*p) );
    }

int DmPartCo::getToCommit( CommitStage stage, list<Container*>& col,
			   list<Volume*>& vol )
    {
    int ret = 0;
    y2milestone( "ret:%d col:%zd vol:%zd", ret, col.size(), vol.size());
    getStorage()->logCo( this );
    unsigned long oco = col.size();
    unsigned long ovo = vol.size();
    Container::getToCommit( stage, col, vol );
    if( stage==INCREASE )
	{
	DmPartPair p = dmpartPair( toChangeId );
	for( DmPartIter i=p.begin(); i!=p.end(); ++i )
	    if( find( vol.begin(), vol.end(), &(*i) )==vol.end() )
		vol.push_back( &(*i) );
	}
    if( del_ptable && find( col.begin(), col.end(), this )==col.end() )
	col.push_back( this );
    if( col.size()!=oco || vol.size()!=ovo )
	y2milestone( "ret:%d col:%zd vol:%zd", ret, col.size(), vol.size());
    return( ret );
    }


int DmPartCo::commitChanges( CommitStage stage, Volume* vol )
    {
    y2milestone( "name %s stage %d", name().c_str(), stage );
    int ret = Container::commitChanges( stage, vol );
    if( ret==0 && stage==INCREASE )
        {
        DmPart * dm = dynamic_cast<DmPart *>(vol);
        if( dm!=NULL )
            {
	    Partition* p = dm->getPtr();
            if( p && disk && Partition::toChangeId( *p ) )
                ret = doSetType( dm );
            }
        else
            ret = DMPART_INVALID_VOLUME;
        }
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int DmPartCo::commitChanges( CommitStage stage )
    {
    y2milestone( "name %s stage %d", name().c_str(), stage );
    int ret = 0;
    if( stage==DECREASE && deleted() )
	{
	ret = doRemove();
	}
    else if( stage==DECREASE && del_ptable )
	{
	ret = doCreateLabel();
	}
    else
	ret = DMPART_COMMIT_NOTHING_TODO;
    y2milestone( "ret:%d", ret );
    return( ret );
    }

void DmPartCo::getCommitActions( list<commitAction*>& l ) const
    {
    y2mil( "l:" << l );
    Container::getCommitActions( l );
    y2mil( "l:" << l );
    if( deleted() || del_ptable )
	{
	list<commitAction*>::iterator i = l.begin();
	while( i!=l.end() )
	    {
	    if( (*i)->stage==DECREASE )
		{
		delete( *i );
		i=l.erase( i );
		}
	    else
		++i;
	    }
	string txt = deleted() ? removeText(false) : 
	                         setDiskLabelText(false);
	l.push_front( new commitAction( DECREASE, staticType(),
				        txt, true, true ));
	}
    y2mil( "l:" << l );
    }

int 
DmPartCo::doCreate( Volume* v ) 
    {
    y2milestone( "Raid:%s part:%s", name().c_str(), v->name().c_str() );
    DmPart * l = dynamic_cast<DmPart *>(v);
    int ret = disk ? 0 : DMPART_INTERNAL_ERR;
    if( ret==0 && l == NULL )
	ret = DMPART_INVALID_VOLUME;
    Partition *p = NULL;
    if( ret==0 )
	{
	if( !silent )
	    {
	    getStorage()->showInfoCb( l->createText(true) );
	    }
	p = l->getPtr();
	if( p==NULL )
	    ret = DMPART_PARTITION_NOT_FOUND;
	else
	    ret = disk->doCreate( p );
	if( ret==0 && p->id()!=Partition::ID_LINUX )
	    ret = doSetType( l );
	}
    if( ret==0 )
	{
	l->setCreated(false);
	if( active )
	    {
	    activate_part(false);
	    activate_part(true);
	    ProcPart pp;
	    updateMinor();
	    l->updateSize( pp );
	    }
	if( p->type()!=EXTENDED )
	    getStorage()->waitForDevice( l->device() );
	}
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int DmPartCo::doRemove()
    {
    return( DMPART_NO_REMOVE );
    }

int DmPartCo::doRemove( Volume* v )
    {
    y2milestone( "Raid:%s name:%s", name().c_str(), v->name().c_str() );
    DmPart * l = dynamic_cast<DmPart *>(v);
    bool save_act = false;
    int ret = disk ? 0 : DMPART_INTERNAL_ERR;
    if( ret==0 && l == NULL )
	ret = DMPART_INVALID_VOLUME;
    if( ret==0 )
	{
	if( !silent )
	    {
	    getStorage()->showInfoCb( l->removeText(true) );
	    }
	ret = v->prepareRemove();
	}
    if( ret==0 )
	{
	save_act = active;
	if( active )
	    activate_part(false);
	Partition *p = l->getPtr();
	if( p==NULL )
	    ret = DMPART_PARTITION_NOT_FOUND;
	else
	    ret = disk->doRemove( p );
	}
    if( ret==0 )
	{
	if( !removeFromList( l ) )
	    ret = DMPART_REMOVE_PARTITION_LIST_ERASE;
	}
    if( save_act )
	{
	activate_part(true);
	updateMinor();
	}
    if( ret==0 )
	getStorage()->waitForDevice();
    y2milestone( "ret:%d", ret );
    return( ret );
    }

int DmPartCo::doResize( Volume* v ) 
    {
    y2milestone( "DmPart:%s name:%s", name().c_str(), v->name().c_str() );
    DmPart * l = dynamic_cast<DmPart *>(v);
    int ret = disk ? 0 : DMPART_INTERNAL_ERR;
    if( ret==0 && l == NULL )
	ret = DMPART_INVALID_VOLUME;
    bool remount = false;
    bool needExtend = false;
    if( ret==0 )
	{
	needExtend = !l->needShrink();
	if( !silent )
	    {
	    getStorage()->showInfoCb( l->resizeText(true) );
	    }
	if( l->isMounted() )
	    {
	    ret = l->umount();
	    if( ret==0 )
		remount = true;
	    }
	if( ret==0 && !needExtend && l->getFs()!=VFAT && l->getFs()!=FSNONE )
	    ret = l->resizeFs();
	}
    if( ret==0 )
	{
	Partition *p = l->getPtr();
	if( p==NULL )
	    ret = DMPART_PARTITION_NOT_FOUND;
	else
	    ret = disk->doResize( p );
	}
    if( ret==0 && active )
	{
	activate_part(false);
	activate_part(true);
	}
    if( ret==0 && needExtend && l->getFs()!=VFAT && l->getFs()!=FSNONE )
	ret = l->resizeFs();
    if( ret==0 )
	{
	ProcPart pp;
	updateMinor();
	l->updateSize( pp );
	getStorage()->waitForDevice( l->device() );
	}
    if( ret==0 && remount )
	ret = l->mount();
    y2milestone( "ret:%d", ret );
    return( ret );
    }

string DmPartCo::setDiskLabelText( bool doing ) const
    {
    string txt;
    string d = dev;
    if( doing )
        {
        // displayed text during action, %1$s is replaced by name (e.g. pdc_igeeeadj),
	// %2$s is replaced by label name (e.g. msdos)
        txt = sformat( _("Setting disk label of %1$s to %2$s"),
		       d.c_str(), labelName().c_str() );
        }
    else
        {
        // displayed text before action, %1$s is replaced by name (e.g. pdc_igeeeadj),
	// %2$s is replaced by label name (e.g. msdos)
        txt = sformat( _("Set disk label of %1$s to %2$s"),
		      d.c_str(), labelName().c_str() );
        }
    return( txt );
    }

string DmPartCo::removeText( bool doing ) const
    {
    string txt;
    if( doing )
        {
        // displayed text during action, %1$s is replaced by a name (e.g. pdc_igeeeadj),
        txt = sformat( _("Removing %1$s"), name().c_str() );
        }
    else
        {
        // displayed text before action, %1$s is replaced by a name (e.g. pdc_igeeeadj),
        txt = sformat( _("Remove %1$s"), name().c_str() );
        }
    return( txt );
    }

void DmPartCo::getInfo( DmPartCoInfo& tinfo ) const
    {
    if( disk )
	{
	disk->getInfo( info.d );
	}
    info.devices.clear();
    list<Pv>::const_iterator i=pv.begin();
    while( i!=pv.end() )
	{
	if( !info.devices.empty() )
	    info.devices += ' ';
	info.devices += i->device;
	++i;
	}
    y2mil( "device:" << info.devices );
    tinfo = info;
    }

namespace storage
{

std::ostream& operator<< (std::ostream& s, const DmPartCo& d )
    {
    s << *((PeContainer*)&d);
    s << " DmNr:" << d.mnr
      << " PNum:" << d.num_part;
    if( d.del_ptable )
      s << " delPT";
    if( !d.active )
      s << " inactive";
    if( !d.valid )
      s << " invalid";
    return( s );
    }

}

string DmPartCo::logDiff( const DmPartCo& d ) const
    {
    string log = PeContainer::logDifference( d );
    if( del_ptable!=d.del_ptable )
	{
	if( d.del_ptable )
	    log += " -->delPT";
	else
	    log += " delPT-->";
	}
    if( active!=d.active )
	{
	if( d.active )
	    log += " -->active";
	else
	    log += " active-->";
	}
    if( valid!=d.valid )
	{
	if( d.valid )
	    log += " -->valid";
	else
	    log += " valid-->";
	}
    return( log );
    }

void DmPartCo::logDifference( const DmPartCo& d ) const
    {
    string log = logDiff( d );
    y2milestone( "%s", log.c_str() );
    ConstDmPartPair p=dmpartPair();
    ConstDmPartIter i=p.begin();
    while( i!=p.end() )
	{
	ConstDmPartPair pc=d.dmpartPair();
	ConstDmPartIter j = pc.begin();
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
    p=d.dmpartPair();
    i=p.begin();
    while( i!=p.end() )
	{
	ConstDmPartPair pc=dmpartPair();
	ConstDmPartIter j = pc.begin();
	while( j!=pc.end() && 
	       (i->device()!=j->device() || i->created()!=j->created()) )
	    ++j;
	if( j==pc.end() )
	    y2mil( "  <--" << *i );
	++i;
	}
    }

bool DmPartCo::equalContent( const DmPartCo& rhs ) const
    {
    bool ret = PeContainer::equalContent(rhs,false) &&
	       active==rhs.active && valid==rhs.valid && 
	       del_ptable==rhs.del_ptable;
    if( ret )
	{
	ConstDmPartPair p = dmpartPair();
	ConstDmPartPair pc = rhs.dmpartPair();
	ConstDmPartIter i = p.begin();
	ConstDmPartIter j = pc.begin();
	while( ret && i!=p.end() && j!=pc.end() ) 
	    {
	    ret = ret && i->equalContent( *j );
	    ++i;
	    ++j;
	    }
	ret == ret && i==p.end() && j==pc.end();
	}
    return( ret );
    }

DmPartCo::DmPartCo( const DmPartCo& rhs ) : PeContainer(rhs)
    {
    y2debug( "constructed DmPartCo by copy constructor from %s", rhs.nm.c_str() );
    active = rhs.active;
    valid = rhs.valid;
    del_ptable = rhs.del_ptable;
    disk = NULL;
    if( rhs.disk )
	disk = new Disk( *rhs.disk );
    ConstDmPartPair p = rhs.dmpartPair();
    for( ConstDmPartIter i = p.begin(); i!=p.end(); ++i )
	{
	DmPart * p = new DmPart( *this, *i );
	vols.push_back( p );
	}
    updatePointers(true);
    }

void DmPartCo::logData( const string& Dir ) {;}

