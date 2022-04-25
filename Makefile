NAME 			= webserv

SRCS_DIR 		= src/
INCLUDE_DIR 	= includes/
OBJS_DIR 		= obj/
LOGS_DIR 		= logs/
WWW				= www/

SRCS			:=	cgi/CGI \
					cgi/CgiParser \
					client/client \
					config/config \
					config/configLocation \
					config/configServer \
					core/main \
					core/message \
					core/webserv \
					descriptors/descriptors \
					formats/files \
					formats/http \
					formats/strbinary \
					formats/strings \
					formats/units \
					request/request \
					request/utils \
					response/response \
					sockets/sockets \
					sockets/socketsListener \
					temporary/temporary \
					temporary/tmpfile


SRCS			:= $(addsuffix .cpp, $(SRCS))
SRCS			:= $(addprefix $(SRCS_DIR), $(SRCS))
OBJS			= $(patsubst $(SRCS_DIR)%.cpp, $(OBJS_DIR)%.o, $(SRCS))

INCLUDES 		= -I$(INCLUDE_DIR)
DEPS 			= $(OBJS:.o=.d)

CC_OVERRIDE 	?= c++
CC				:= $(CC_OVERRIDE)
FLAGS 			= -MMD -g -Wall -Wextra -Werror -std=c++98 -DWWW=\"$(WWW)\" #-pedantic

$(NAME): $(OBJS) | $(OBJS_DIR)
	$(CC) $(FLAGS) $(OBJS) $(INCLUDES) -o $(NAME)

all: $(NAME)

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

$(LOGS_DIR):
	@mkdir -p $(LOGS_DIR)

$(OBJS): $(OBJS_DIR)%.o : $(SRCS_DIR)%.cpp | $(OBJS_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $(INCLUDES) $< -o $@

debug: fclean
debug: FLAGS += -DDEBUG
debug: all

clean:
	rm -rf $(OBJS_DIR)
	rm -rf $(LOGS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all | $(OBJS_DIR)

test: all | $(LOGS_DIR)
	
-include $(DEPS)

.PHONY: all clean fclean re test debug
