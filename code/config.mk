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

DATA_FLOW_DIR			:=	dataFlow/
DATA_FLOW_FILES			:=	RawResponse.cpp			\
							FlowBuffer.cpp			\
							ResponsesHandler.cpp	\
							RequestHandler.cpp		\
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
REQUEST_FILES			:=	GetRequestUtils.cpp	\
							GetRequest.cpp	\
							DeleteRequest.cpp	\
							DeleteRequestUtils.cpp	\
							PutRequest.cpp	\
							ARequest.cpp	\
							ARequestUtils.cpp
REQUEST					:=	$(addprefix $(REQUEST_DIR), $(REQUEST_FILES))
