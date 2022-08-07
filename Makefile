
NAME	=	ft_irc

BOT		=	bot

SRCS	=	IrcServ.cpp ftClient.cpp run.cpp Commands.cpp Message.cpp Channel.cpp

BONUS	=	Bot.cpp bonus.cpp

OBJ		=	${SRCS:.cpp=.o}

CC		=	g++

FLAGS	=	-std=c++98 -pedantic-errors -Wextra -Wall -Werror

DFLAGS	=	-ggdb -std=c++98 -pedantic-errors -Wextra -Wall -Werror

$(NAME)	:	$(SRCS) $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) 
	@echo "\033[0;36m*****************"
	@echo "** \033[0;97mtype \033[0;31m$(NAME)\033[0;36m **"
	@echo "*****************\033[0m"

all	:	$(NAME)

debug	:	$(SRCS) fclean
	$(CC) $(DFLAGS) $(SRCS) -o $(NAME) 

bonus	:	$(BONUS)
	$(CC) $(FLAGS) -o $(BOT) $(BONUS)
	@echo "\033[0;36m*****************"
	@echo "** \033[0;97mrun  \033[0;31m./$(BOT)\033[0;36m  **"
	@echo "*****************\033[0m"

clean	:
	rm -f $(OBJ)

fclean	:	clean	
	rm -f $(NAME)
	rm -f $(BOT)

re	:	fclean all

.PHONY: all clean fclean re
