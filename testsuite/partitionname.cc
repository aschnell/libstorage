
#include <stdlib.h>
#include <iostream>

#include <storage/StorageInterface.h>

#include "common.h"


using namespace std;
using namespace storage;


StorageInterface* s = 0;


int
main()
{ 
    s = createStorageInterface(TestEnvironment());

    cout << s->getPartitionName("/dev/sda", 1) << endl;
    cout << s->getPartitionName("/dev/cciss/c0d1", 2) << endl;
    cout << s->getPartitionName("/dev/mmcblk0", 3) << endl;
    cout << s->getPartitionName("/dev/mapper/isw_cfcjajfdfh_test", 4) << endl;

    delete s;
}
