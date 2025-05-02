#include "CgiOut.hpp"
#include "FlowBuffer.hpp"
#include "requestStatusCode.hpp"

void	CgiOut::readFromCgi()
{
	if (_error || _state == FILE_TO_BUFFER || !_isActive || _state == DONE)
		return ;
	const FlowState flowState = _flowBuf.srcToBuff(getFd());

	if (flowState == FLOW_BUFFER_FULL || flowState == FLOW_MORE)
		return ;
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
		_state = FILE_TO_BUFFER;
	else if (_error)
		setFinished();
	else
		_state = CGI_TO_BUFFER;
}

void	CgiOut::writeToBuff(void)
{
	const int		fd = _srcFile ? _srcFile->getFd() : getFd();
	const FlowState	flowState = _flowBuf.srcToBuff(fd);

	if (flowState == FLOW_BUFFER_FULL || flowState == FLOW_MORE)
		return ;
	setFinished();
}
