/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwen <mwen@student.42wolfsburg.de>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 18:52:50 by graja             #+#    #+#             */
/*   Updated: 2022/08/19 12:52:20 by graja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServ.hpp"
#include "Channel.hpp"
#include "ftClient.hpp"

int	main(int argc, char **argv)
{
	int		port;

	if (argc != 3)
	{
			std::cout << argv[0] << " [PORT] [PASSWORD]" << std::endl;
			exit (1);
	}
	port = atoi(argv[1]);
	if (port < 1024 || port > 65534)
	{
			std::cout << "Error: wrong port range [1024 - 65534]" << std::endl;
			exit (1);
	}
	IrcServ	srv(argv[1], argv[2]);
	srv.loop();
	return(0);
}
