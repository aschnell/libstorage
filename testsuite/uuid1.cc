
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>

#include "common.h"


using namespace std;
using namespace storage;


void
print_fstab ()
{
    ifstream fstab("tmp/etc/fstab");
    string line;

    cout << "begin of fstab" << endl;

    while (getline (fstab, line))
	if (boost::contains(line, "/tmp/mnt"))
	    cout << line << endl;

    cout << "end of fstab" << endl;
}


void
run1 ()
{
    cout << "run1" << endl;

    StorageInterface* s = createStorageInterface(TestEnvironment());

    string disk = "/dev/sdb";

    s->destroyPartitionTable (disk, "msdos");

    long int S = 4 * 1000000;

    string name;
    cout << s->createPartitionKb(disk, PRIMARY, 0, S, name) << endl;

    cout << name << endl;

    cout << s->changeFormatVolume(name, true, REISERFS) << endl;
    cout << s->changeMountPoint(name, "/tmp/mnt") << endl;
    cout << s->changeMountBy(name, MOUNTBY_UUID) << endl;

    cout << s->commit() << endl;

    delete s;
}


void
run2 ()
{
    cout << "run2" << endl;

    StorageInterface* s = createStorageInterface(TestEnvironment());

    string name = "/dev/sdb1";

    cout << name << endl;

    cout << s->changeMountBy(name, MOUNTBY_DEVICE) << endl;

    cout << s->commit() << endl;

    delete s;
}


int
main()
{
    cout.setf(std::ios::boolalpha);

    setup_logger();

    setup_system("thalassa");

    run1 ();
    print_fstab ();

    run2 ();
    print_fstab ();
}
