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
}

Commands::~Commands() {}


void	Commands::read_command(int socket, std::stringstream& str, std::string& command,
				std::string& param)
{
	//pending: add buf size exceed limit test
	std::getline(str, command, ' ');
	if (*(command.end() - 1) == '\n')
	{
		command.erase(command.find('\n'));
		param = ""; //Segfaults + compiler telling me nullptr is C++11 
		return ;
	}
	std::getline(str, param, '\n');
	if (*(param.end() - 1) == '\n')
		param.erase(param.find('\n'));
}

void	Commands::handle_command(ftClient& client, const void* buf)
{
	std::string			command;
	std::string			param;
	std::stringstream	str(static_cast<const char*>(buf));

	this->read_command(client.get_fd(), str, command, param);
	userCommandsMap::const_iterator it = userCommands.find(command);
	if (it == userCommands.end())
	{
		if (send(client.get_fd(), ERR_COMMAND, sizeof(ERR_COMMAND), 0) == -1)
			perror(ERR_COMMAND);
	}
	else
		(this->*(it->second))(client, param);
}

int		Commands::away(ftClient& client, std::string& param) { return 1; }
int		Commands::die(ftClient& client, std::string& param) { return 1; }
int		Commands::info(ftClient& client, std::string& param) { return 1; }
int		Commands::invite(ftClient& client, std::string& param) { return 1; }
int		Commands::join(ftClient& client, std::string& param) { return 1; }
int		Commands::kick(ftClient& client, std::string& param) { return 1; }
int		Commands::kill(ftClient& client, std::string& param) { return 1; }
int		Commands::list(ftClient& client, std::string& param) { return 1; }
int		Commands::lusers(ftClient& client, std::string& param) { return 1; }
int		Commands::mode(ftClient& client, std::string& param) { return 1; }
int		Commands::motd(ftClient& client, std::string& param) { return 1; }
int		Commands::names(ftClient& client, std::string& param) { return 1; }
int		Commands::nick(ftClient& client, std::string& param) { return 1; }
int		Commands::notice(ftClient& client, std::string& param) { return 1; }
int		Commands::oper(ftClient& client, std::string& param) { return 1; }
int		Commands::part(ftClient& client, std::string& param) { return 1; }
int		Commands::pass(ftClient& client, std::string& param) { return 1; }
int		Commands::ping(ftClient& client, std::string& param)
{
	std::string pong = "PONG " + param + "\n";
	if (param.empty())
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
int		Commands::pong(ftClient& client, std::string& param) { return 1; }
int		Commands::privmsg(ftClient& client, std::string& param) { return 1; }
int		Commands::quit(ftClient& client, std::string& param) { return 1; }
int		Commands::rehash(ftClient& client, std::string& param) { return 1; }
int		Commands::restart(ftClient& client, std::string& param) { return 1; }
int		Commands::service(ftClient& client, std::string& param) { return 1; }
int		Commands::servlist(ftClient& client, std::string& param) { return 1; }
int		Commands::squery(ftClient& client, std::string& param) { return 1; }
int		Commands::stats(ftClient& client, std::string& param) { return 1; }
int		Commands::time(ftClient& client, std::string& param) { return 1; }
int		Commands::topic(ftClient& client, std::string& param) { return 1; }
int		Commands::user(ftClient& client, std::string& param) { return 1; }
int		Commands::userhost(ftClient& client, std::string& param) { return 1; }
int		Commands::version(ftClient& client, std::string& param) { return 1; }
int		Commands::who(ftClient& client, std::string& param) { return 1; }
int		Commands::whois(ftClient& client, std::string& param) { return 1; }
int		Commands::whowas(ftClient& client, std::string& param) { return 1; }
