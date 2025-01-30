SOCKET_DIR				:=	socketCommunication/
SOCKET_FILES			:=	createServerSockets.cpp	\
							socketCommunication.cpp	\
							FdData.cpp			\
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
							RequestHandler.cpp
DATA_FLOW				:=	$(addprefix $(DATA_FLOW_DIR), $(DATA_FLOW_FILES))

CONFIGURATION_DIR		:=	configuration/
CONFIGURATION_FILES		:=	Host.cpp	\
							Configuration.cpp
CONFIGURATION			:=	$(addprefix $(CONFIGURATION_DIR), $(CONFIGURATION_FILES))