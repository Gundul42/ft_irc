#include "Commands.hpp"

Commands::Commands()
{
	serviceCommands["KILL"] = &Commands::kill;
	serviceCommands["NICK"] = &Commands::nick;
	serviceCommands["NOTICE"] = &Commands::notice;
	serviceCommands["OPER"] = &Commands::oper;
	serviceCommands["PASS"] = &Commands::pass;
	serviceCommands["PING"] = &Commands::ping;
	serviceCommands["PONG"] = &Commands::pong;
	serviceCommands["PRIVMSG"] = &Commands::privmsg;
	serviceCommands["QUIT"] = &Commands::quit;
	serviceCommands["SERVICE"] = &Commands::service;
	serviceCommands["SERVLIST"] = &Commands::servlist;
	serviceCommands["SQUERY"] = &Commands::squery;
	serviceCommands["USER"] = &Commands::user;
	serviceCommands["WHO"] = &Commands::who;
	serviceCommands["WHOIS"] = &Commands::whois;
	serviceCommands["WHOWAS"] = &Commands::whowas;
	userCommands = serviceCommands;
	userCommands["AWAY"] = &Commands::away;
	userCommands["DIE"] = &Commands::die;
	userCommands["INFO"] = &Commands::info;
	userCommands["INVITE"] = &Commands::invite;
	userCommands["JOIN"] = &Commands::join;
	userCommands["KICK"] = &Commands::kick;
	userCommands["LIST"] = &Commands::list;
	userCommands["LUSERS"] = &Commands::lusers;
	userCommands["MODE"] = &Commands::mode;
	userCommands["MOTD"] = &Commands::motd;
	userCommands["NAMES"] = &Commands::names;
	userCommands["PART"] = &Commands::part;
	userCommands["REHASH"] = &Commands::rehash;
	userCommands["RESTART"] = &Commands::restart;
	userCommands["STATS"] = &Commands::stats;
	userCommands["TIME"] = &Commands::time;
	userCommands["TOPIC"] = &Commands::topic;
	userCommands["USERHOST"] = &Commands::userhost;
	userCommands["VERSION"] = &Commands::version;
	userCommands["CAP"] = &Commands::cap;

}

Commands::~Commands() {}

void	Commands::handle_command(const std::map<int, ftClient*>& usermap, int socket, const char* buf)
{
	std::stringstream	str(buf);
	this->users = usermap;

	for (std::string line; std::getline(str, line, '\n'); )
	{
		Message msg(line);
		userCommandsMap::const_iterator it = userCommands.find(msg.getCommand());
		if (it == userCommands.end())
		{
			if (send(socket, ERR_COMMAND, sizeof(ERR_COMMAND), 0) == -1)
				perror(ERR_COMMAND);
		}
	else
		(this->*(it->second))(*(users.find(socket)->second), msg);
	}
}

int		Commands::away(ftClient& client, Message& msg) { return 1; }
int		Commands::die(ftClient& client, Message& msg) { return 1; }
int		Commands::info(ftClient& client, Message& msg) { return 1; }
int		Commands::invite(ftClient& client, Message& msg) { return 1; }
int		Commands::join(ftClient& client, Message& msg) { return 1; }
int		Commands::kick(ftClient& client, Message& msg) { return 1; }
int		Commands::kill(ftClient& client, Message& msg) { return 1; }
int		Commands::list(ftClient& client, Message& msg) { return 1; }
int		Commands::lusers(ftClient& client, Message& msg) { return 1; }
int		Commands::mode(ftClient& client, Message& msg) { return 1; }
int		Commands::motd(ftClient& client, Message& msg) { return 1; }
int		Commands::names(ftClient& client, Message& msg) { return 1; }
int		Commands::nick(ftClient& client, Message& msg)
{
	if (msg.getParam().empty())
	{
		std::cout << "1\n";
		//send ERR_NONICKNAMEGIVEN
		return false;
	}
	else if (msg.getParam().size() > 9 || msg.getParam()[0] < 64 || msg.getParam().find(' ') != std::string::npos)
	{
		std::cout << "2\n";
		//send ERR_ERRONEUSNICKNAME
		return false;
	}

	std::map<int, ftClient*>::const_iterator it = this->users.begin();
	for (; it != this->users.end(); it++)
	{
		if (msg.getParam() == (*it).second->get_name())
		{
			std::cout << "3\n";
			//send ERR_NICKNAMEINUSE
			return false;
		}
	}

	//check collision - not sure if we need to implement...
	/*Returned by a server to a client when it detects a
	nickname collision (registered of a NICK that
	already exists by another server).*/

	client.set_name(msg.getParam());
	// std::cout << "Name is " << client.get_name() << "\n";
	return true;
}
int		Commands::notice(ftClient& client, Message& msg) { return 1; }
int		Commands::oper(ftClient& client, Message& msg) { return 1; }
int		Commands::part(ftClient& client, Message& msg) { return 1; }
int		Commands::pass(ftClient& client, Message& msg) { return 1; }
int		Commands::ping(ftClient& client, Message& msg)
{
	std::string pong = "PONG " + msg.getParam() + "\n";
	if (msg.getParam().empty())
	{
		if (send(client.get_fd(), ERR_NULLPARAM, sizeof(ERR_NULLPARAM), 0) == -1)
			perror("ping param empty");
		return false;
	}
	else
	{
		if (send(client.get_fd(), pong.c_str(), pong.length(), 0) == -1)
			perror("ping send");
		return true;
	}
}
int		Commands::pong(ftClient& client, Message& msg) { return 1; }
int		Commands::privmsg(ftClient& client, Message& msg) { return 1; }
int		Commands::quit(ftClient& client, Message& msg) { return 1; }
int		Commands::rehash(ftClient& client, Message& msg) { return 1; }
int		Commands::restart(ftClient& client, Message& msg) { return 1; }
int		Commands::service(ftClient& client, Message& msg) { return 1; }
int		Commands::servlist(ftClient& client, Message& msg) { return 1; }
int		Commands::squery(ftClient& client, Message& msg) { return 1; }
int		Commands::stats(ftClient& client, Message& msg) { return 1; }
int		Commands::time(ftClient& client, Message& msg) { return 1; }
int		Commands::topic(ftClient& client, Message& msg) { return 1; }
int		Commands::user(ftClient& client, Message& msg)
{
	
}
int		Commands::userhost(ftClient& client, Message& msg) { return 1; }
int		Commands::version(ftClient& client, Message& msg) { return 1; }
int		Commands::who(ftClient& client, Message& msg) { return 1; }
int		Commands::whois(ftClient& client, Message& msg) { return 1; }
int		Commands::whowas(ftClient& client, Message& msg) { return 1; }
int		Commands::cap(ftClient& client, Message& msg) {return true;}
