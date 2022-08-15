#ifndef FTCLIENT_H
# define FTCLIENT_H

# include "config.hpp"
# include "lib.hpp"

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
				std::string	_hostname;	//rDNS name of connected client IPv4 ONLY
				std::string	_realname;
				std::string	_username;
				std::string	_password;
				std::string	_awayMsg;
				int			_quit;
				std::vector<std::string>	_invited;//channels invited to

				ftClient(const ftClient & cpy);

		public:
				std::string		tmpBuffer;	// tmp buffer until we receive a cr and/or lf

				ftClient(int fd,const std::string name, const std::string addr,
								const std::string host);
				ftClient & operator=(const ftClient & rgt);
				~ftClient(void);

				void			validate(void);
				void			deval(void);

				std::string		get_name(void) const;
				std::string 	get_addr(void) const;
				std::string		get_hostname(void) const;
				int				get_fd(void) const;
				int				get_msgs(void) const;
				std::string		get_prefix(void) const; //get usermask
				unsigned		get_flags(void) const;
				std::string		get_awaymsg(void) const;
				std::string		get_username(void) const;
				int				get_quit(void) const;
				bool			get_send(void) const; //server end
				std::string		getPwd(void) const;

				void			set_name(const std::string& name);
				void			set_names(const std::string& username, const std::string& realname);
				void			setPwd(const std::string& pass);
				void			set_flags(const std::string& add_remove, unsigned flag);
				void			set_awaymsg(const std::string& msg);
				void			set_quit();
				void			set_send(void); //server end
				void			set_fd(const int &fd);

				int				getTimeConnected(void) const;
				time_t			getLastAction(void) const;
				void			setLastAction(time_t newT);
				void			add_msgsCount(int nbr);
				void			set_msgsZero(void);

				bool			isRegistered(void) const;
				bool			isOperator(void) const;
				void			invited(std::string& channel);
				bool			is_invited(std::string& channel);

				//to be continued
};

#endif
