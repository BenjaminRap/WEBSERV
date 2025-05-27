#include "Headers.hpp"

const std::map<std::string, InsertType>	getHeadersInsertType(void);

const std::map<std::string, InsertType>	Headers::_headersInsertType = getHeadersInsertType();

const std::map<std::string, InsertType>	getHeadersInsertType(void)
{
	std::map<std::string, InsertType>	headersInsertType;

	headersInsertType["accept"] = CONCAT_COMMA;
	headersInsertType["accept-charset"] = CONCAT_COMMA;
	headersInsertType["accept-encoding"] = CONCAT_COMMA;
	headersInsertType["accept-language"] = CONCAT_COMMA;
	headersInsertType["accept-ranges"] = CONCAT_COMMA;
	headersInsertType["access-control-allow-origin"] = UNIQUE;
	headersInsertType["age"] = UNIQUE;
	headersInsertType["allow"] = CONCAT_COMMA;
	headersInsertType["authorization"] = UNIQUE;
	headersInsertType["cache-control"] = CONCAT_COMMA;
	headersInsertType["connection"] = CONCAT_COMMA;
	headersInsertType["content-encoding"] = CONCAT_COMMA;
	headersInsertType["content-language"] = CONCAT_COMMA;
	headersInsertType["content-length"] = UNIQUE;
	headersInsertType["content-location"] = REPLACE;
	headersInsertType["content-type"] = UNIQUE;
	headersInsertType["content-disposition"] = REPLACE;
	headersInsertType["cookie"] = CONCAT_SEMICOLON;
	headersInsertType["date"] = UNIQUE;
	headersInsertType["etag"] = UNIQUE;
	headersInsertType["expect"] = CONCAT_COMMA;
	headersInsertType["expires"] = REPLACE;
	headersInsertType["from"] = UNIQUE;
	headersInsertType["host"] = UNIQUE;
	headersInsertType["if-match"] = CONCAT_COMMA;
	headersInsertType["if-modified-since"] = REPLACE;
	headersInsertType["if-none-match"] = CONCAT_COMMA;
	headersInsertType["if-range"] = REPLACE;
	headersInsertType["keep-alive"] = REPLACE;
	headersInsertType["server"] = UNIQUE;
	headersInsertType["last-modified"] = REPLACE;
	headersInsertType["location"] = UNIQUE;
	headersInsertType["pragma"] = CONCAT_COMMA;
	headersInsertType["proxy-authenticate"] = MULTIPLE;
	headersInsertType["proxy-authorization"] = UNIQUE;
	headersInsertType["range"] = CONCAT_COMMA;
	headersInsertType["referer"] = REPLACE;
	headersInsertType["retry-after"] = UNIQUE;
	headersInsertType["set-cookie"] = MULTIPLE;
	headersInsertType["server"] = UNIQUE;
	headersInsertType["te"] = CONCAT_COMMA;
	headersInsertType["trailer"] = CONCAT_COMMA;
	headersInsertType["transfer-encoding"] = CONCAT_COMMA;
	headersInsertType["upgrade"] = CONCAT_COMMA;
	headersInsertType["user-agent"] = UNIQUE;
	headersInsertType["vary"] = CONCAT_COMMA;
	headersInsertType["via"] = CONCAT_COMMA;
	headersInsertType["warning"] = MULTIPLE;
	headersInsertType["www-authenticate"] = MULTIPLE;
	return (headersInsertType);
}
