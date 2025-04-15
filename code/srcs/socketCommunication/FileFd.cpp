#include "FileFd.hpp"

FileFd::FileFd(const std::string& path, int flags, mode_t mode, EPollHandler& ePollHandler) :
	AFdData(open(path.c_str(), flags, mode), false, ePollHandler)
{
}

FileFd::~FileFd()
{
}

void	FileFd::callback(uint32_t events)
{
	(void)events;
}
