/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bonus.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: graja <graja@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 09:49:56 by graja             #+#    #+#             */
/*   Updated: 2022/07/29 09:54:59 by graja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Bot.hpp"

int main(int argc, char **argv)
{
	if (argc != 4)
	{
			std::cout << argv[0] << ": Error not enough argumments [Address][Port][Password]"
					<< std::endl;
			exit(EXIT_FAILURE);
	}

	Bot		bot(argv[1], argv[2], argv[3]);

	exit(1);
}

