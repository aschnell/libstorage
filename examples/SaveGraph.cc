
#include <stdlib.h>

#include <y2storage/StorageInterface.h>
#include <y2storage/Graph.h>

using namespace storage;

int
main (int argc, char** argv)
{
    initDefaultLogger();
    StorageInterface* s = createStorageInterface(true, false, true);

    saveGraph(s, "storage.dot");

    exit(EXIT_SUCCESS);
}
