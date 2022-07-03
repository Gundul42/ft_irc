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

void	Commands::handle_command(const std::map<int, ftClient*>& usermap, int socket, std::string buf)
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

int		Commands::away(ftClient& client, Message& msg)
{
	if (client.get_name().empty())
		return !serverSend(client.get_fd(), "", "", "You are not registered yet.");
	if (!msg.getTrailing().size())
	{
		client.set_flags("-", UserMode::AWAY);
		return serverSend(client.get_fd(), "", "305 ", "You are no longer marked as being away");
	}
	else
	{
		client.set_flags("+", UserMode::AWAY);
		client.set_awaymsg(msg.getTrailing());
		return serverSend(client.get_fd(), "", "306 ", "You have been marked as being away");
	}
}

int		Commands::die(ftClient& client, Message& msg) { return 1; }
int		Commands::info(ftClient& client, Message& msg) { return 1; }
int		Commands::invite(ftClient& client, Message& msg) { return 1; }

//JOIN
int		Commands::join(ftClient& client, Message& msg) 
{
		std::vector<std::string>					params = msg.getParam();
		servChannel::iterator						itchan;
		bool										isnew = false;
		IrcChannel*									newChan;

		if (client.get_name().empty())
			return !serverSend(client.get_fd(), "", "", "You are not registered yet.");
		if (params.size() == 0)
			return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
		if (params[0] == "0")
		{
			if (_channels.size())
			{
				itchan = _channels.begin();
				for (; itchan != _channels.end();)
				{
					Message m((*itchan).second->getName());
					if ((*itchan).second->isMember(client))
					{
						itchan++;
						part(client, m);
					}
				}
			}
			return true;
		}
		for (int i = 0; i != params.size(); i++)
		{
			if ((itchan = _channels.find(params[i])) == _channels.end())
			{
				newChan = new IrcChannel(params[i], client);
				if (newChan->valChanName(params[i]) == false)
				{
					delete newChan;
					return !sendCommandResponse(client, ERR_NOSUCHCHANNEL, params[i], "No such channel");
				}
				isnew = true;
			}
			if (isnew)
			{
				if (params[i][0] == '#')
				{
					newChan->setFlags("+", ChannelMode::parse('n'));
					newChan->setFlags("+", ChannelMode::parse('s'));
					newChan->setFlags("+", ChannelMode::parse('t'));
				}
				_channels.insert(std::pair<std::string, IrcChannel*>(params[i], newChan));
				serverSend(client.get_fd(),client.get_name(), "JOIN " + params[i], "");
				serverSend(client.get_fd(),IRCSERVNAME, "MODE " + params[i] + " +" + newChan->toString(), "");
				sendCommandResponse(client, RPL_NAMREPLY, "@ " + params[i], "@" + client.get_name());
				sendCommandResponse(client, RPL_ENDOFNAMES, params[i], "End of /NAMES list.");
			}
			else
			{
				//below error cases need to be tested, hostname check will have problem as ipv4 and ipv6 rDNS problem, currently all hostnames are ""
				if ((*itchan).second->isBanned(client))
					return !sendCommandResponse(client, ERR_BANNEDFROMCHAN, params[i],
								"Cannot join channel (+b)");
				else if ((*itchan).second->getLimit() > 0 && (*itchan).second->getLimit() <= (*itchan).second->getMembers().size())
					return !sendCommandResponse(client, ERR_CHANNELISFULL, params[i],
								"Cannot join channel (+l)");
				else if ((*itchan).second->getFlags() & ChannelMode::INVITE_ONLY && !(*itchan).second->isInvited(client))
					return !sendCommandResponse(client, ERR_INVITEONLYCHAN, params[i],
								"Cannot join channel (+i)");
				else if ((*itchan).second->getFlags() & ChannelMode::KEY && msg.getKeys()[i] != (*itchan).second->getKey())
					return !sendCommandResponse(client, ERR_BADCHANNELKEY, params[i],
								"Cannot join channel (+k)");
				else if (!(*itchan).second->isMember(client))
				{
					(*itchan).second->addMember(client);
					//respond 332 353
					return serverSend(client.get_fd(),client.get_name(), "JOIN " + params[i], "");
				}
			}
		}
		return true;
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

		if (client.get_name().empty())
			return !serverSend(client.get_fd(), "", "", "You are not registered yet.");
		if (params.size() == 0)
			return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
		if (msg.isChannel(params[0]))
		{
			itchan =_channels.find(params[0]);
			if (itchan == _channels.end())
				return !sendCommandResponse(client, ERR_NOSUCHCHANNEL, params[0], "No such channel");
			if (params.size() == 1)
			{
				serverSend(client.get_fd(),IRCSERVNAME, "324 " +
					client.get_name() + " " + params[0] + " +" +
					(*itchan).second->toString(), "");
				return serverSend(client.get_fd(),IRCSERVNAME, "329 " +
					client.get_name() + " " + params[0] + " " +
					(*itchan).second->getCtime(), "");
			}
			else
			{
				for (int i = 1; i != msg.getFlags().size(); i++)
				{
					//response not shown on client
					if (ChannelMode::parse(msg.getFlags()[i][0]) == 0 || msg.getFlags()[i] == "a")
						return !serverSend(client.get_fd(), "", "472 " +
								client.get_name() + " " + msg.getFlags()[i] + " ", "is an unknown mode char to me");
					else if (msg.getFlags()[0] == "+" && (ChannelMode::parse(msg.getFlags()[i][0]) & (*itchan).second->getFlags()))
						return false;
					(*itchan).second->setFlags(msg.getFlags()[0], ChannelMode::parse(msg.getFlags()[i][0]));
						return serverSend(client.get_fd(), client.get_name(), "MODE " + (*itchan).second->getName() + msg.getFlags()[0] + " " + msg.getFlags()[0], "");
					//set mask, key, limit etc
				}
			}
		}
		//MODE USER
		return 1;
}

//MOTD
int		Commands::motd(ftClient& client, Message& msg)
{
		std::fstream		motd;
		std::ostringstream	tosend;
		std::string			str;

		if (client.get_name().empty())
			return !serverSend(client.get_fd(), "", "", "You are not registered yet.");
		motd.open(IRCMOTDFILE, std::ios::in);
		if (!motd.is_open())
			return !sendCommandResponse(client, ERR_NOMOTD, "MOTD File is missing");
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
	std::string	newnick;

	if (msg.getParam().empty())
			return !serverSend(client.get_fd(), "", "431 " + oldnick + " " + newnick,
							"No nickname given");
	newnick = msg.getParam().front();
	if (newnick.size() > 9 || newnick[0] < 64 || newnick.find(' ') != std::string::npos)
			return !serverSend(client.get_fd(), "", "432 " + oldnick + " " + newnick, "Erroneus nickname");
	std::map<int, ftClient*>::const_iterator it = this->_users.begin();
	for (; it != this->_users.end(); it++)
		if (newnick == (*it).second->get_name())
			return !serverSend(client.get_fd(), "", "433 " + oldnick + " " + newnick,
							"Nickname is already in use");
	client.set_name(msg.getParam().front());
	if (oldnick.empty())
		oldnick = newnick;
	return serverSend(client.get_fd(), oldnick,  msg.getCommand(), newnick);
}

int		Commands::notice(ftClient& client, Message& msg)
{
	std::string target = msg.getParam().front();
	int			pos = 0;

	if (!client.get_name().size() || msg.getParam().size() < 2)
		return false;
	if (msg.isNickname(target))
	{
		if ((pos = target.find('@')) || (pos = target.find('%')))
			target = target.substr(0, pos - 1);
		std::map<int, ftClient*>::const_iterator it = this->_users.begin();
		for (; it != this->_users.end(); it++)
		{
			if (target == (*it).second->get_name())
			{
				serverSend((*it).second->get_fd(), client.get_prefix(), msg.getCommand() + " " + target, msg.getTrailing());
				return serverSend(client.get_fd(), client.get_prefix(), msg.getCommand() + " " + target, msg.getTrailing());
			}
		}
		return false;
	}
	else if (msg.isChannel(target))
	{
		servChannel::iterator it = _channels.find(target);

		if (it != _channels.end())
		{
			//check if can send to channel
			std::vector<ftClient*> members = (*it).second->getMembers();
			int size = members.size();
			for (int i = 0; i != size; i++)
				serverSend(members[i]->get_fd(), client.get_prefix(), msg.getCommand() + " " + target, msg.getTrailing());
			return true;
		}
		return false;
	}
	//else if (msg.isMask(target))
	return false;
}

int		Commands::oper(ftClient& client, Message& msg)
{
	if (client.get_name().empty())
		return !serverSend(client.get_fd(), "", "", "You are not registered yet.");
	else if (msg.getParam().size() < 2)
		return !serverSend(client.get_fd(), "", "461 " + msg.getCommand(), "Not enough parameters");
	// std::map<std::string, Oper*>::const_iterator it = this->_operList.getOperList().find(client.get_name());
	// if ( it == this->_operList.getOperList().end() || (*it).second->getPass() != msg.getParam()[1])
	// 	return !serverSend(client.get_fd(), "", "464 ", ":Password incorrect");
	// if ((*it).second->getHost() != client.get_addr())
	// 	return !serverSend(client.get_fd(), "", "491 ", ":No O-lines for your host");
	//set usermode
	return true;
}
int		Commands::part(ftClient& client, Message& msg)
{
	std::vector<std::string>					params;
	servChannel::iterator						itchan;

	if (client.get_name().empty())
		return !serverSend(client.get_fd(), "", "", "You are not registered yet.");
	else if (!msg.getParam().size())
		return !serverSend(client.get_fd(), "", "461 " + msg.getCommand(), "Not enough parameters");
	params = msg.getParam();
	for (int i = 0; i != params.size(); i++)
	{
		if ((itchan = _channels.find(params[i])) == _channels.end())
			return !sendCommandResponse(client, ERR_NOSUCHCHANNEL, params[i], "No such channel");
		if ((*itchan).second->isMember(client))
		{
			(*itchan).second->removeMember(client);
			if (!(*itchan).second->getMembers().size())
			{
				delete (*itchan).second;
				_channels.erase(itchan);
			}
			return serverSend(client.get_fd(), client.get_prefix(), "PART " + params[i] + " ", msg.getTrailing());
		}
		return !serverSend(client.get_fd(), client.get_prefix(), "PART " + params[i] + " ", msg.getTrailing());
	}
	return false;
}

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
int		Commands::privmsg(ftClient& client, Message& msg)
{
	std::string target;
	int			pos = 0;

	if (client.get_name().empty())
		return !serverSend(client.get_fd(), "", "", "You are not registered yet.");
	else if (!msg.getParam().size())
		return !serverSend(client.get_fd(), "", "411 ", "No recipient given " + msg.getCommand());
	else if (!msg.getTrailing().size())
		return !serverSend(client.get_fd(), "", "412 ", "No text to send");
	target = msg.getParam().front();
	if (msg.isNickname(target))
	{
		if ((pos = target.find('@')) || (pos = target.find('%')))
			target = target.substr(0, pos - 1);
		std::map<int, ftClient*>::const_iterator it = this->_users.begin();
		for (; it != this->_users.end(); it++)
		{
			if (target == (*it).second->get_name())
			{
				if (UserMode::AWAY & (*it).second->get_flags())
					return !serverSend(client.get_fd(), "", "301 " + client.get_name() + " " + target, (*it).second->get_awaymsg());
				serverSend((*it).second->get_fd(), client.get_prefix(), msg.getCommand() + " " + target, msg.getTrailing());
				return serverSend(client.get_fd(), client.get_prefix(), msg.getCommand() + " " + target, msg.getTrailing());
			}
		}
		return !serverSend(client.get_fd(), "", "401 " + target, "No such nick/channel");
	}
	else if (msg.isChannel(target))
	{
		servChannel::iterator it = _channels.find(target);

		if (it != _channels.end())
		{
			if (((*it).second->getFlags() & ChannelMode::NO_OUTSIDE_MSG && !(*it).second->isMember(client))
				|| (*it).second->getFlags() & ChannelMode::MODERATED && !(*it).second->isChop(client)
				|| (*it).second->isBanned(client))
				return !serverSend(client.get_fd(), "", "404 " + target, "Cannot send to channel");
			//check if can send to channel: moderated, no_outside_msg, and check client flag
			std::vector<ftClient*> members = (*it).second->getMembers();
			int size = members.size();
			for (int i = 0; i != size; i++)
				if (members[i]->get_name() != client.get_name())
					serverSend(members[i]->get_fd(), client.get_prefix(), msg.getCommand() + " " + target, msg.getTrailing());
			return true;
		}
		return !serverSend(client.get_fd(), "", "401 " + target, "No such nick/channel");
	}
	//else if (msg.isMask(target))
	return true;
}

int		Commands::quit(ftClient& client, Message& msg)
{
	servChannel::iterator	itchan;

	if (_channels.size())
	{
		itchan = _channels.begin();
		for (; itchan != _channels.end();)
		{
			Message m((*itchan).second->getName());
			if ((*itchan).second->isMember(client))
			{
				itchan++;
				part(client, m);
			}
		}
	}
	client.set_quit();
	return true;
}

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
int		Commands::topic(ftClient& client, Message& msg)
{
	servChannel::iterator	it;

	if (msg.getParam().empty())
		return !serverSend(client.get_fd(), "", "461 " + msg.getCommand(), "Not enough parameters");
	if (msg.getParam().size() > 1)
		return false;
	it = _channels.find(msg.getParam()[0]);
	if (it == _channels.end())
		return !serverSend(client.get_fd(), "", "403 " + client.get_name() + " " + msg.getParam()[0], "No such channel");//weechat does not react to this response
	else if ((*it).second->getName()[0] == '+')
		return !sendCommandResponse(client, ERR_NOCHANMODES, msg.getParam()[0], "Channel doesn't support modes");
	else if (!msg.getTrailing().size())
	{
		if ((*it).second->getTopic().empty())
			return !sendCommandResponse(client, RPL_NOTOPIC, msg.getParam()[0], "No topic is set");
		return serverSend(client.get_fd(), "", "332 " + client.get_name() + " " + msg.getParam()[0], (*it).second->getTopic());
	}
	else if (!(*it).second->isMember(client))
		return !sendCommandResponse(client, ERR_NOTONCHANNEL, msg.getParam()[0], "You're not on that channel");
	else if ((*it).second->getFlags() & ChannelMode::TOPIC_SETTABLE_BY_CHANOP)
	{
		if (!(*it).second->isChop(client))
			return !sendCommandResponse(client, ERR_CHANOPRIVSNEEDED, msg.getParam()[0], "You're not channel operator");
	}
	(*it).second->setTopic(msg.getTrailing());
	return serverSend(client.get_fd(), client.get_name(), msg.getCommand() + " " + msg.getParam()[0], msg.getTrailing());
}

//USER
int		Commands::user(ftClient& client, Message& msg)
{
		std::string			username;
		std::string			realname = msg.getTrailing();
		std::string			servername = IRCSERVNAME;
		std::string			serverversion = IRCSERVVERSION;
		int					i = 0;

		if (client.isRegistered())
			return !sendCommandResponse(client, ERR_ALREADYREGISTRED, "You may not reregister");
		else if (msg.getParam().size() < 3)
			return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
		username = msg.getParam()[0];
		client.set_names(username, realname);
		client.validate();
		motd(client, msg); //show motd
		serverSend(client.get_fd(), "", "001 " + client.get_name(), "Welcome to the Internet Relay Network " + client.get_name());
		serverSend(client.get_fd(), "", "002 " + client.get_name(), "Your host is " + servername + ", running version " + serverversion);
		return serverSend(client.get_fd(), "", "003 " + client.get_name(), "The server was created on I don't know how long ago...");
}
int		Commands::userhost(ftClient& client, Message& msg) { return 1; }

//VERSION
int		Commands::version(ftClient& client, Message& msg)
{
		std::vector<std::string>	params = msg.getParam();

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

