#include <iostream>

#include <blocxx/AppenderLogger.hpp>
#include <blocxx/FileAppender.hpp>

#include <y2storage/StorageInterface.h>

using namespace storage;
using namespace std;
using namespace blocxx;

void scrollbarCb( const string& id, unsigned cur, unsigned max )
    {
    cout << "SCROLLBAR id:" << id << " cur:" << cur << " max:" << max << endl;
    }

void installInfoCb( const string& info )
    {
    cout << "INFO " << info << endl;
    }

void
printCommitActions( StorageInterface* s )
    {
    deque<string> l = s->getCommitActions( false );
    for( deque<string>::iterator i=l.begin(); i!=l.end(); ++i )
	cout << *i << endl;
    }

int doCommit( StorageInterface* s )
    {
    static int cnt = 1;

    //printCommitActions( s );
    int ret = s->commit();
    if( ret ) cerr << "retcode:" << ret << endl;
    if( ret==0 )
	{
	printCommitActions( s );
	}
    cnt++;
    return( ret );
    }

int
main( int argc_iv, char** argv_ppcv )
    {
    int ret = 0;
    initDefaultLogger();
    //StorageInterface* s = createStorageInterface(false,true);
    StorageInterface* s = createDefaultStorageInterface();
    s->setCallbackProgressBar( scrollbarCb );
    s->setCallbackShowInstallInfo( installInfoCb );
    string disk = "/dev/hdb";
    string device;
    deque<string> devs;
    deque<PartitionInfo> infos;
    s->getPartitionsOfDisk( disk, infos );
    for( deque<PartitionInfo>::iterator i=infos.begin(); i!=infos.end(); ++i )
	if( i->partitionType!=EXTENDED )
	    devs.push_back( i->v.name );
    if( ret==0 )
	{
	ret = s->createLvmVg( "testvg", 8*1024, false, devs );
	if( ret ) cerr << "retcode:" << ret << endl;
	}
    if( ret==0 )
	{
	ret = s->createLvmLv( "testvg", "test1", 1*1024, 2, device );
	if( ret ) cerr << "retcode:" << ret << endl;
	}
    if( ret==0 && devs.size()>0 )
	{
	deque<string> ds;
	ds.push_back( devs.back() );
	ret = s->shrinkLvmVg( "testvg", ds );
	if( ret ) cerr << "retcode:" << ret << endl;
	}
    if( ret==0 )
	{
	ret = doCommit( s );
	}
    if( ret==0 )
	{
	ret = s->resizeVolume( device, 2900*1024 );
	if( ret ) cerr << "retcode:" << ret << endl;
	}
    if( ret==0 )
	{
	ret = doCommit( s );
	}
    if( ret==0 )
	{
	ret = s->createPartitionAny( disk, 1*1024*1024, device );
	if( ret ) cerr << "retcode:" << ret << endl;
	}
    string ext_device;
    if( ret==0 )
	{
	deque<string> ds;
	ds.push_back( device );
	ext_device = device;
	ret = s->extendLvmVg( "testvg", ds );
	if( ret ) cerr << "retcode:" << ret << endl;
	}
    if( ret==0 )
	{
	ret = s->createLvmLv( "testvg", "test2", 3*1024, 1, device );
	if( ret ) cerr << "retcode:" << ret << endl;
	}
    if( ret==0 )
	{
	ret = doCommit( s );
	}
    if( ret==0 )
	{
	ret = s->removeLvmLvByDevice( device );
	if( ret ) cerr << "retcode:" << ret << endl;
	}
    if( ret==0 )
	{
	ret = doCommit( s );
	}
    if( ret==0 )
	{
	deque<string> ds;
	ds.push_back( ext_device );
	ret = s->shrinkLvmVg( "testvg", ds );
	if( ret ) cerr << "retcode:" << ret << endl;
	}
    if( ret==0 )
	{
	ret = doCommit( s );
	}
    if( ret==0 )
	{
	ret = s->removeLvmVg( "testvg" );
	if( ret ) cerr << "retcode:" << ret << endl;
	}
    if( ret==0 )
	{
	ret = doCommit( s );
	}
    delete(s);
    }
