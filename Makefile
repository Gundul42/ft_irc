
NAME	=	ft_irc

SRCS	=	IrcServ.cpp ftClient.cpp run.cpp Commands.cpp Message.cpp Channel.cpp Oper.cpp

OBJ		=	${SRCS:.cpp=.o}

CC		=	c++

FLAGS	=	-std=c++98 -g -Wextra -Wall #-Werror  
$(NAME)	:	$(OBJ)	$(SRCS)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)
	@echo "\033[0;36m*****************"
	@echo "** \033[0;97mtype \033[0;31m$(NAME)\033[0;36m **"
	@echo "*****************\033[0m"

all	:	$(NAME)

clean	:
	rm -f $(OBJ)

fclean	:	clean	
	rm -f $(NAME)

re	:	fclean all

.PHONY: all clean fclean re
