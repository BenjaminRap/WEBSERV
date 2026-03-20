#include <map>          // for map
#include <string>       // for basic_string, operator<, string

#include "ContentTypes.hpp"

const std::map<std::string, std::string>	getContentTypesmap(void)
{
	std::map<std::string, std::string>	contentTypes;

	contentTypes["html"] = "text/html";
	contentTypes["htm"]  = "text/html";
	contentTypes["css"]  = "text/css";
	contentTypes["js"]   = "application/javascript";
	contentTypes["json"] = "application/json";
	contentTypes["xml"]  = "application/xml";
	contentTypes["txt"]  = "text/plain";
	contentTypes["csv"]  = "text/csv";

	contentTypes["jpg"]  = "image/jpeg";
	contentTypes["jpeg"] = "image/jpeg";
	contentTypes["png"]  = "image/png";
	contentTypes["gif"]  = "image/gif";
	contentTypes["bmp"]  = "image/bmp";
	contentTypes["webp"] = "image/webp";
	contentTypes["svg"]  = "image/svg+xml";

	contentTypes["ico"]  = "image/x-icon";
	contentTypes["tif"]  = "image/tiff";
	contentTypes["tiff"] = "image/tiff";

	contentTypes["mp3"]  = "audio/mpeg";
	contentTypes["wav"]  = "audio/wav";
	contentTypes["ogg"]  = "audio/ogg";
	contentTypes["m4a"]  = "audio/mp4";

	contentTypes["mp4"]  = "video/mp4";
	contentTypes["webm"] = "video/webm";
	contentTypes["ogv"]  = "video/ogg";
	contentTypes["avi"]  = "video/x-msvideo";

	contentTypes["woff"] = "font/woff";
	contentTypes["woff2"]= "font/woff2";
	contentTypes["ttf"]  = "font/ttf";
	contentTypes["otf"]  = "font/otf";

	contentTypes["pdf"]  = "application/pdf";
	contentTypes["zip"]  = "application/zip";
	contentTypes["tar"]  = "application/x-tar";
	contentTypes["gz"]   = "application/gzip";
	contentTypes["7z"]   = "application/x-7z-compressed";
	return (contentTypes);
}

const std::map<std::string, std::string>	ContentTypes::_extensionContentTypeMap = getContentTypesmap();

const std::string&	ContentTypes::getFileContentType(const std::string& fileName)
{
	const std::string	extension = fileName.substr(fileName.find_last_of('.') + 1);

	try {
		return ContentTypes::_extensionContentTypeMap.at(extension);
	}
	catch (const std::exception&) {
		return _extensionContentTypeMap.at("txt");
	}
}
