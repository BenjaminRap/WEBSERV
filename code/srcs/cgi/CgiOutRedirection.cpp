#include <fcntl.h>                // for O_RDONLY
#include <stddef.h>               // for NULL, size_t
#include <string>                 // for basic_string, string

#include "CgiOut.hpp"             // for CgiOut
#include "FileFd.hpp"             // for FileFd
#include "FlowBuffer.hpp"         // for FlowState, FlowBuffer
#include "Headers.hpp"            // for Headers
#include "requestStatusCode.hpp"  // for HTTP_INTERNAL_SERVER_ERROR, HTTP_BA...

std::string	sizeTToString(size_t value);

void	CgiOut::readFromCgi()
{
	if (_error || _state == CgiOut::FILE_TO_BUFFER || _state == CgiOut::DONE)
		return ;
	const FlowState flowState = _flowBuf.srcToBuff(getFd());

	if (flowState == FLOW_BUFFER_FULL || flowState == FLOW_MORE)
		return ;

	delete _srcFile;
	_srcFile = NULL;
	if (_state == CgiOut::CGI_TO_TEMP && flowState == FLOW_DONE)
	{
		try
		{
			_srcFile = new FileFd(_tempName, O_RDONLY);
			_headers.addHeader("content-length", sizeTToString(_srcFile->getSize()));
		}
		catch (FileFd::FileOpeningError& e)
		{
			_code = HTTP_INTERNAL_SERVER_ERROR;
			_error = true;
		}
		generateFirstPart();
	}
	else if (_state == CgiOut::READ_HEADER || _state == CgiOut::CGI_TO_TEMP)
	{
		_code = HTTP_BAD_GATEWAY;
		_error = true;
		generateFirstPart();
	}
	else
		setFinished();
}

void	CgiOut::writeToTemp(void)
{
	if (_flowBuf.buffToDest(_srcFile->getFd()) == FLOW_DONE)
		return ;
	delete _srcFile;
	_srcFile = NULL;
	_code = HTTP_INTERNAL_SERVER_ERROR;
	_error = true;
	generateFirstPart();
}

void	CgiOut::writeFirstPart(void)
{
	char* const	buffer = &_firstPart[_charsWritten];
	size_t		bufferSize = _firstPart.size() - _charsWritten;

	_charsWritten += _flowBuf.addContent(buffer, bufferSize);

	if (_charsWritten != _firstPart.size())
		return ;
	if (_srcFile != NULL)
		_state = CgiOut::FILE_TO_BUFFER;
	else if (_error)
		setFinished();
	else
		_state = CgiOut::CGI_TO_BUFFER;
}

void	CgiOut::writeToBuff(void)
{
	const int		fd = _srcFile ? _srcFile->getFd() : getFd();
	const FlowState	flowState = _flowBuf.srcToBuff(fd);

	if (flowState == FLOW_BUFFER_FULL || flowState == FLOW_MORE)
		return ;
	setFinished();
}
