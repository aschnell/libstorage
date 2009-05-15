
#include <stdlib.h>

#include <y2storage/StorageInterface.h>
#include <y2storage/Graph.h>

using namespace storage;

int
main (int argc, char** argv)
{
    initDefaultLogger();

    StorageInterface* s = createStorageInterface(Environment(true));

    saveGraph(s, "storage.gv");

    destroyStorageInterface(s);

    exit(EXIT_SUCCESS);
}
