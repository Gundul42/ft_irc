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
	std::string pong = ":ftIrcServ.nowhere.xy PONG " + msg.getParam() + "\x0d\x0a";
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

int		Commands::cap(ftClient& client, Message& msg) { return true; }
