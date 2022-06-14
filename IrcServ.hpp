/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: graja <graja@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 18:04:16 by graja             #+#    #+#             */
/*   Updated: 2022/06/14 14:16:16 by graja            ###   ########.fr       */
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
#include "ftClient.hpp"

class IrcServ
{
		private:
				int	socketfd;
				IrcServ(const IrcServ & cpy);

				IrcServ & operator=(const IrcServ & rgt);

				void add_to_pfds( pollfd *pfds[], int newfd, int *fd_count, int *fd_size);
				void del_from_pfds(pollfd pfds[], int i, int *fd_count);
				void *get_in_addr(sockaddr *sa);

		public:
				IrcServ(const char *port);
				~IrcServ(void);

				int		getSocketFd(void) const;
				void	loop(void);
};
