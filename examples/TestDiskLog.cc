#include <getopt.h>

#include <ycp/y2log.h>

#include <y2storage/StorageInterface.h>

using namespace storage;
using namespace std;

int
main( int argc_iv, char** argv_ppcv )
    {
    int ret;
    StorageInterface* s = createStorageInterface();
    string dev;
    ret = s->destroyPartitionTable( "/dev/hdb", s->defaultDiskLabel() );
    if( ret ) cerr << "retcode:" << ret << endl;
    ret = s->createPartitionKb( "/dev/hdb", EXTENDED, 5000*1024, 10000*1024, dev );
    cout << dev << endl;
    if( ret ) cerr << "retcode:" << ret << endl;
    int i=0;
    while( i<40 && ret==0 )
	{
	ret = s->createPartitionKb( "/dev/hdb", LOGICAL, (5000+i*300)*1024, 300*1024, 
	                            dev );
	if(ret==0) cout << dev << endl;
	i++;
	}
    ret = s->commit();
    if( ret ) cerr << "retcode:" << ret << endl;
    delete(s);
    }
