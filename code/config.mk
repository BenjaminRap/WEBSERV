SOCKET_DIR				:=	socketCommunication/
SOCKET_FILES			:=	createServerSockets.cpp	\
							socketCommunication.cpp	\
							SocketData.cpp			\
							signals.cpp				\
							SocketsHandler.cpp		\
							setSocketsOption.cpp	\
							socketDataCallbacks.cpp
SOCKET					:=	$(addprefix $(SOCKET_DIR), $(SOCKET_FILES))

CONFIGURATION_DIR		:=	configuration/
CONFIGURATION_FILES		:=	Configuration.cpp	\
							ServerConfiguration.cpp	\
							Route.cpp	\
							Host.cpp
CONFIGURATION			:=	$(addprefix $(CONFIGURATION_DIR), $(CONFIGURATION_FILES))

PARSING_CONF_DIR		:=	configuration/parsing/
PARSING_CONF_FILES		:=	parsing.cpp	\
							utils.cpp	\
							parse_route.cpp	\
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
