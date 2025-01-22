NAME = webserv

SRC_DIR = src
SRC_SOCK_DIR = src/socket
BIN_DIR = bin

SRC_FILES = main.cpp Config.cpp
SRC_SOCK_FILES = cltSocket.cpp  srvSocket.cpp

SRCS = $(addprefix $(SRC_DIR)/,$(SRC_FILES)) \
		$(addprefix $(SRC_SOCK_DIR)/,$(SRC_SOCK_FILES))

OBJS = $(SRCS:%.cpp=$(BIN_DIR)/%.o)

INCLUDES = -I includes

CFLAGS = -Wall -Wextra -Werror -std=c++98
CC = c++ $(CFLAGS)

BOLD = \033[1m
GRAY = \033[30m
RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
MAGENTA = \033[35m
CYAN = \033[36m
RESET = \033[0m

all: $(NAME)

$(BIN_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "$(CYAN)Compiling $<...$(RESET)"
	@$(CC) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	@echo "\n$(YELLOW)Linking objects...$(RESET)"
	@$(CC) -o $(NAME) $^
	@echo "$(BLUE)Progress: 100%$(RESET)"
	@echo "$(GREEN)Compilation complete!$(RESET)"

clean:
	@echo "$(BLUE)Cleaning objects...$(RESET)"
	@rm -rf $(BIN_DIR)
	@echo "$(GREEN)Cleaning complete!$(RESET)"

fclean: clean
	@echo "$(BLUE)Complete cleanup..."
	@rm -f $(NAME)
	@echo "$(GREEN)Cleaning complete!$(RESET)"

re: fclean all

.PHONY: all clean fclean re
