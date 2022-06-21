#include <string>
#include <stdio.h> //needed for perror
#include <sstream>
#include <map>
#include <sys/socket.h>
#include "ftClient.hpp"
#include <iostream>
#include "Message.hpp"

#define ERR_COMMAND "Command does not exits\n"
#define ERR_NOTOPER "User must be an operator to use the command\n"
#define ERR_NULLPARAM "Command incomplete\n"
#define ERR_NICKEXIST "Nickname already existed\n"

class Commands
{
		typedef int (Commands::*UserCommandPointer)(ftClient&, Message& msg);
		typedef std::map<std::string, UserCommandPointer>	userCommandsMap;
		typedef std::map<std::string, UserCommandPointer>	serviceCommandsMap;
		typedef std::map<int, ftClient*>					userMap;

		userCommandsMap		userCommands;
		serviceCommandsMap	serviceCommands;
		userMap				users;
		static const		size_t maxLineSize;

		Commands(const Commands& other);
		Commands& operator=(const Commands& other);

		public:
					Commands();
					~Commands();
					void	handle_command(const std::map<int, ftClient*>& usermap, int socket, const char* buf);

					//commands, to be implemented
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
					int		time(ftClient& client, Message& msg);
					int		topic(ftClient& client, Message& msg);
					int		user(ftClient& client, Message& msg);
					int		userhost(ftClient& client, Message& msg);
					int		version(ftClient& client, Message& msg);
					int		who(ftClient& client, Message& msg);
					int		whois(ftClient& client, Message& msg);
					int		whowas(ftClient& client, Message& msg);
					int		cap(ftClient& client, Message& msg);

};
