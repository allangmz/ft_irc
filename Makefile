SRCS	:=	main.cpp \
			Commande.cpp \
			Server.cpp \
			Client.cpp \
			Channel.cpp \


NAME	:=	ircserv

GCC		:=	c++

RM		:=	rm -f

FLAGS	:=	-Wall -Wextra -Werror -std=c++98 -g3

OBJS	:=	${SRCS:.cpp=.o}

.cpp.o:
	${GCC} ${FLAGS} -I./include -c $< -o ${<:.cpp=.o}

RESET		:= \033[0m
RED			:= \033[1;31m
GREEN		:= \033[1;32m
YELLOW		:= \033[1;33m
BLUE		:= \033[1;34m
CYAN		:= \033[1;36m
RM_LINE		:= \033[2K\r


${NAME}:	${OBJS}
			@echo "$(GREEN)Compilation ${RESET}of ${YELLOW}$(NAME) ${RESET}..."
			${GCC} ${FLAGS} -I./include -o ${NAME} ${OBJS}
			@echo "$(GREEN)$(NAME) created ✔️${RESET}"

all:		${NAME}

clean:
			@ ${RM} *.o */*.o */*/*.o
			@ echo "$(RED)Deleting $(CYAN)$(NAME) $(RESET)objs ✔️"

fclean:		clean
			@ ${RM} ${NAME}
			@ echo "$(RED)Deleting $(CYAN)$(NAME) $(RESET)binary ✔️"

re:			fclean all

.PHONY:		all clean fclean re