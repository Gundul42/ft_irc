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

				int					_socketfd;
				userCommandsMap		userCommands;
				serviceCommandsMap	serviceCommands;

				IrcServ(const IrcServ & cpy);
				IrcServ & operator=(const IrcServ & rgt);

				void _add_to_pfds( pollfd *pfds[], int newfd, int *fd_count, int *fd_size);
				void _del_from_pfds(pollfd pfds[], int i, int *fd_count);
				void *_get_in_addr(sockaddr *sa);

				std::string	_printTime(void);
				void	handle_command(int socket, const void* buf);

		public:
				IrcServ(const char *port);
				~IrcServ(void);

				int		getSocketFd(void) const;
				void	loop(void);
				int		getTimeDiff(ftClient & start);
				int		updateTimeDiff(ftClient & start);

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
				//int		time();   //conflicting with time() from <ctime>
				int		topic();
				int		user();
				int		userhost();
				int		version();
				int		who();
				int		whois();
				int		whowas();
};
