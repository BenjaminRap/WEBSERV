
#include "Headers.hpp"
#include "Status.hpp"
#include "parsing.hpp"

void	processAddHeader(Headers &responseHeaders, const std::list<ConfigHeaders>& addHeader, uint16_t code)
{
	for (std::list<ConfigHeaders>::const_iterator header = addHeader.begin(); header != addHeader.end(); header++)
	{
		if (header->always == true
			|| Status::isCodeOfType(code, (StatusType)(STATUS_SUCESSFULL | STATUS_REDIRECTION)))
		{
			responseHeaders.addHeader(header->key, header->value);
		}
	}
}
