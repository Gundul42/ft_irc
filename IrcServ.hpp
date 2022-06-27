#ifndef IRCSERV_H
# define IRCSERV_H

# include "lib.hpp"
# include "config.hpp"
# include "Commands.hpp"
# include "ftClient.hpp"

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
				std::string	_rdns(std::string addr);

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
};

#endif
