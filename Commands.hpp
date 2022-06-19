#include <string>
#include <stdio.h> //needed for perror
#include <unistd.h> //usleep
#include <sstream>
#include <map>
#include <sys/socket.h>
#include "ftClient.hpp"
#include <iostream>

#define ERR_COMMAND "Command does not exits\n"
#define ERR_NOTOPER "User must be an operator to use the command\n"
#define ERR_NULLPARAM "Command incomplete\n"

#define IRCSERVNAME "ftIrcServ.nowhere.xy"
#define IRCSERVWLC "Welcome to ft_IrcServ V 0.1beta"
#define IRCSERVVERSION "V0.1beta"
#define IRCSERVCDATE "180622"
#define IRCSERVUSERMODES "aiwroOs"
#define IRCSERVCHANMODES "birkfsmqz"

class Commands
{
		typedef int (Commands::*UserCommandPointer)(ftClient&, std::string& param);
		typedef std::map<std::string, UserCommandPointer> userCommandsMap;
		typedef std::map<std::string, UserCommandPointer> serviceCommandsMap;

		userCommandsMap		userCommands;
		serviceCommandsMap	serviceCommands;
		static const		size_t maxLineSize;

		Commands(const Commands& other);
		Commands& operator=(const Commands& other);

		public:
					Commands();
					~Commands();
					void	read_command(int socket, std::stringstream& str, std::string& command,
									std::string& message);
					void	handle_command(ftClient& client, const void* buf);

					bool	sendCommandResponse(const std::string & code, const ftClient & clt) const;
					bool	sendErrorResponse(const std::string & code, const ftClient & clt) const;

					//commands, to be implemented
					int		away(ftClient& client, std::string& param);
					int		die(ftClient& client, std::string& param);
					int		info(ftClient& client, std::string& param);
					int		invite(ftClient& client, std::string& param);
					int		join(ftClient& client, std::string& param);
					int		kick(ftClient& client, std::string& param);
					int		kill(ftClient& client, std::string& param);
					int		list(ftClient& client, std::string& param);
					int		lusers(ftClient& client, std::string& param);
					int		mode(ftClient& client, std::string& param);
					int		motd(ftClient& client, std::string& param);
					int		names(ftClient& client, std::string& param);
					int		nick(ftClient& client, std::string& param);
					int		notice(ftClient& client, std::string& param);
					int		oper(ftClient& client, std::string& param);
					int		part(ftClient& client, std::string& param);
					int		pass(ftClient& client, std::string& param);
					int		ping(ftClient& client, std::string& param);
					int		pong(ftClient& client, std::string& param);
					int		privmsg(ftClient& client, std::string& param);
					int		quit(ftClient& client, std::string& param);
					int		rehash(ftClient& client, std::string& param);
					int		restart(ftClient& client, std::string& param);
					int		service(ftClient& client, std::string& param);
					int		servlist(ftClient& client, std::string& param);
					int		squery(ftClient& client, std::string& param);
					int		stats(ftClient& client, std::string& param);
					int		time(ftClient& client, std::string& param);
					int		topic(ftClient& client, std::string& param);
					int		user(ftClient& client, std::string& param);
					int		userhost(ftClient& client, std::string& param);
					int		version(ftClient& client, std::string& param);
					int		who(ftClient& client, std::string& param);
					int		whois(ftClient& client, std::string& param);
					int		whowas(ftClient& client, std::string& param);
};
