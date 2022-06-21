
#pragma once
#include <iostream>
#include <ctime>

#define FT_IRC_TIMEOUT 20

class ftClient
{
		private:
				int			_fd;			// File descriptor
				std::string	_name;			// NICK name
				std::string	_addr;			// addr string
				bool		_val;			// flag if server has validated the client 
				bool		_oper;			// if the client is an operator
				time_t		_connect;		// time of connection
				time_t		_lastAction;	// when was the last action
				/*In addition to the nickname, all servers must have the
				following information about all clients: the real name of the host
				that the client is running on, the username of the client on that
				host, and the server to which the client is connected.*/
				std::string _host;
				std::string _username;
				std::string _server;
				//maybe more to come, at least a timestamp

				ftClient(const ftClient & cpy);
				ftClient operator=(const ftClient & rgt);

		public:
				ftClient(int fd,const std::string name, const std::string addr);
				~ftClient(void);

				void			validate(void);

				std::string		get_name(void) const;
				void			set_name(const std::string& name);
				std::string 	get_addr(void) const;
				int				get_fd(void) const;

				int				getTimeConnected(void) const;
				time_t			getLastAction(void) const;
				void			setLastAction(time_t newT);

				bool			isRegistered(void) const;
				bool			isOperator(void) const;

				//to be continued
};
