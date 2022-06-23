#ifndef FTCLIENT_H
# define FTCLIENT_H

# include "config.hpp"
# include "lib.hpp"

# define FT_IRC_TIMEOUT 20

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
				int			_msgs;			// nbr of messages sent
				/*In addition to the nickname, all servers must have the
				following information about all clients: the real name of the host
				that the client is running on, the username of the client on that
				host, and the server to which the client is connected.*/
				std::string _host; //same as addr ??? or rather string of rDNS ?
				std::string _username;
				std::string _server;

				ftClient(const ftClient & cpy);
				ftClient & operator=(const ftClient & rgt);

		public:
				ftClient(int fd,const std::string name, const std::string addr);
				~ftClient(void);

				void			validate(void);

				std::string		get_name(void) const;
				void			set_name(const std::string& name);
				std::string 	get_addr(void) const;
				int				get_fd(void) const;
				int				get_msgs(void) const;

				int				getTimeConnected(void) const;
				time_t			getLastAction(void) const;
				void			setLastAction(time_t newT);
				void			add_msgsCount(int nbr);
				void			set_msgsZero(void);

				bool			isRegistered(void) const;
				bool			isOperator(void) const;

				//to be continued
};

#endif
