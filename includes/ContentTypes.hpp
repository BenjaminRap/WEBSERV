#include <map>
#include <string>

class	ContentTypes
{
	private:
		static const std::map<std::string, std::string>	_extensionContentTypeMap;
	public:
		static const std::string&	getFileContentType(const std::string& fileName);
};
