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
			sendCommandResponse(*(users.find(socket)->second), ERR_UNKNOWNCOMMAND, "Unknown command");
	else
		(this->*(it->second))(*(users.find(socket)->second), msg);
	}
}

int		Commands::away(ftClient& client, Message& msg) { return 1; }
int		Commands::die(ftClient& client, Message& msg) { return 1; }
int		Commands::info(ftClient& client, Message& msg) { return 1; }
int		Commands::invite(ftClient& client, Message& msg) { return 1; }
int		Commands::join(ftClient& client, Message& msg) 
{
		servChannel::const_iterator it;
		if (channels.size() > 0 && channels.find(msg.getParam().front()) != channels.end())
			return 1;
		channels.insert(std::pair<std::string, IrcChannel*>(msg.getParam().front(), IrcChannel(msg.getParam().front())));
		return 0; 
}

int		Commands::kick(ftClient& client, Message& msg) { return 1; }
int		Commands::kill(ftClient& client, Message& msg) { return 1; }
int		Commands::list(ftClient& client, Message& msg) { return 1; }
int		Commands::lusers(ftClient& client, Message& msg) { return 1; }
int		Commands::mode(ftClient& client, Message& msg) { return 1; }
int		Commands::motd(ftClient& client, Message& msg) { return 1; }
int		Commands::names(ftClient& client, Message& msg) { return 1; }
int		Commands::nick(ftClient& client, Message& msg)
{
	std::string oldnick = client.get_name();
	std::string	newnick = msg.getParam().front();

	if (msg.getParam().empty())
		return !sendCommandResponse(client, ERR_NONICKNAMEGIVEN, "No nickname given");
	else if (newnick.size() > 9 || newnick[0] < 64 || newnick.find(' ') != std::string::npos)
		return !sendCommandResponse(client, ERR_ERRONEUSNICKNAME, "Erroneus nickname");

	std::map<int, ftClient*>::const_iterator it = this->users.begin();
	for (; it != this->users.end(); it++)
	{
		if (newnick == (*it).second->get_name())
			return !sendCommandResponse(client, ERR_NICKNAMEINUSE, "Nickname is already in use");
	}
	//check collision?
	client.set_name(msg.getParam().front());
	if (oldnick.empty())
		oldnick = newnick;
	std::string response = ":" + oldnick + " " + msg.getCommand() + " " + newnick + "\x0d\x0a";
	if (send(client.get_fd(), response.c_str(), response.length(), 0) == -1)
		perror("sendNickResponse");
	return true;
}
int		Commands::notice(ftClient& client, Message& msg) { return 1; }
int		Commands::oper(ftClient& client, Message& msg) { return 1; }
int		Commands::part(ftClient& client, Message& msg) { return 1; }
int		Commands::pass(ftClient& client, Message& msg)
{
	if (client.isRegistered())
		return !sendCommandResponse(client, ERR_ALREADYREGISTRED, "You may not reregister");
	else if (msg.getParam().empty())
		return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
	client.set_pass(msg.getParam().front());
	return true;
}

int		Commands::ping(ftClient& client, Message& msg)
{
	std::string pong = ":ftIrcServ.nowhere.xy PONG " + msg.getParam().front() + "\x0d\x0a";
	if (msg.getParam().empty())
		return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
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
		std::string			username = msg.getParam().front();
		std::string			realname = msg.getTrailing();
		int					i = 0;

		sendCommandResponse(client, "001", "welcome");

		if (client.isRegistered())
			return !sendCommandResponse(client, ERR_ALREADYREGISTRED, "You may not reregister");
		else if (msg.getParam().size() < 3)
			return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
		//need to test below
		/*Between servers USER must to be prefixed with client's NICKname.
		Note that hostname and servername are normally ignored by the IRC
		server when the USER command comes from a directly connected client
		(for security reasons), but they are used in server to server
		communication.*/
		client.set_username(username);
		client.set_realname(realname);
		client.validate();
		return true;
}
int		Commands::userhost(ftClient& client, Message& msg) { return 1; }
int		Commands::version(ftClient& client, Message& msg) { return 1; }
int		Commands::who(ftClient& client, Message& msg) { return 1; }
int		Commands::whois(ftClient& client, Message& msg) { return 1; }
int		Commands::whowas(ftClient& client, Message& msg) { return 1; }


bool Commands::sendCommandResponse(const ftClient & clt, const int & code, 
				const std::string & trailer) const
{
	std::ostringstream	tosend;

	tosend << code;
	return sendCommandResponse(clt, tosend.str(), trailer);
}

bool Commands::sendCommandResponse(const ftClient & clt, const std::string & code, 
				const std::string & trailer) const
{
	std::ostringstream	tosend;
	std::string			go;

	tosend << ":" << IRCSERVNAME << " " << code << " " << clt.get_name() << " :";
	tosend << trailer << "\x0d\x0a";
	go = tosend.str();
	if (send(clt.get_fd(), go.c_str(), go.length(), 0) == -1)
		perror("sendCommandResponse");
	usleep(100); 
	return true;
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
