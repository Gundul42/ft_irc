
NAME	=	ft_irc

SRCS	=	IrcServ.cpp ftClient.cpp run.cpp Commands.cpp Message.cpp Channel.cpp

OBJ		=	${SRCS:.cpp=.o}

CC		=	c++

FLAGS	=	#-Wextra -Werror -Wall -std=c++98 -g

$(NAME)	:	$(OBJ)	$(SRCS)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)

all	:	$(NAME)

clean	:
	rm -f $(OBJ)

fclean	:	clean	
	rm -f $(NAME)

re	:	fclean all

.PHONY: all clean fclean re
