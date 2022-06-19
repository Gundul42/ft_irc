# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mwen <mwen@student.42wolfsburg.de>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/12/27 14:36:06 by graja             #+#    #+#              #
#    Updated: 2022/06/19 11:07:27 by graja            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	ft_irc	

SRCS	=	IrcServ.cpp Commands.cpp ftClient.cpp  run.cpp

OBJ		=	${SRCS:.cpp=.o}

CC		=	c++

FLAGS	=	#-Wextra -Werror -Wall -std=c++98

$(NAME)	:	$(OBJ)	$(SRCS)
	$(CC) $(FLAGS) -o $(NAME) $(OBJ)

all	:	$(NAME)

clean	:
	rm -f $(OBJ)

fclean	:	clean	
	rm -f $(NAME)

re	:	fclean all

.PHONY: all clean fclean re
