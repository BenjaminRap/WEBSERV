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
CONFIGURATION_FILES		:=	Host.cpp	\
							Configuration.cpp
CONFIGURATION			:=	$(addprefix $(CONFIGURATION_DIR), $(CONFIGURATION_FILES))