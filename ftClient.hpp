#ifndef FTCLIENT_H
# define FTCLIENT_H

# include "config.hpp"
# include "lib.hpp"

# define FT_IRC_TIMEOUT 20

class UserMode
{
public:
	enum Flag
	{
		/** 'a' - User is flagged as away */
		AWAY = 1,
		/** 'i' - Marks a User as invisible */
		INVISIBLE = 1 << 1,
		/** 'w' - User receives wallops */
		WALLOPS = 1 << 2,
		/** 'r' - Restricted User connection */
		RESTRICTED = 1 << 3,
		/** 'o' - Operator flag */
		OPERATOR = 1 << 4,
		/** 's' - Marks a User for receipt of server notices */
		MARK = 1 << 6
	};

	UserMode(unsigned flags = 0);
	~UserMode();

	static Flag	parse(char c);

protected:
	unsigned					_flags;

private:
	/* Quick lookup table for lower alphabet */
	static const unsigned short	_lowerFlagTable[26];
};

class ftClient : public UserMode
{
		private:
				int			_fd;			// File descriptor
				std::string	_name;			// NICK name
				std::string	_addr;			// addr string
				bool		_val;			// flag if server has validated the client 
				bool		_send;			// flag Oper send a /DIE cmd
				time_t		_connect;		// time of connection
				time_t		_lastAction;	// when was the last action
				int			_msgs;			// nbr of messages sent
				/*In addition to the nickname, all servers must have the
				following information about all clients: the real name of the host
				that the client is running on, the username of the client on that
				host, and the server to which the client is connected.*/
				std::string	_hostname;	//rDNS name of connected client IPv4 ONLY
				std::string	_realname;
				std::string	_username;
				std::string	_server;	//server is static and saved in the macro IRCSERVNAME
				std::string	_password;
				std::string	_awayMsg;
				int			_quit;

				ftClient(const ftClient & cpy);
				ftClient & operator=(const ftClient & rgt);

		public:
				std::string		tmpBuffer;	// tmp buffer until we receive a cr and/or lf

				ftClient(int fd,const std::string name, const std::string addr,
								const std::string host);
				~ftClient(void);

				void			validate(void);

				std::string		get_name(void) const;
				std::string 	get_addr(void) const;
				std::string		get_hostname(void) const;
				int				get_fd(void) const;
				int				get_msgs(void) const;
				std::string		get_prefix(void) const; //get usermask
				unsigned		get_flags(void);
				std::string		get_awaymsg(void);
				std::string		get_username(void) const;
				int				get_quit(void) const;
				bool			get_send(void) const; //server end

				void			set_name(const std::string& name);
				void			set_names(const std::string& username, const std::string& realname);
				void			set_pass(const std::string& pass);
				void			set_flags(const std::string& add_remove, unsigned flag);
				void			set_awaymsg(const std::string& msg);
				void			set_quit();
				void			set_send(void); //server end

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
