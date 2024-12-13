NAME			=	WebServ
CC				=	c++
MAKEFILE		=	Makefile
ifndef NOFLAGS
	FLAGS		=	-Wall -Werror -Wextra -g -std=c++98
endif

HEADERS_DIR		=	includes/

TEMPLATES_DIR	=	templates/

INCLUDES		=	-I$(HEADERS_DIR) -I$(TEMPLATES_DIR)

SRCS_DIR		=	srcs/
SRCS_FILES		=	main.cpp
SRCS			=	$(addprefix $(SRCS_DIR), $(SRCS_FILES))

# OBJECTS GENERAL
OBJS_DIR		=	objs/
OBJS_FILES		=	$(SRCS_FILES:.cpp=.o)
OBJS			=	$(addprefix $(OBJS_DIR), $(OBJS_FILES))

# DEPENDENCIES
DEPS_DIR		=	deps/
DEPS_FILES		=	$(OBJS_FILES:.o=.d)
DEPS			=	$(addprefix $(DEPS_DIR), $(DEPS_FILES))


# RULES
all: $(NAME)

$(NAME): $(OBJS) $(MAKEFILE)
	$(CC) $(FLAGS) $(INCLUDES) $(OBJS) -o $@

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
	@mkdir -p $(@D)
	@mkdir -p $(DEPS_DIR)$(dir $*)
	$(CC) $(FLAGS) $(INCLUDES) -MP -MMD -MF $(DEPS_DIR)$*.d -c $< -o $@ 

-include $(DEPS)

clean:
	rm -rf $(OBJS_DIR)
	rm -rf $(DEPS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
