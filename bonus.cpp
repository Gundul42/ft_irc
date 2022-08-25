/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bonus.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: graja <graja@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 09:49:56 by graja             #+#    #+#             */
/*   Updated: 2022/08/25 11:02:06 by graja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

int main(int argc, char **argv)
{
	int		port;

	if (argc != 4)
	{
			std::cout << argv[0] << ": Error not enough argumments [Address][Port][Password]"
					<< std::endl;
			exit(EXIT_FAILURE);
	}
	port = atoi(argv[2]);
	if (port < 1024 || port > 65535)
	{
			std::cout << "Error: wrong port range [1024 - 65535]" << std::endl;
			exit (EXIT_FAILURE);
	}
	Bot		bot(argv[1], argv[2], argv[3]);
	sleep(1);
	if (!bot.login())
	{
		std::cout << "Error: Login failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << std::endl << FT_BOTNAME << " successfully started !" << std::endl << std::endl;
	bot.loop();
	std::cout << "ERROR: Connection to " << argv[1] << " was lost !" << std::endl;
	exit(1);
}
