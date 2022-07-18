/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwen <mwen@student.42wolfsburg.de>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 18:52:50 by graja             #+#    #+#             */
/*   Updated: 2022/07/08 17:01:49 by graja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServ.hpp"
#include "Channel.hpp"
#include "ftClient.hpp"
int	main(int argc, char **argv)
{
	std::cout << argc << std::endl;
	if (argc != 3)
	{
			std::cout << argv[0] << " [PORT] [PASSWORD]" << std::endl;
			exit (1);
	}

	IrcServ	srv(argv[1], argv[2]);

	srv.loop();

	return(0);
}
