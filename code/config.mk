SOCKET_DIR				:=	socketCommunication/
SOCKET_FILES			:=	createServerSockets.cpp	\
							socketCommunication.cpp	\
							AFdData.cpp				\
							ASocketData.cpp			\
							FileFd.cpp				\
							signals.cpp				\
							EPollHandler.cpp		\
							setFdOption.cpp	\
							unixSocket.cpp			\
							ConnectedSocketData.cpp	\
							ServerSocketData.cpp
SOCKET					:=	$(addprefix $(SOCKET_DIR), $(SOCKET_FILES))

REQUEST_HANDLING_DIR	:=	dataFlow/requestHandling/
REQUEST_HANDLING_FILES	:=	RequestHandler.cpp		\
							RequestHandlerRedirection.cpp	\
							RequestHandlerParsing.cpp		\
							RequestHandlerExecution.cpp
REQUEST_HANDLING		:=	$(addprefix $(REQUEST_HANDLING_DIR), $(REQUEST_HANDLING_FILES))

DATA_FLOW_DIR			:=	dataFlow/
DATA_FLOW_FILES			:=	RawResponse.cpp			\
							FlowBuffer.cpp			\
							ResponsesHandler.cpp	\
							ABody.cpp				\
							SizedBody.cpp			\
							Request.cpp				\
							ChunkedBody.cpp			\
							RequestParsing.cpp		\
							Response.cpp			\
							Headers.cpp				\
							headersInsertType.cpp
DATA_FLOW				:=	$(addprefix $(DATA_FLOW_DIR), $(DATA_FLOW_FILES))

CONFIGURATION_DIR		:=	configuration/
CONFIGURATION_FILES		:=	Configuration.cpp		\
							ServerConfiguration.cpp	\
							Route.cpp				\
							Host.cpp
CONFIGURATION			:=	$(addprefix $(CONFIGURATION_DIR), $(CONFIGURATION_FILES))

PARSING_CONF_DIR		:=	configuration/parsing/
PARSING_CONF_FILES		:=	parsing.cpp				\
							utils.cpp				\
							parseRoute.cpp			\
							parseHost.cpp
PARSING_CONF			:=	$(addprefix $(PARSING_CONF_DIR), $(PARSING_CONF_FILES))

REQUEST_DIR				:=	requestManagement/
REQUEST_FILES			:=	GetRequestUtils.cpp		\
							GetRequest.cpp			\
							DeleteRequest.cpp		\
							DeleteRequestUtils.cpp	\
							PutRequest.cpp			\
							PostRequest.cpp			\
							ARequestType.cpp		\
							ARequestUtils.cpp		\
							statuses.cpp			\
							Status.cpp
REQUEST					:=	$(addprefix $(REQUEST_DIR), $(REQUEST_FILES))

CGI_DIR					:=	cgi/
CGI_FILES				:=	CgiIn.cpp				\
							CgiOut.cpp				\
							CgiOutRedirection.cpp	\
							CgiOutParsing.cpp		\
							RequestContext.cpp		\
							execCGI.cpp
CGI						:=	$(addprefix $(CGI_DIR), $(CGI_FILES))

UTILS_DIR				:=	utils/
UTILS_FILES				:=	conversion.cpp			\
							EMethods.cpp			\
							closeAndPrintError.cpp	\
							deleteArray.cpp
UTILS					:=	$(addprefix $(UTILS_DIR), $(UTILS_FILES))


CGI_ENV_DIR			:=	cgiEnv/
CGI_ENV_FILES		:=	setEnv.cpp	\
						setArgv.cpp
CGI_ENV			:=	$(addprefix $(CGI_ENV_DIR), $(CGI_ENV_FILES))
