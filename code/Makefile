NAME			:=	WebServ
ifdef iwyu
CC				:=	iwyu
else
CC				:=	c++
endif
MAKEFILE		:=	Makefile
CONFIG			:=	config.mk
ifndef NOFLAGS
	FLAGS		:=	-Wall -Werror -Wextra -g -std=c++98
endif

include $(CONFIG)

HEADERS_DIR		:=	includes/

TEMPLATES_DIR	:=	templates/

INCLUDES		:=	-I$(HEADERS_DIR) -I$(TEMPLATES_DIR)

SRCS_DIR		:=	srcs/
SRCS_FILES		:=	$(SOCKET) $(CONFIGURATION) $(PARSING_CONF) $(DATA_FLOW)

SRCS			:=	$(addprefix $(SRCS_DIR), $(SRCS_FILES))
ifndef MAIN
SRCS_FILES		+=	main.cpp
SRCS			+= $(SRCS_DIR)main.cpp
else
SRCS_FILES		+= $(notdir $(MAIN))
SRCS			+= $(MAIN)
endif

# OBJECTS GENERAL
OBJS_DIR		:=	objs/
OBJS_FILES		:=	$(SRCS_FILES:.cpp=.o)
OBJS			:=	$(addprefix $(OBJS_DIR), $(OBJS_FILES))

# DEPENDENCIES
DEPS_DIR		:=	deps/
DEPS_FILES		:=	$(OBJS_FILES:.o=.d)
DEPS			:=	$(addprefix $(DEPS_DIR), $(DEPS_FILES))

# RULES
all: $(NAME)

$(NAME): $(OBJS) $(MAKEFILE) $(MAKEFILE) $(CONFIG)
	$(CC) $(FLAGS) $(INCLUDES) $(OBJS) -o $@

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp $(MAKEFILE) $(CONFIG)
	@mkdir -p $(@D)
	@mkdir -p $(DEPS_DIR)$(dir $*)
	$(CC) $(FLAGS) $(INCLUDES) -MP -MMD -MF $(DEPS_DIR)$*.d -c $< -o $@

ifdef MAIN
MAIN_OBJECT	:=	$(basename $(notdir $(MAIN))).o
$(OBJS_DIR)$(MAIN_OBJECT): $(MAIN) $(MAKEFILE) $(CONFIG)
	@mkdir -p $(@D)
	@mkdir -p $(DEPS_DIR)$(dir $*)
	$(CC) $(FLAGS) $(INCLUDES) -MP -MMD -MF $(DEPS_DIR)$*.d -c $< -o $@
endif

-include $(DEPS)

clean:
	rm -rf $(OBJS_DIR)
	rm -rf $(DEPS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
