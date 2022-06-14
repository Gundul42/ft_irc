# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: graja <graja@student.42wolfsburg.de>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/12/27 14:36:06 by graja             #+#    #+#              #
#    Updated: 2022/06/14 13:38:02 by graja            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	ft_irc	

SRCS	=	IrcServ.cpp run.cpp ftClient.cpp

CC	=	c++

FLAGS	=	-Wextra -Werror -Wall -std=c++98

$(NAME)	:	$(SRCS)
	$(CC) $(FLAGS) $(SRCS) -o $(NAME)

all	:	$(NAME)

clean	:

fclean	:	
	rm -f $(NAME)

re	:	fclean all
