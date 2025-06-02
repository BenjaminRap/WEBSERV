#include <stdint.h>     // for uint16_t
#include <list>         // for list, _List_const_iterator, operator!=

#include "Headers.hpp"  // for Headers
#include "Status.hpp"   // for StatusType, Status
#include "parsing.hpp"  // for ConfigHeaders

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
