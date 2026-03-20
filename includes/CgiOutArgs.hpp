#ifndef CGI_OUT_ARGS_HPP
# define CGI_OUT_ARGS_HPP

# include <list>

class	ServerConfiguration;
class	FlowBuffer;
class	ConfigHeaders;

class CgiOutArgs {
private:
	CgiOutArgs();
	CgiOutArgs(const CgiOutArgs& ref);
	CgiOutArgs &operator=(const CgiOutArgs& ref);

	const ServerConfiguration&		_serverConfiguration;
	FlowBuffer& 					_responsesFlowBuffer;
	const std::list<ConfigHeaders>&	_addHeader;

	friend class	CgiOut;

public:
	CgiOutArgs
	(
		const ServerConfiguration& serverConfiguration,
		FlowBuffer& responsesFlowBuffer,
		const std::list<ConfigHeaders>& addHeader
	) :
		_serverConfiguration(serverConfiguration),
		_responsesFlowBuffer(responsesFlowBuffer),
		_addHeader(addHeader)
	{
	}

	~CgiOutArgs()
	{
	}
};

#endif // !CGI_OUT_ARGS_HPP
