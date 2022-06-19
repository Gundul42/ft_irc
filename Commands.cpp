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
	if ((command.find(':') != std::string::npos)) //if trailer is found cut it off
	{
		std::getline(str, command, ':');
		param = ""; //Segfaults + compiler telling me nullptr is C++11 
		return ;
	}
	std::getline(str, param, ':'); //cut until trailer found or nothing
	//if (*(param.end() - 1) == '\n')
	//	param.erase(param.find('\n'));
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
int		Commands::nick(ftClient& client, std::string& param)
{ 
		if (client.isRegistered() == false)
		{
				client.set_name(param);
				client.validate();
				return (1);
		}
		//check for already registered client names before setting new nick
		//return error code in case of already existing
		client.set_name(param);
		serverSend(client.get_fd(), "", "NICK", param);
		return 1; 
}
int		Commands::notice(ftClient& client, std::string& param) { return 1; }
int		Commands::oper(ftClient& client, std::string& param) { return 1; }
int		Commands::part(ftClient& client, std::string& param) { return 1; }
int		Commands::pass(ftClient& client, std::string& param) { return 1; }
int		Commands::ping(ftClient& client, std::string& param)
{
	std::string pong = ":ftIrcServ.nowhere.xy PONG " + param + "\x0d\x0a";
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
int		Commands::user(ftClient& client, std::string& param)
{ 
		//std::cout << std::endl << param << std::endl;
		sendCommandResponse("001", client);
		return 1; 
}
int		Commands::userhost(ftClient& client, std::string& param) { return 1; }
int		Commands::version(ftClient& client, std::string& param) { return 1; }
int		Commands::who(ftClient& client, std::string& param) { return 1; }
int		Commands::whois(ftClient& client, std::string& param) { return 1; }
int		Commands::whowas(ftClient& client, std::string& param) { return 1; }

bool Commands::sendCommandResponse(const std::string & code, const ftClient & clt) const
{
	std::ostringstream	tosend;
	std::string			go;

	if (code !="001")
			return false;
	tosend << ":" << IRCSERVNAME << " " << code << " " << clt.get_name() << " :welcome\x0d\x0a";
	go = tosend.str();
	if (send(clt.get_fd(), go.c_str(), go.length(), 0) == -1)
		perror("sendCommandResponse");
	usleep(100); 
	return true;
}

bool Commands::sendErrorResponse(const std::string & code, const ftClient & clt) const
{
		//todo
}

void Commands::serverSend(int fd, std::string prefix, std::string msg, std::string trl)
{
		std::string	tosend;

		if (prefix.length() == 0)
				prefix = IRCSERVNAME;
		if (trl.length() == 0)
				trl = IRCSERVNAME;

		tosend = ":" + prefix + " " + msg + " :" + trl + "\x0d\x0a";
		if (send(fd, tosend.c_str(), tosend.length(), 0) == -1)
				perror("serverSend");
		usleep(100); // break of 0.1s to avoid of omitting this msg in case of a following close()
}

