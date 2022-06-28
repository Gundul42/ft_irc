#include "Commands.hpp"

Commands::Commands()
{
	_userCommands["KILL"] = &Commands::kill;
	_userCommands["NICK"] = &Commands::nick;
	_userCommands["NOTICE"] = &Commands::notice;
	_userCommands["OPER"] = &Commands::oper;
	_userCommands["PASS"] = &Commands::pass;
	_userCommands["PING"] = &Commands::ping;
	_userCommands["PONG"] = &Commands::pong;
	_userCommands["PRIVMSG"] = &Commands::privmsg;
	_userCommands["QUIT"] = &Commands::quit;
	_userCommands["SERVICE"] = &Commands::service;
	_userCommands["SERVLIST"] = &Commands::servlist;
	_userCommands["SQUERY"] = &Commands::squery;
	_userCommands["USER"] = &Commands::user;
	_userCommands["WHO"] = &Commands::who;
	_userCommands["WHOIS"] = &Commands::whois;
	_userCommands["WHOWAS"] = &Commands::whowas;
	_userCommands["AWAY"] = &Commands::away;
	_userCommands["DIE"] = &Commands::die;
	_userCommands["INFO"] = &Commands::info;
	_userCommands["INVITE"] = &Commands::invite;
	_userCommands["JOIN"] = &Commands::join;
	_userCommands["KICK"] = &Commands::kick;
	_userCommands["LIST"] = &Commands::list;
	_userCommands["MODE"] = &Commands::mode;
	_userCommands["MOTD"] = &Commands::motd;
	_userCommands["NAMES"] = &Commands::names;
	_userCommands["PART"] = &Commands::part;
	_userCommands["REHASH"] = &Commands::rehash;
	_userCommands["RESTART"] = &Commands::restart;
	_userCommands["STATS"] = &Commands::stats;
	_userCommands["TIME"] = &Commands::ustime;
	_userCommands["TOPIC"] = &Commands::topic;
	_userCommands["USERHOST"] = &Commands::userhost;
	_userCommands["VERSION"] = &Commands::version;
	_userCommands["CAP"] = &Commands::cap;

}

Commands::~Commands() {}

void	Commands::handle_command(const std::map<int, ftClient*>& usermap, int socket, const char* buf)
{
	std::stringstream	str(buf);
	this->_users = usermap;

	for (std::string line; std::getline(str, line, '\n'); )
	{
		Message msg(line);
		userCommandsMap::const_iterator it = _userCommands.find(msg.getCommand());
		if (it == _userCommands.end())
			sendCommandResponse(*(_users.find(socket)->second), ERR_UNKNOWNCOMMAND, "Unknown command");
	else
		(this->*(it->second))(*(_users.find(socket)->second), msg);
	}
}

int		Commands::away(ftClient& client, Message& msg) { return 1; }
int		Commands::die(ftClient& client, Message& msg) { return 1; }
int		Commands::info(ftClient& client, Message& msg) { return 1; }
int		Commands::invite(ftClient& client, Message& msg) { return 1; }

//JOIN
int		Commands::join(ftClient& client, Message& msg) 
{
		std::vector<std::string>					params = msg.getParam();
		std::vector<std::string>::const_iterator	itpar;
		servChannel::iterator						itchan;
		bool										isnew = false;
		IrcChannel*									newChan;

		if (params.size() == 0)
			return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
		if (_channels.find(params[0]) == _channels.end())
		{
			newChan = new IrcChannel(params[0], client);
			if (newChan->valChanName(params[0]) == false)
			{
				delete newChan;
				return !sendCommandResponse(client, ERR_NOSUCHCHANNEL, params[0], "No such channel");
			}
			isnew = true;
		}
		if (isnew)
		{
			_channels.insert(std::pair<std::string, IrcChannel*>(params[0], newChan));
			serverSend(client.get_fd(),client.get_name(), "JOIN " + params[0], "");
			serverSend(client.get_fd(),IRCSERVNAME, "MODE " + params[0] + " +Cnst", "");
			sendCommandResponse(client, RPL_NAMREPLY, "@ " + params[0], "@" + client.get_name());
			sendCommandResponse(client, RPL_ENDOFNAMES, params[0], "End of /NAMES list.");
			return 0;
		}
		else
			std::cout << "Existing Channel -- to be continued\n";
		return 0;

}

int		Commands::kick(ftClient& client, Message& msg) { return 1; }
int		Commands::kill(ftClient& client, Message& msg) { return 1; }
int		Commands::list(ftClient& client, Message& msg) { return 1; }
int		Commands::lusers(ftClient& client, Message& msg) { return 1; }

//MODE
int		Commands::mode(ftClient& client, Message& msg)
{
		std::vector<std::string>					params = msg.getParam();
		std::vector<std::string>::const_iterator	itpar;
		servChannel::iterator						itchan;

		if (params.size() == 0)
			return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
		itchan =_channels.find(params[0]);
		if (itchan == _channels.end())
		{
			return !sendCommandResponse(client, ERR_NOSUCHCHANNEL, params[0], "No such channel");
		}
		if (params.size() == 1)
		{
			serverSend(client.get_fd(),IRCSERVNAME, "324 " +
				(*itchan).second->getCreator()->get_name() + " " + params[0] + " +Cnst", "");
			serverSend(client.get_fd(),IRCSERVNAME, "329 " +
				(*itchan).second->getCreator()->get_name() + " " + params[0] + " " +
				(*itchan).second->getCtime(), "");
		}
		return 1;
}

//MOTD
int		Commands::motd(ftClient& client, Message& msg)
{
		std::fstream		motd;
		std::ostringstream	tosend;
		std::string			str;

		motd.open(IRCMOTDFILE, std::ios::in);
		if (!motd.is_open())
		{
			sendCommandResponse(client, ERR_NOMOTD, "MOTD File is missing");
			return (1);
		}
		tosend << IRCSERVNAME << " Message of the day";
		sendCommandResponse(client, RPL_MOTDSTART, tosend.str());
		while (getline(motd, str))
			sendCommandResponse(client, RPL_MOTD, str);
		sendCommandResponse(client, RPL_ENDOFMOTD, "End of /MOTD command.");
		motd.close();
		return 1;
}

//NAMES
int		Commands::names(ftClient& client, Message& msg) { return 1; }

//NICK
int		Commands::nick(ftClient& client, Message& msg)
{
	std::string oldnick = client.get_name();
	std::string	newnick = msg.getParam().front();

	if (msg.getParam().empty())
			return !serverSend(client.get_fd(), "", "431 " + oldnick + " " + newnick,
							"No nickname given");
	else if (newnick.size() > 9 || newnick[0] < 64 || newnick.find(' ') != std::string::npos)
			return !serverSend(client.get_fd(), "", "432 " + oldnick + " " + newnick, "Erroneus nickname");
	std::map<int, ftClient*>::const_iterator it = this->_users.begin();
	for (; it != this->_users.end(); it++)
		if (newnick == (*it).second->get_name())
			return !serverSend(client.get_fd(), "", "433 " + oldnick + " " + newnick,
							"Nickname is already in use");
	//check collision?
	client.set_name(msg.getParam().front());
	if (oldnick.empty())
		oldnick = newnick;
	return serverSend(client.get_fd(), oldnick,  msg.getCommand(), newnick);
}
int		Commands::notice(ftClient& client, Message& msg) { return 1; }
int		Commands::oper(ftClient& client, Message& msg)
{
	if (!client.isRegistered())
		return !serverSend(client.get_fd(), "", "451 ", "You have not registered");
	else if (msg.getParam().size() < 2)
		return !serverSend(client.get_fd(), "", "461 " + msg.getCommand(), "Not enough parameters");
	std::map<std::string, Oper*>::const_iterator it = this->_operList.getOperList().find(client.get_name());
	if ( it == this->_operList.getOperList().end() || (*it).second->getPass() != msg.getParam()[1])
		return !serverSend(client.get_fd(), "", "464 ", ":Password incorrect");
	if ((*it).second->getHost() != client.get_addr())
		return !serverSend(client.get_fd(), "", "491 ", ":No O-lines for your host");
	//set usermode
}
int		Commands::part(ftClient& client, Message& msg) { return 1; }

//PASS
int		Commands::pass(ftClient& client, Message& msg)
{
	if (client.isRegistered())
		return !serverSend(client.get_fd(), "", "462 ", "You may not reregister");
	else if (msg.getParam().empty())
		return !serverSend(client.get_fd(), "", "461 " + msg.getCommand(), "Not enough parameters");
	client.set_pass(msg.getParam().front());
	return true;
}

//PING
int		Commands::ping(ftClient& client, Message& msg)
{
	std::ostringstream oss;

	oss << "PONG " << IRCSERVNAME;
	if (msg.getParam().empty())
		return !serverSend(client.get_fd(), "", "461 " + msg.getCommand(), "Not enough parameters");
	else
		return serverSend(client.get_fd(), "", oss.str(), msg.getParam().front());
}
int		Commands::pong(ftClient& client, Message& msg) { return true; }
int		Commands::privmsg(ftClient& client, Message& msg) { return 1; }
int		Commands::quit(ftClient& client, Message& msg) { return 1; }
int		Commands::rehash(ftClient& client, Message& msg) { return 1; }

//RESTART - RFC2812 4.4 ... this message is optional ...
int		Commands::restart(ftClient& client, Message& msg) { return 1; }

//SERVICE
//should not be allowed from client rfc2813, 4.1.4
int		Commands::service(ftClient& client, Message& msg) { return 1; }

//SERVLIST
//bonus would be a service ? a bot ?
int		Commands::servlist(ftClient& client, Message& msg) { return 1; }

//SQUERY
//this would be a PM to a service
int		Commands::squery(ftClient& client, Message& msg) { return 1; }

//STATS
//all stats about the server, this would mean to register each byte going in and out !
int		Commands::stats(ftClient& client, Message& msg) { return 1; }

//TIME
int		Commands::ustime(ftClient& client, Message& msg)
{
		std::vector<std::string>	params = msg.getParam();
   		time_t 						now;
		time(&now);
   		std::string					mytime(ctime(&now));
		std::ostringstream			oss;

		if (!params.empty() && params[0] != IRCSERVNAME)
			return !sendCommandResponse(client, ERR_NOSUCHSERVER, params[0], "No such server");
		oss << "391 " << IRCSERVNAME;
   		mytime =  mytime.substr(0, mytime.size() - 1);
		return serverSend(client.get_fd(), "", oss.str(), mytime);
}

//TOPIC
int		Commands::topic(ftClient& client, Message& msg) { return 1; }

//USER
int		Commands::user(ftClient& client, Message& msg)
{
		std::string			username = msg.getParam().front();
		std::string			realname = msg.getTrailing();
		int					i = 0;

		serverSend(client.get_fd(), "", "001 " + client.get_name(), "Welcome to the Internet Relay Network " + client.get_prefix());

		if (client.isRegistered())
			return !sendCommandResponse(client, ERR_ALREADYREGISTRED, "You may not reregister");
		else if (msg.getParam().size() < 3)
			return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
		//need to test below: 
		/*Between servers USER must to be prefixed with client's NICKname.
		Note that hostname and servername are normally ignored by the IRC
		server when the USER command comes from a directly connected client
		(for security reasons), but they are used in server to server
		communication.*/
		client.set_username(username);
		client.set_realname(realname);
		client.validate();
		motd(client, msg); //show motd
		return true;
}
int		Commands::userhost(ftClient& client, Message& msg) { return 1; }

//VERSION
int		Commands::version(ftClient& client, Message& msg)
{
		std::vector<std::string>					params = msg.getParam();

		if (!params.empty() && params[0] != IRCSERVNAME)
			return !sendCommandResponse(client, ERR_NOSUCHSERVER, params[0], "No such server");
		return !sendCommandResponse(client, RPL_VERSION, IRCSERVVERSION, "");
}

int		Commands::who(ftClient& client, Message& msg) { return 1; }
int		Commands::whois(ftClient& client, Message& msg) { return 1; }
int		Commands::whowas(ftClient& client, Message& msg) { return 1; }
int		Commands::cap(ftClient& client, Message& msg) { return true; }


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

bool Commands::sendCommandResponse(const ftClient & clt, const int & code, 
				const std::string & argument, const std::string & trailer) const
{
	std::ostringstream	tosend;
	std::string			go;

	tosend << ":" << IRCSERVNAME << " " << code << " " << clt.get_name() << " " << argument << " :";
	tosend << trailer << "\x0d\x0a";
	go = tosend.str();
	if (send(clt.get_fd(), go.c_str(), go.length(), 0) == -1)
		perror("sendCommandResponse");
	usleep(100); 
	return true;
}

bool Commands::serverSend(int fd, std::string prefix, std::string msg, std::string trl)
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
		return true;
}

