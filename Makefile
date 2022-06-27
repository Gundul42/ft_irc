
NAME	=	ft_irc

SRCS	=	IrcServ.cpp ftClient.cpp run.cpp Commands.cpp Message.cpp Channel.cpp Oper.cpp

OBJ		=	${SRCS:.cpp=.o}

CC		=	c++

FLAGS	=	-std=c++98 -g -Wextra -Wall #-Werror  
$(NAME)	:	$(OBJ)	$(SRCS)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)

all	:	$(NAME)

clean	:
	rm -f $(OBJ)

fclean	:	clean	
	rm -f $(NAME)

re	:	fclean all

.PHONY: all clean fclean re
