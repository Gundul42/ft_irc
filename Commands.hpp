#ifndef COMMANDS_H
# define COMMANDS_H

# include "config.hpp"
# include "lib.hpp"
# include "ftClient.hpp"
# include "Message.hpp"
# include "NumCodes.hpp"
# include "Channel.hpp"

class Commands
{

		private:
				
				typedef int (Commands::*UserCommandPointer)(ftClient&, Message& msg);
				typedef std::map<std::string, UserCommandPointer>				userCommandsMap;
				typedef std::map<int, ftClient*>								userMap;
				typedef std::map<std::string, IrcChannel*>						servChannel;

				userCommandsMap					_userCommands;
				userMap							_users;
				servChannel						_channels;
				std::vector<std::string>		_operList;

				Commands(const Commands& other);
				Commands& operator=(const Commands& other);

				bool	isUser(const std::string& nick);
				void	printList(unsigned int incoming_flag, IrcChannel &channel, 
									const std::string &flag, ftClient &client);
		public:

				Commands();
				~Commands();
				void	handle_command(const std::map<int, ftClient*>& usermap, int socket, 
								std::string buf);
				bool 	sendCommandResponse(const ftClient & clt, const std::string & code, 
								const std::string & trailer) const;
				bool 	sendCommandResponse(const ftClient & clt, const int & code,
								const std::string & trailer) const;
				bool	sendCommandResponse(const ftClient & clt, const int & code, 
								const std::string & argument, const std::string & trailer) const;
				bool	serverSend(int fd, std::string prefix, std::string msg, std::string trl);

				//commands, will need to remove commands that are not used
				int		away(ftClient& client, Message& msg);
				int		die(ftClient& client, Message& msg);
				int		info(ftClient& client, Message& msg);
				int		invite(ftClient& client, Message& msg);
				int		join(ftClient& client, Message& msg);
				int		kick(ftClient& client, Message& msg);
				int		kill(ftClient& client, Message& msg);
				int		list(ftClient& client, Message& msg);
				int		lusers(ftClient& client, Message& msg);
				int		mode(ftClient& client, Message& msg);
				int		motd(ftClient& client, Message& msg);
				int		names(ftClient& client, Message& msg);
				int		nick(ftClient& client, Message& msg);
				int		notice(ftClient& client, Message& msg);
				int		oper(ftClient& client, Message& msg);
				int		part(ftClient& client, Message& msg);
				int		pass(ftClient& client, Message& msg);
				int		ping(ftClient& client, Message& msg);
				int		pong(ftClient& client, Message& msg);
				int		privmsg(ftClient& client, Message& msg);
				int		quit(ftClient& client, Message& msg);
				int		rehash(ftClient& client, Message& msg);
				int		restart(ftClient& client, Message& msg);
				int		service(ftClient& client, Message& msg);
				int		servlist(ftClient& client, Message& msg);
				int		squery(ftClient& client, Message& msg);
				int		stats(ftClient& client, Message& msg);
				int		ustime(ftClient& client, Message& msg);
				int		topic(ftClient& client, Message& msg);
				int		user(ftClient& client, Message& msg);
				int		userhost(ftClient& client, Message& msg);
				int		version(ftClient& client, Message& msg);
				int		who(ftClient& client, Message& msg);
				int		whois(ftClient& client, Message& msg);
				int		whowas(ftClient& client, Message& msg);
				int		cap(ftClient& client, Message& msg);
};

#endif
