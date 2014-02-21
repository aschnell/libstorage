
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "common.h"


using namespace std;
using namespace storage;


int
main()
{
    setup_logger();

    setup_system("empty");

    StorageInterface* s = createStorageInterface(TestEnvironment());

    string device;
    cout << s->createPartition("/dev/sda", EXTENDED, RegionInfo(  0, 1000), device) << endl;
    cout << s->createPartition("/dev/sda", LOGICAL,  RegionInfo(  0,  200), device) << endl;
    cout << s->createPartition("/dev/sda", LOGICAL,  RegionInfo(200,  200), device) << endl;
    cout << s->createPartition("/dev/sda", LOGICAL,  RegionInfo(400,  200), device) << endl;
    cout << s->createPartition("/dev/sda", LOGICAL,  RegionInfo(600,  200), device) << endl;

    list<string> md_devices;
    md_devices.push_back("/dev/sda6");
    md_devices.push_back("/dev/sda7");
    list<string> md_spares;
    md_spares.push_back("/dev/sda8");
    cout << s->createMd("/dev/md0", RAID1, md_devices, md_spares) << endl;

    deque<MdInfo> md_infos;
    cout << s->getMdInfo(md_infos) << endl;
    cout << boost::join(md_infos[0].devices, " ") << endl;
    cout << boost::join(md_infos[0].spares, " ") << endl;

    cout << s->removePartition("/dev/sda5") << endl;

    cout << s->getMdInfo(md_infos) << endl;
    cout << boost::join(md_infos[0].devices, " ") << endl;
    cout << boost::join(md_infos[0].spares, " ") << endl;

    delete s;
}
