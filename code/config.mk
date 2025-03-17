SOCKET_DIR				:=	socketCommunication/
SOCKET_FILES			:=	createServerSockets.cpp	\
							socketCommunication.cpp	\
							FdData.cpp				\
							signals.cpp				\
							SocketsHandler.cpp		\
							setSocketsOption.cpp	\
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
							Body.cpp				\
							SizedBody.cpp			\
							Request.cpp				\
							Response.cpp
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
							parse_route.cpp			\
							parse_host.cpp
PARSING_CONF			:=	$(addprefix $(PARSING_CONF_DIR), $(PARSING_CONF_FILES))

REQUEST_DIR				:=	requestManagement/
REQUEST_FILES			:=	GetRequestUtils.cpp		\
							GetRequest.cpp			\
							DeleteRequest.cpp		\
							DeleteRequestUtils.cpp	\
							PutRequest.cpp			\
							ARequestType.cpp		\
							ARequestUtils.cpp		\
							statuses.cpp
REQUEST					:=	$(addprefix $(REQUEST_DIR), $(REQUEST_FILES))

UTILS_DIR				:=	utils/
UTILS_FILES				:=	conversion.cpp
UTILS					:=	$(addprefix $(UTILS_DIR), $(UTILS_FILES))
