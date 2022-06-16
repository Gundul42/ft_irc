#include <stdlib.h>
#include <stdio.h>	// perror!
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <ctime>
#include <iostream>
#include <map>
#include "ftClient.hpp"
#include "Commands.hpp"

class IrcServ
{
		private:

				int					_socketfd;
				Commands			_commands;

				IrcServ(const IrcServ & cpy);
				IrcServ & operator=(const IrcServ & rgt);

				void		_add_to_pfds( pollfd *pfds[], int newfd, int *fd_count, int *fd_size);
				void		_del_from_pfds(pollfd pfds[], int i, int *fd_count);
				void		*_get_in_addr(sockaddr *sa);

				std::string	_printTime(void);

		public:
				IrcServ(const char *port);
				~IrcServ(void);

				int		getSocketFd(void) const;
				void	loop(void);
				int		getTimeDiff(ftClient & start);
				int		updateTimeDiff(ftClient & start);
};
