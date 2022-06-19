#include <stdlib.h>
#include <unistd.h>	// usleep
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

#define IRCSERVNAME "ftIrcServ.nowhere.xy"
#define IRCSERVWLC "Welcome to ft_IrcServ V 0.1beta"
#define IRCSERVVERSION "V0.1beta"
#define IRCSERVCDATE "180622"
#define IRCSERVUSERMODES "aiwroOs"
#define IRCSERVCHANMODES "birkfsmqz"


class IrcServ
{
		private:

				int								_socketfd;
				Commands						_commands;
				std::map<int, ftClient*>		_connections;

				IrcServ(const IrcServ & cpy);
				IrcServ & operator=(const IrcServ & rgt);

				void		_add_to_pfds( pollfd *pfds[], int newfd, int *fd_count, int *fd_size);
				void		_del_from_pfds(pollfd pfds[], int i, int *fd_count);
				void		*_get_in_addr(sockaddr *sa);

				std::string	_printTime(void) const;
				void		_logAction(const std::string & log) const;
				void		_debugBuffer(const char *buf) const;

		public:
				IrcServ(const char *port);
				~IrcServ(void);

				int		getSocketFd(void) const;
				void	loop(void);
				int		getTimeDiff(ftClient & start);
				int		updateTimeDiff(ftClient & start);
				void	check_valid_client(pollfd *pfds,int *fd_count);
				bool	NickExists(const std::string & nick) const;

};
