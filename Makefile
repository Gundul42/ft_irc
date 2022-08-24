# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: graja <graja@student.42wolfsburg.de>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/08/20 13:02:33 by graja             #+#    #+#              #
#    Updated: 2022/08/24 12:14:19 by graja            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	ircserv

BOT		=	bot

SRCS	=	IrcServ.cpp ftClient.cpp run.cpp Commands.cpp Message.cpp Channel.cpp

BONUS	=	Bot.cpp bonus.cpp

OBJ		=	${SRCS:.cpp=.o}

CC		=	g++

FLAGS	=	-std=c++98 -pedantic-errors -Wextra -Wall -Werror

DFLAGS	=	-ggdb

$(NAME)	:	$(SRCS) $(OBJ)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ) 
	@echo "\033[0;36m*******************"
	@echo "** \033[0;97mrun \033[0;31m./$(NAME)\033[0;36m **"
	@echo "*******************\033[0m"

all	:	$(NAME)

debug	:	$(BONUS) $(SRCS) fclean
	@echo
	@echo "Server:"
	$(CC) $(DFLAGS) $(FLAGS) $(SRCS) -o $(NAME)
	@echo
	@echo "Bot:"
	$(CC) $(DFLAGS) $(FLAGS) $(BONUS) -o $(BOT) 

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

.PHONY: all clean bonus debug fclean re
