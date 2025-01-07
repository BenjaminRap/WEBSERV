SOCKET_DIR				:=	socketCommunication/
SOCKET_FILES			:=	createServerSockets.cpp	\
							socketCommunication.cpp	\
							SocketData.cpp			\
							serverInterruption.cpp
SOCKET					:=	$(addprefix $(SOCKET_DIR), $(SOCKET_FILES))

CONFIGURATION_DIR		:=	configuration/
CONFIGURATION_FILES		:=	Configuration.cpp		\
							ServerConfiguration.cpp
CONFIGURATION			:=	$(addprefix $(CONFIGURATION_DIR), $(CONFIGURATION_FILES))

UTILS_DIR				:=	utils/
UTILS_FILES				:=	bzero.cpp
UTILS					:=	$(addprefix $(UTILS_DIR), $(UTILS_FILES))