
#include <iostream>

#include <storage/StorageInterface.h>
#include <storage/Utils/StorageTmpl.h>


namespace storage
{

    template <typename Type> void
    check_failure(const char* str, Type actual, Type expected, const char* file,
		  int line, const char* func)
    {
	std::cerr << file << ":" << line << ": \"" << func << "\"" << std::endl;
	std::cerr << "check \"" << str << "\"" << " failed. expected " << expected
		  << ", actual " << actual << "." << std::endl;

	exit(EXIT_FAILURE);
    }


#define check_true(expr)						\
    do {								\
	bool actual = (expr);						\
	if (!actual)							\
	    check_failure(#expr, actual, true, __FILE__, __LINE__,	\
			  __PRETTY_FUNCTION__);				\
    } while (0)


#define check_zero(expr)						\
    do {								\
	int actual = (expr);						\
	if (actual != 0)						\
	    check_failure(#expr, actual, 0, __FILE__, __LINE__,		\
			  __PRETTY_FUNCTION__);				\
    } while (0)


    void print_fstab();
    void print_crypttab();

    void print_commitinfos(const StorageInterface* s);

}
