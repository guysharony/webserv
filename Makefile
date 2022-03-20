ROOT						=	$(realpath .)

SOURCE					=	$(ROOT)/src
OBJECT					=	$(ROOT)/obj
INCLUDE					=	$(ROOT)/include
WWW						=	$(ROOT)/www

SRCS						=	$(wildcard $(SOURCE)/*/*.cpp)
OBJS						=	$(patsubst $(SOURCE)/%.cpp, $(OBJECT)/%.o, $(SRCS))

CXX						=	clang++
CXXFLAGS					=	-g -Wall -Wextra -Werror --std=c++98

HIDE						=	@
MAKEDIR					=	mkdir -p
REMOVE					=	rm -rf

BIN						=	webserv


.PHONY:					all
all:
						$(HIDE)$(MAKEDIR) $(WWW)
all:						CXXFLAGS += -DWWW=\"$(WWW)\"
all:						$(BIN)

.PHONY:					debug
debug:					CXXFLAGS += -DDEBUG
debug:					all

.PHONY:					$(BIN)
$(BIN):					$(OBJS)
						$(HIDE)$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJECT)/%.o:				$(SOURCE)/%.cpp
						$(HIDE)$(MAKEDIR) $(dir $@)
						$(HIDE)$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $< -o $@

.PHONY:					clean
clean:
						$(HIDE)$(REMOVE) $(OBJECT)

.PHONY:					fclean
fclean:					clean
						$(HIDE)$(REMOVE) $(BIN)

.PHONY:					re
re:						fclean all