/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwen <mwen@student.42wolfsburg.de>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 18:04:16 by graja             #+#    #+#             */
/*   Updated: 2022/06/14 20:23:48 by mwen             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <iostream>
#include <map>
#include <set>
#include "ftClient.hpp"
#include <string>
#include <sstream>

#define ERR_BUFF "You cannot write text in this buffer\n"

class IrcServ
{
		private:
				typedef int (IrcServ::*UserCommandPointer)();
				typedef std::map<std::string, UserCommandPointer> userCommandsMap;
				typedef std::map<std::string, UserCommandPointer> serviceCommandsMap;

				int					socketfd;
				userCommandsMap		userCommands;
				serviceCommandsMap	serviceCommands;

				IrcServ(const IrcServ & cpy);

				IrcServ & operator=(const IrcServ & rgt);

				void	add_to_pfds( pollfd *pfds[], int newfd, int *fd_count, int *fd_size);
				void	del_from_pfds(pollfd pfds[], int i, int *fd_count);
				void	*get_in_addr(sockaddr *sa);
				void	handle_command(int socket, const void* buf);

		public:
				IrcServ(const char *port);
				~IrcServ(void);

				int		getSocketFd(void) const;
				void	loop(void);

				//commands, to be implemented
				int		away();
				int		die();
				int		info();
				int		invite();
				int		join();
				int		kick();
				int		kill();
				int		list();
				int		lusers();
				int		mode();
				int		motd();
				int		names();
				int		nick();
				int		notice();
				int		oper();
				int		part();
				int		pass();
				int		ping();
				int		pong();
				int		privmsg();
				int		quit();
				int		rehash();
				int		restart();
				int		service();
				int		servlist();
				int		squery();
				int		stats();
				int		time();
				int		topic();
				int		user();
				int		userhost();
				int		version();
				int		who();
				int		whois();
				int		whowas();
};
