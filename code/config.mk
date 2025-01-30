SOCKET_DIR				:=	socketCommunication/
SOCKET_FILES			:=	emptyFile.cpp
SOCKET					:=	$(addprefix $(SOCKET_DIR), $(SOCKET_FILES))

CONFIGURATION_DIR		:=	configuration/
CONFIGURATION_FILES		:=	Configuration.cpp	\
							ServerConfiguration.cpp \
							readfile.cpp \
							Route.cpp
CONFIGURATION			:=	$(addprefix $(CONFIGURATION_DIR), $(CONFIGURATION_FILES))

REQUEST_DIR				:=	requestManagement/
REQUEST_FILES			:=	GetRequestUtils.cpp	\
							GetRequest.cpp	\
							DeleteRequest.cpp	\
							DeleteRequestUtils.cpp
REQUEST					:=	$(addprefix $(REQUEST_DIR), $(REQUEST_FILES))