SOCKET_DIR				:=	socketCommunication/
SOCKET_FILES			:=	createServerSockets.cpp	\
							socketCommunication.cpp	\
							SocketData.cpp			\
							signals.cpp				\
							SocketsHandler.cpp		\
							setSocketsOption.cpp	\
							socketDataCallbacks.cpp
SOCKET					:=	$(addprefix $(SOCKET_DIR), $(SOCKET_FILES))

DATA_FLOW_DIR			:=	dataFlow/
DATA_FLOW_FILES			:=	RawResponse.cpp			\
							FlowBuffer.cpp			\
							ResponsesHandler.cpp
DATA_FLOW				:=	$(addprefix $(DATA_FLOW_DIR), $(DATA_FLOW_FILES))

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
