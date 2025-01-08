SOCKET_DIR				:=	socketCommunication/
SOCKET_FILES			:=	emptyFile.cpp
SOCKET					:=	$(addprefix $(SOCKET_DIR), $(SOCKET_FILES))

CONFIGURATION_DIR		:=	configuration/
CONFIGURATION_FILES		:=	Configuration.cpp	\
							ServerConfiguration.cpp 
CONFIGURATION			:=	$(addprefix $(CONFIGURATION_DIR), $(CONFIGURATION_FILES))
