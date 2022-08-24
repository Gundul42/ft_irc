#include "Commands.hpp"

Commands::Commands()
{
	_userCommands["NICK"] = &Commands::nick;
	_userCommands["NOTICE"] = &Commands::notice;
	_userCommands["OPER"] = &Commands::oper;
	_userCommands["PASS"] = &Commands::pass;
	_userCommands["PING"] = &Commands::ping;
	_userCommands["PRIVMSG"] = &Commands::privmsg;
	_userCommands["QUIT"] = &Commands::quit;
	_userCommands["USER"] = &Commands::user;
	_userCommands["AWAY"] = &Commands::away;
	_userCommands["DIE"] = &Commands::die;
	_userCommands["INVITE"] = &Commands::invite;
	_userCommands["JOIN"] = &Commands::join;
	_userCommands["KICK"] = &Commands::kick;
	_userCommands["LIST"] = &Commands::list;
	_userCommands["MODE"] = &Commands::mode;
	_userCommands["MOTD"] = &Commands::motd;
	_userCommands["NAMES"] = &Commands::names;
	_userCommands["PART"] = &Commands::part;
	_userCommands["TIME"] = &Commands::ustime;
	_userCommands["TOPIC"] = &Commands::topic;
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
	if (!client.isRegistered())
		return !serverSend(client.get_fd(), "", "", "You are not registered yet");
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

int		Commands::die(ftClient& client, Message& msg) 
{ 
		servChannel::iterator	it = _channels.begin();
		std::vector<std::string>::iterator	itop = _operList.begin();

		msg.getPrefix();

		while (itop != _operList.end())
		{
			if ((*itop) == client.get_name())
					break;
			itop++;
		}
		if (itop == _operList.end())
				return false;
		std::cout << client.get_hostname() << " from " << client.get_addr() <<
				" shuts down server." << std::endl;
		while (it != _channels.end())
		{
			delete it->second;
			it++;
		}
		_channels.clear();
		client.set_send();
		return true;
}

int		Commands::invite(ftClient& client, Message& msg)
{
	servChannel::iterator		itchan;
	std::string					nickname;
	std::string					channel;
	ftClient*					target;

	if (!client.isRegistered())
		return !serverSend(client.get_fd(), "", "", "You are not registered yet");
	else if (msg.getParam().size() < 2)
		return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
	nickname = msg.getParam()[0];
	channel = msg.getParam()[1];
	if ((itchan = _channels.find(channel)) == _channels.end())
		return !sendCommandResponse(client, ERR_NOSUCHCHANNEL, channel, "No such channel");
	else if (!(*itchan).second->isMember(client))
		return !sendCommandResponse(client, ERR_NOTONCHANNEL, channel, "You're not on that channel");
	else if (!(*itchan).second->isChop(client))
		return !sendCommandResponse(client, ERR_CHANOPRIVSNEEDED, channel, "You're not channel operator");
	else if (!isUser(nickname))
		return !serverSend(client.get_fd(), "", "401 " + client.get_name() +
		" " + nickname, "No such nick/channel");
	else if ((*itchan).second->getMember(nickname, &target))
		return !serverSend(client.get_fd(), "", "443 " + nickname +
			" " + (*itchan).second->getName(), "is already on channel");
	else //invite does not add to invite list of channel
	{
		for (std::map<int, ftClient*>::iterator it = _users.begin();
			it != _users.end(); it++)
		{
			if ((*it).second->get_name() == nickname)
			{
				(*it).second->invited(channel);
				if ((*it).second->get_flags() & UserMode::AWAY)
					return !serverSend(client.get_fd(), "", "301 " + client.get_name() +
						" " + nickname, (*it).second->get_awaymsg());
				serverSend(client.get_fd(), "", "341 " + client.get_name() + 
					" " + nickname + " " + channel, "");
				serverSend((*it).second->get_fd(), client.get_name(), "INVITE " + nickname, channel);
			}
		}
		return true;
	}
}

int		Commands::join(ftClient& client, Message& msg) 
{
		std::vector<std::string>	params = msg.getChannel();
		servChannel::iterator		itchan;
		// bool						isnew = false;
		IrcChannel*					newChan;
		IrcChannel*					existedChan;

		if (client.get_name().empty() || !client.isRegistered())
			return !serverSend(client.get_fd(), "", "", "You are not registered yet");
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
		for (size_t i = 0; i != params.size(); i++)
		{
			// if ((itchan = _channels.find(params[i])) == _channels.end())
			if (!getChannel(params[i], &existedChan))
			{
				newChan = new IrcChannel(params[i], client);
				if (newChan->valChanName(params[i]) == false)
				{
					delete newChan;
					return !sendCommandResponse(client, ERR_NOSUCHCHANNEL, params[i], "No such channel");
				}
				// isnew = true;
				//moved to here for cases like first channel is new but second channel not will cause join error
				if (params[i][0] == '#')
				{
					newChan->setFlags("+", ChannelMode::parse('n'));
					newChan->setFlags("+", ChannelMode::parse('t'));
				}
				_channels.insert(std::pair<std::string, IrcChannel*>(params[i], newChan));
				serverSend(client.get_fd(), client.get_name(), "JOIN " + params[i], client.get_name());
				Message m(params[i]);
				mode(client, m);
				names(client, m);
			}
			// if (isnew)
			// {
			// 	if (params[i][0] == '#')
			// 	{
			// 		newChan->setFlags("+", ChannelMode::parse('n'));
			// 		newChan->setFlags("+", ChannelMode::parse('t'));
			// 	}
			// 	_channels.insert(std::pair<std::string, IrcChannel*>(params[i], newChan));
			// 	serverSend(client.get_fd(), client.get_name(), "JOIN " + params[i], client.get_name());
			// 	Message m(params[i]);
			// 	mode(client, m);
			// 	names(client, m);
			// }
			else
			{
				// if ((*itchan).second->isBanned(client))
				if (existedChan->isBanned(client))
					return !sendCommandResponse(client, ERR_BANNEDFROMCHAN, existedChan->getName(),
								"Cannot join channel (+b)");
				// else if ((*itchan).second->getLimit() > 0 && (*itchan).second->getLimit()
				// 				<= static_cast<int>((*itchan).second->getMembers().size()))
				// 	return !sendCommandResponse(client, ERR_CHANNELISFULL, params[i],
				// 				"Cannot join channel (+l)");
				else if (existedChan->getLimit() > 0 && existedChan->getLimit()
								<= static_cast<int>(existedChan->getMembers().size()))
					return !sendCommandResponse(client, ERR_CHANNELISFULL, existedChan->getName(),
								"Cannot join channel (+l)");
				// else if ((*itchan).second->getFlags() & ChannelMode::INVITE_ONLY &&
				// 				!(*itchan).second->isInvited(client))
				// 	return !sendCommandResponse(client, ERR_INVITEONLYCHAN, params[i],
				// 				"Cannot join channel (+i)");
				else if (existedChan->getFlags() & ChannelMode::INVITE_ONLY &&
								!existedChan->isInvited(client))
					return !sendCommandResponse(client, ERR_INVITEONLYCHAN, existedChan->getName(),
								"Cannot join channel (+i)");
				// else if (((*itchan).second->getFlags() & ChannelMode::KEY && msg.getKeys().empty())
				// 				|| ((*itchan).second->getFlags() & ChannelMode::KEY && msg.getKeys()[i]
				// 				!= (*itchan).second->getKey()))
				// 	return !sendCommandResponse(client, ERR_BADCHANNELKEY, params[i],
				// 				"Cannot join channel (+k)");
				else if ((existedChan->getFlags() & ChannelMode::KEY && msg.getKeys().empty())
								|| (existedChan->getFlags() & ChannelMode::KEY && msg.getKeys()[i]
								!= existedChan->getKey()))
					return !sendCommandResponse(client, ERR_BADCHANNELKEY, existedChan->getName(),
								"Cannot join channel (+k)");
				// if (!(*itchan).second->isMember(client))
				if (!existedChan->isMember(client))
				{
					std::cout << "channel join: " << existedChan->getName() << "\n";
					// (*itchan).second->addMember(client);
					existedChan->addMember(client);
					// for (size_t j = 0; j != (*itchan).second->getMembers().size(); j++)
					// 	serverSend((*itchan).second->getMembers()[j]->get_fd(), client.get_name(),
					// 			"JOIN " + (*itchan).second->getName(), client.get_name());
					for (size_t j = 0; j != existedChan->getMembers().size(); j++)
						serverSend(existedChan->getMembers()[j]->get_fd(), client.get_name(),
								"JOIN " + existedChan->getName(), client.get_name());
					// if (!(*itchan).second->getTopic().empty())
					if (!existedChan->getTopic().empty())
					{
						Message mt("TOPIC " + existedChan->getName());
						topic(client, mt);
					}
					Message m(existedChan->getName());
					names(client, m);
				}
			}
		}
		return true;
}

int		Commands::kick(ftClient& client, Message& msg)
{
	servChannel::iterator		itchan;
	std::vector<std::string>	params = msg.getChannel();
	ftClient*					target;
	std::string					kick_msg;

	if (!client.isRegistered())
		return !serverSend(client.get_fd(), "", "", "You are not registered yet");
	else if (msg.getParam().size() < 2)
		return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
	for(size_t i = 0; i < msg.getChannel().size(); i++)
	{
		if ((itchan = _channels.find(params[i])) == _channels.end())
			sendCommandResponse(client, ERR_NOSUCHCHANNEL, params[i], "No such channel");
		else if (!(*itchan).second->isMember(client))
			sendCommandResponse(client, ERR_NOTONCHANNEL, params[i], "You're not on that channel");
		else if (!(*itchan).second->isChop(client))
			sendCommandResponse(client, ERR_CHANOPRIVSNEEDED, params[i], "You're not channel operator");
		else
		{
			for (size_t j = 0; j < msg.getKeys().size(); j++)
			{
				if (!isUser(msg.getKeys()[j]))
					serverSend(client.get_fd(), "", "401 " + client.get_name() +
							" " + msg.getKeys()[j], "No such nick/channel");
				else if (!(*itchan).second->getMember(msg.getKeys()[j], &target))
					serverSend(client.get_fd(), "", "441 " + msg.getKeys()[j] +
							" " + (*itchan).second->getName(), "They aren't on that channel");
				else
				{
					Message partmsg("PART " + params[i]);
					kick_msg = msg.getTrailing().empty() ? msg.getKeys()[j] : msg.getTrailing();
					(*itchan).second->removeMember(*target);
					serverSend(client.get_fd(), client.get_name(), "KICK " + params[i] + " " + msg.getKeys()[j], kick_msg);
					serverSend(target->get_fd(), client.get_name(), "KICK " + params[i] + " " + msg.getKeys()[j], kick_msg);
					if (itchan->second->getMembers().size() == 0)
					{
						delete itchan->second;
						_channels.erase(itchan);
					}
				}
			}
		}
	}
	return true;
}

int		Commands::list(ftClient& client, Message& msg)
{
		servChannel::iterator						it = _channels.begin();
		std::vector<std::string>					params = msg.getParam();
		std::ostringstream							oss;
		std::string									chan;
		size_t										pos;

		if (_channels.size() == 0 || params.size() > 2)
			return !sendCommandResponse(client, RPL_LISTEND, "End of /LIST");
		sendCommandResponse(client, RPL_LISTSTART, "Channel", "Users Name");
		if (params.size() == 0)
		{
			while (it != _channels.end())
			{
				oss.str("");
				oss << RPL_LIST << " " << client.get_name() << " " << (*it).first <<
						" " << (*it).second->getMembers().size();
				serverSend(client.get_fd(), "", oss.str(), (*it).second->getTopic());
				it++;
			}
			sendCommandResponse(client, RPL_LISTEND, "End of /LIST");
			return 1;
		}
		if (params.size() == 2 && params[1] != IRCSERVNAME)
		{
			sendCommandResponse(client, ERR_NOSUCHSERVER, params[1], "No such server");
			sendCommandResponse(client, RPL_LISTEND, "End of /LIST");
			return 1;
		}
		chan = params[0];
		do
		{	
				pos = chan.find(',');
				it = _channels.find(chan.substr(0, pos));
				if (it != _channels.end())
				{
					oss.str("");
					oss << RPL_LIST << " " << client.get_name() << " " << (*it).first <<
							" " << (*it).second->getMembers().size();
					serverSend(client.get_fd(), "", oss.str(), (*it).second->getTopic());
				}
				chan = chan.substr(pos + 1, std::string::npos);
		}
		while (pos != std::string::npos);
		return 1;
}

void	Commands::printList(unsigned int incoming_flag, IrcChannel &channel,
								const std::string &flag, ftClient &client)
{
	std::string	reply[] = {"346 ", "348 ", "367 ", "347 ", "349 ", "368 "};
	std::string	trailing[] = {"invite", "exception", "ban"};
	std::vector<std::string> list;
	int	i = 0;

	if (flag == "e")
		i = 1;
	else if (flag == "b")
		i = 2;
	channel.getMasks().getMaskList(incoming_flag, list);
	for (std::vector<std::string>::iterator it = list.begin();
		it != list.end(); it++)
		serverSend(client.get_fd(), "", reply[i] + client.get_name() 
			+ " " + channel.getName() + " " +  (*it) +
			" " + client.get_prefix(), " ");
	serverSend(client.get_fd(), "", reply[i + 3] + client.get_name() + " " +
					channel.getName(),
					"End of Channel " + trailing[i] + " List");
}

int		Commands::mode(ftClient& client, Message& msg)
{
		std::vector<std::string>					params = msg.getParam();
		std::vector<std::string>::const_iterator	itpar;
		// servChannel::iterator					itchan;
		unsigned int								incoming_flag;
		std::string									add_remove = "+";
		std::string									mask;
		ftClient*									target;
		std::vector<ftClient*>::iterator			itmem;
		std::vector<ftClient*>						members;
		IrcChannel*									existedChan;

		if (client.get_name().empty() || !client.isRegistered())
			return !serverSend(client.get_fd(), "", "", "You are not registered yet");
		if (!params.size())
			return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
		if (msg.isChannel(params[0]))
		{
			// itchan =_channels.find(params[0]);
			// if (itchan == _channels.end())
				// return !sendCommandResponse(client, ERR_NOSUCHCHANNEL, params[0], "No such channel");
			if (!getChannel(params[0], &existedChan))
				return !sendCommandResponse(client, ERR_NOSUCHCHANNEL, params[0], "No such channel");
			if (params.size() == 1) //mode #channel -> prints channel info
			{
				serverSend(client.get_fd(),IRCSERVNAME, "324 " +
					client.get_name() + " " + existedChan->getName() + " +" +
					existedChan->toString(), "");
				return serverSend(client.get_fd(),IRCSERVNAME, "329 " + client.get_name() + " " +
										existedChan->getName() + " " + existedChan->getCtime(), "");
			}
			for (size_t i = 0; i != msg.getFlags().size(); i++)
			{
				incoming_flag = ChannelMode::parse(msg.getFlags()[i][0]);
				if (i == 0 && (msg.getFlags()[i] == "-" || msg.getFlags()[i] == "+")) //get +/-, if not provided then +
					add_remove = msg.getFlags()[i];
				else if (incoming_flag == 0 || msg.getFlags()[i] == "a")
					serverSend(client.get_fd(), "", "472 " + client.get_name() + " " +
									msg.getFlags()[i].c_str(), "is an unknown mode char to me");
				else if (params.size() == 2 &&  msg.getFlags()[i] == "b") //print ban list, no chop needed
					printList(incoming_flag, *existedChan, msg.getFlags()[i], client);
				else if (!existedChan->isChop(client)) //check chop
					sendCommandResponse(client, ERR_CHANOPRIVSNEEDED,
									"You're not channel operator");
				else if (params.size() == 2 && (msg.getFlags()[i] == "I" 
					|| msg.getFlags()[i] == "e")) //print invite or exception list
					printList(incoming_flag, *existedChan, msg.getFlags()[i], client);
				else if	(add_remove == "+" && params.size() < 3 
					&& (incoming_flag == ChannelMode::KEY 
					|| incoming_flag == ChannelMode::LIMIT 
					|| incoming_flag == ChannelMode::VOICE 
					|| incoming_flag == ChannelMode::OPERATOR))
						sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
				else if (msg.getFlags()[i].find_first_not_of("ovtimnklbeI") == std::string::npos)
				{
					if (msg.getParam().size() == 3)
						mask = msg.getParam()[2];
					if ((incoming_flag == ChannelMode::VOICE
						|| incoming_flag == ChannelMode::OPERATOR) && !isUser(mask)) //flags requiring user to be valid
							serverSend(client.get_fd(), "", "401 " + client.get_name() +
									" " + mask, "No such nick/channel");
					else if ((incoming_flag == ChannelMode::VOICE
						|| incoming_flag == ChannelMode::OPERATOR) && !existedChan->getMember(mask, &target))
							serverSend(client.get_fd(), "", "441 " + mask +
									" " + existedChan->getName(), "They aren't on that channel");
					else if ((add_remove == "+" && (existedChan->addChop(incoming_flag, *target)
								|| existedChan->addVoice(incoming_flag, *target)
								|| existedChan->setMasks(incoming_flag, mask)))
						|| (add_remove == "-" && (existedChan->removeChop(incoming_flag, *target)
									|| existedChan->removeVoice(incoming_flag, *target)
									|| existedChan->unsetMasks(incoming_flag, mask))))
					{
						existedChan->setFlags(add_remove, incoming_flag);
						members = existedChan->getMembers();
						itmem = members.begin();
						while (itmem != members.end())
						{
							serverSend((*itmem)->get_fd(), client.get_name(), "MODE "
									+ existedChan->getName() + " " + add_remove 
									+ msg.getFlags()[i] + " " + mask, " ");
							itmem++;
						}
					}
					else if (msg.getFlags()[i].find_first_not_of("timnl") == std::string::npos)
					{
						if ((add_remove == "+" && (incoming_flag & existedChan->getFlags()))
							|| (add_remove == "-" && !(incoming_flag & existedChan->getFlags())))
							continue; //if add already exit flag/ remove already not exist flag, send no response
						else
						{
							existedChan->setFlags(add_remove, incoming_flag);
							members = existedChan->getMembers();
							itmem = members.begin();
							while (itmem != members.end())
							{
								serverSend(client.get_fd(), client.get_name(), "MODE "
										+ existedChan->getName() + " " 
										+ add_remove + msg.getFlags()[i], " ");
								itmem++;
							}
						}
					}
				}
			}
		}
		else //mode user is for server operator, not supported
			return !sendCommandResponse(client, ERR_USERSDONTMATCH, 
								"Cannot change mode for other users");
		return true;
}

int		Commands::motd(ftClient& client, Message& msg)
{
		std::fstream		motd;
		std::ostringstream	tosend;
		std::string			str;

		(void)msg;
		motd.open(IRCMOTDFILE, std::ios::in);
		if (!motd.is_open())
			return !sendCommandResponse(client, ERR_NOMOTD, "MOTD File is missing");
		tosend << IRCSERVNAME << " Message of the day";
		sendCommandResponse(client, RPL_MOTDSTART, tosend.str());
		while (getline(motd, str))
			sendCommandResponse(client, RPL_MOTD, str);
		sendCommandResponse(client, RPL_ENDOFMOTD, "End of /MOTD command.");
		motd.close();
		return true;
}

int		Commands::names(ftClient& client, Message& msg)
{
	servChannel::iterator	it;
	std::vector<ftClient*>	members;
	std::string				res_param;
	std::string				res_trailing;

	if (!client.isRegistered())
		return !serverSend(client.get_fd(), "", "", "You are not registered yet");
	res_param.clear();
	res_trailing.clear();
	if (msg.getParam().empty())
	{
		it = _channels.begin();
		for (; it != _channels.end(); it++)
		{
			Message m((*it).second->getName());
			names(client, m);
		}
		return true;
	}
	for (size_t i = 0; i != msg.getParam().size(); i++)
	{
		it = _channels.find(msg.getParam()[i]);
		if (it != _channels.end())
		{
			if ((*it).second->getFlags() & ChannelMode::SECRET)
				res_param += " @ ";
			else if ((*it).second->getFlags() & ChannelMode::PRIVATE)
				res_param += " * ";
			else
				res_param += " = ";
			res_param += (*it).second->getName();
			members = (*it).second->getMembers();
			for (size_t i = 0; i != members.size(); i++)
			{
				if ((*it).second->isChop(*members[i]))
					res_trailing += "@";
				res_trailing += members[i]->get_name() + (i + 1 == members.size() ? "" : " ");
			}
			sendCommandResponse(client, RPL_NAMREPLY, res_param, res_trailing);
		}
		sendCommandResponse(client, RPL_ENDOFNAMES, msg.getParam()[i], "End of /NAMES list.");
	}
	return true;
}

int		Commands::nick(ftClient& client, Message& msg)
{
	std::string oldnick = client.get_name();
	std::string	newnick;

	if (msg.getParam().empty())
			return !serverSend(client.get_fd(), "", "431 " + oldnick + " " + newnick,
							"No nickname given");
	newnick = msg.getParam().front();
	if (newnick.size() > 9 || newnick[0] < 64 || newnick.find(' ') != std::string::npos)
			return !serverSend(client.get_fd(), "", "432 " + oldnick + " " +
							newnick, "Erroneus nickname");
	std::map<int, ftClient*>::const_iterator it = this->_users.begin();
	for (; it != this->_users.end(); it++)
		if (newnick == (*it).second->get_name())
			return !serverSend(client.get_fd(), "", "433 " + oldnick + " " + newnick,
							"Nickname is already in use");
	client.set_name(msg.getParam().front());
	if (oldnick.empty())
		oldnick = newnick;
	serverSend(client.get_fd(), oldnick,  msg.getCommand(), newnick);
	client.validate();
	if (client.isRegistered())
		return sendRegistered(client, msg);
	return true;
}

int		Commands::notice(ftClient& client, Message& msg)
{
	std::string target;
	int			pos = 0;
	IrcChannel*	existedChan;

	if (!client.get_name().size() || msg.getParam().size() != 1)
		return false;
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
				return serverSend((*it).second->get_fd(), client.get_prefix(), msg.getCommand() +
								" " + target, msg.getTrailing());
			}
		}
		return false;
	}
	else if (msg.isChannel(target))
	{
		// servChannel::iterator it = _channels.find(target);

		// if (it != _channels.end())
		if (getChannel(target, &existedChan))
		{
			if ((existedChan->getFlags() & ChannelMode::NO_OUTSIDE_MSG &&
									!existedChan->isMember(client))
				|| (existedChan->getFlags() & ChannelMode::MODERATED &&
						!existedChan->isChop(client) && !existedChan->isVoice(client))
				|| ((!existedChan->isChop(client) && !existedChan->isVoice(client)) &&
								existedChan->isBanned(client)))
				return false;
			std::vector<ftClient*> members = existedChan->getMembers();
			int size = members.size();
			for (int i = 0; i != size; i++)
				serverSend(members[i]->get_fd(), client.get_prefix(), msg.getCommand() +
							" " + existedChan->getName(), msg.getTrailing());
			return true;
		}
		return false;
	}
	//else if (msg.isMask(target)), for furture dev
	return false;
}

int		Commands::oper(ftClient& client, Message& msg)
{
	std::vector<std::string>::iterator it = _operList.begin();

	if (!client.isRegistered())
		return !serverSend(client.get_fd(), "", "", "You are not registered yet");
	else if (msg.getParam().size() < 2)
		return !serverSend(client.get_fd(), "", "461 " + msg.getCommand(), "Not enough parameters");
	else if (client.get_name() != msg.getParam()[0])
		return false;
	for (; it != _operList.end(); it++)
		if((*it) == msg.getParam()[0])
			return false;
	if (msg.getParam()[0] == IRCADMIN && msg.getParam()[1] == IRCADMINPWD)
	{
		_operList.push_back(msg.getParam()[0]);
		return serverSend(client.get_fd(), "", "381 " + client.get_name(),
						"You are now an IRC operator");
	}
	return !serverSend(client.get_fd(), "", "491 " + client.get_name(), "No O-lines for your host");
}

int		Commands::part(ftClient& client, Message& msg)
{
	std::vector<std::string>			params;
	servChannel::iterator				itchan;
	std::vector<ftClient*>				members;
	std::vector<ftClient*>::iterator	itmem;
	std::string							comment;
	std::string							parse;
	std::string							tmp;
	size_t								pos;

	params = msg.getParam();
	if (!client.isRegistered())
		return !sendCommandResponse(client, ERR_NOTREGISTERED, "You have not registered");
	if (params.size() == 0)
		return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, msg.getCommand(),
						"Not enough parameters");
	if (params.size() > 1)
			comment = params[1];
	else
			comment = client.get_name();
	parse = params[0];
	do
	{
		pos = parse.find(',');
		tmp = parse.substr(0, pos);
		itchan = _channels.find(tmp);
		if (itchan == _channels.end())
			sendCommandResponse(client, ERR_NOSUCHCHANNEL, tmp, "No such channel");
		else
		{
			if (!itchan->second->isMember(client))
				sendCommandResponse(client, ERR_NOTONCHANNEL, tmp, "You're not on that channel");
			else
			{
				members = itchan->second->getMembers();
				itmem = members.begin();
				while (itmem != members.end())
				{
					serverSend((*itmem)->get_fd(), client.get_prefix(), "PART " + tmp, comment);
					itmem++;
				}
				itchan->second->removeMember(client);
			}
			if (itchan->second->getMembers().size() == 0)
			{
				delete itchan->second;
				_channels.erase(itchan);
			}
		}
		parse = parse.substr(pos + 1, std::string::npos);
	}
	while (pos != std::string::npos);
	return false;
}

int		Commands::pass(ftClient& client, Message& msg)
{
	if (client.isRegistered())
		return !serverSend(client.get_fd(), "", "462 ", "You may not reregister");
	else if (msg.getParam().empty() && msg.getTrailing().empty())
		return !serverSend(client.get_fd(), "", "461 " + msg.getCommand(), "Not enough parameters");
	else if (msg.getTrailing().size())
		client.setPwd(msg.getTrailing());
	else
		client.setPwd(msg.getParam().front());
	return true;
}

int		Commands::ping(ftClient& client, Message& msg)
{
	std::ostringstream oss;

	oss << "PONG " << IRCSERVNAME;
	if (msg.getParam().empty())
		return !serverSend(client.get_fd(), "", "461 " + msg.getCommand(), "Not enough parameters");
	else
		return serverSend(client.get_fd(), "", oss.str(), msg.getParam().front());
}

int		Commands::privmsg(ftClient& client, Message& msg)
{
	std::string target;
	int			pos = 0;
	IrcChannel*	existedChan;

	if (!client.isRegistered())
		return !serverSend(client.get_fd(), "", "", "You are not registered yet");
	else if (!msg.getParam().size())
		return !serverSend(client.get_fd(), "", "411 ", "No recipient given " + msg.getCommand());
	else if (!msg.getTrailing().size())
		return !serverSend(client.get_fd(), "", "412 ", "No text to send");
	target = msg.getParam().front();
	if (msg.isNickname(target))
	{
		if ((pos = target.find('@')) || (pos = target.find('%')))
			target = target.substr(0, pos);
		std::map<int, ftClient*>::const_iterator it = this->_users.begin();
		for (; it != this->_users.end(); it++)
		{
			if (target == (*it).second->get_name())
			{
				if (UserMode::AWAY & (*it).second->get_flags())
					return !serverSend(client.get_fd(), "", "301 " + client.get_name() +
									" " + target, (*it).second->get_awaymsg());
				return (serverSend((*it).second->get_fd(), client.get_prefix(), msg.getCommand() +
								" " + target, msg.getTrailing()));
			}
		}
		return !serverSend(client.get_fd(), "", "401 " + target, "No such nick/channel");
	}
	else if (msg.isChannel(target))
	{
		// servChannel::iterator it = _channels.find(target);

		// if (it != _channels.end())
		if (getChannel(target, &existedChan))
		{
			if ((existedChan->getFlags() & ChannelMode::NO_OUTSIDE_MSG &&
								!existedChan->isMember(client))
				|| (existedChan->getFlags() & ChannelMode::MODERATED &&
								!existedChan->isChop(client) && !existedChan->isVoice(client))
				|| ((!existedChan->isChop(client) && !existedChan->isVoice(client)) &&
								existedChan->isBanned(client)))
				return !serverSend(client.get_fd(), "", "404 " + existedChan->getName(), "Cannot send to channel");
			std::vector<ftClient*> members = existedChan->getMembers();
			int size = members.size();
			for (int i = 0; i != size; i++)
				if (members[i]->get_name() != client.get_name() && !existedChan->isBanned(*(members[i])))
					serverSend(members[i]->get_fd(), client.get_name(), msg.getCommand() +
									" " + existedChan->getName(), msg.getTrailing());
			return true;
		}
		return !serverSend(client.get_fd(), "", "401 " + target, "No such nick/channel");
	}
	//else if (msg.isMask(target)) oper related, for future dev
	return true;
}

int		Commands::quit(ftClient& client, Message& msg)
{
	servChannel::iterator	itchan;

	(void)msg;
	serverSend(client.get_fd(), client.get_prefix(), "QUIT", "Client Quit");
	serverSend(client.get_fd(), " ", "Error", "Closing Link: " + 
					client.get_addr() + " (Client Quit)");
	itchan = _channels.begin();
	while (itchan != _channels.end())
	{
		if ((*itchan).second->isMember(client))
		{
			(*itchan).second->removeMember(client);
			if ((*itchan).second->getMembers().size() == 0)
			{
				delete itchan->second;
				_channels.erase(itchan);
				itchan = _channels.begin();
				continue;
			}
		}
		itchan++;
	}
	client.set_quit();
	return true;
}

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

int		Commands::topic(ftClient& client, Message& msg)
{
	// servChannel::iterator	it;
	IrcChannel*	existedChan;
	std::string	target_channel;

	if (msg.getParam().empty())
		return !serverSend(client.get_fd(), "", "461 " + msg.getCommand(), "Not enough parameters");
	if (msg.getParam().size() > 1)
		return false;
	// it = _channels.find(msg.getParam()[0]);
	// if (it == _channels.end())
	target_channel = msg.getParam()[0];
	if (!getChannel(target_channel, &existedChan))
		return !serverSend(client.get_fd(), "", "403 " + client.get_name() + " "
						+ target_channel, "No such channel");
	else if (existedChan->getName()[0] == '+')
		return !sendCommandResponse(client, ERR_NOCHANMODES, existedChan->getName(),
						"Channel doesn't support modes");
	else if (!msg.getTrailing().size())
	{
		if (existedChan->getTopic().empty())
			return !sendCommandResponse(client, RPL_NOTOPIC, existedChan->getName(), "No topic is set");
		return serverSend(client.get_fd(), "", "332 " + client.get_name() + " " +
						existedChan->getName(), existedChan->getTopic());
	}
	else if (!existedChan->isMember(client))
		return !sendCommandResponse(client, ERR_NOTONCHANNEL, existedChan->getName(),
						"You're not on that channel");
	else if (existedChan->getFlags() & ChannelMode::TOPIC_SETTABLE_BY_CHANOP)
	{
		if (!existedChan->isChop(client))
			return !sendCommandResponse(client, ERR_CHANOPRIVSNEEDED, existedChan->getName(),
							"You're not channel operator");
	}
	existedChan->setTopic(msg.getTrailing());
	return serverSend(client.get_fd(), client.get_name(), msg.getCommand() + " " +
					existedChan->getName(), msg.getTrailing());
}

int		Commands::user(ftClient& client, Message& msg)
{
		std::string			username;
		std::string			realname = msg.getTrailing();
		std::string			servername = IRCSERVNAME;
		std::string			serverversion = IRCSERVVERSION;
		std::string			serverdate = IRCSERVCDATE;

		if (client.isRegistered())
			return !sendCommandResponse(client, ERR_ALREADYREGISTRED, "You may not reregister");
		else if (msg.getParam().size() < 3)
			return !sendCommandResponse(client, ERR_NEEDMOREPARAMS, "Not enough parameters");
		username = msg.getParam()[0];
		client.set_names(username, realname);
		client.validate();
		if (client.isRegistered())
			return sendRegistered(client, msg);
		return false;
}

int		Commands::version(ftClient& client, Message& msg)
{
		std::vector<std::string>	params = msg.getParam();

		if (!params.empty() && params[0] != IRCSERVNAME)
			return !sendCommandResponse(client, ERR_NOSUCHSERVER, params[0], "No such server");
		return !sendCommandResponse(client, RPL_VERSION, IRCSERVVERSION, "");
}

int		Commands::cap(ftClient& client, Message& msg)
{
	(void)client;
	(void)msg;
	return true;
}

bool	Commands::sendCommandResponse(const ftClient & clt, const int & code, 
				const std::string & trailer) const
{
	std::ostringstream	tosend;

	tosend.clear();
	tosend << code;
	return sendCommandResponse(clt, tosend.str(), trailer);
}

bool	Commands::sendCommandResponse(const ftClient & clt, const std::string & code, 
				const std::string & trailer) const
{
	std::ostringstream	tosend;
	std::string			go;

	tosend.clear();
	go.clear();
	tosend << ":" << IRCSERVNAME << " " << code << " " << clt.get_name() << " :";
	tosend << trailer << "\x0d\x0a";
	go = tosend.str();
	if (send(clt.get_fd(), go.c_str(), go.length(), 0) == -1)
		perror("sendCommandResponse");
	usleep(100); 
	return true;
}

bool	Commands::sendCommandResponse(const ftClient & clt, const int & code, 
				const std::string & argument, const std::string & trailer) const
{
	std::ostringstream	tosend;
	std::string			go;

	tosend.clear();
	go.clear();
	tosend << ":" << IRCSERVNAME << " " << code << " " << clt.get_name() << " " << argument << " :";
	tosend << trailer << "\x0d\x0a";
	go = tosend.str();
	if (send(clt.get_fd(), go.c_str(), go.length(), 0) == -1)
		perror("sendCommandResponse");
	usleep(100); 

	return true;
}

bool	Commands::serverSend(int fd, std::string prefix, std::string msg, std::string trl)
{
		std::string	tosend;

		if (prefix.length() == 0)
				prefix = IRCSERVNAME;
		if (trl.length() == 0)
				trl = IRCSERVNAME;

		tosend.clear();
		tosend = ":" + prefix + " " + msg + " :" + trl + "\x0d\x0a";
		if (send(fd, tosend.c_str(), tosend.length(), 0) == -1)
				perror("serverSend");
		usleep(100); // break of 0.1s to avoid of omitting this msg in case of a following close()
		return true;
}

bool	Commands::isUser(const std::string& nick)
{
	userMap::iterator it = _users.begin();
	for (; it != _users.end(); it++)
	{
		if ((*it).second->get_name() == nick)
			return true;
	}
	return false;
}

bool	Commands::sendRegistered(ftClient &client, Message &msg)
{
	std::string			servername = IRCSERVNAME;
	std::string			serverversion = IRCSERVVERSION;

	if (!motd(client, msg)) //show motd
		return false;
	serverSend(client.get_fd(), "", "001 " + client.get_name(),
					"Welcome to the Internet Relay Network " + client.get_name());
	serverSend(client.get_fd(), "", "002 " + client.get_name(), "Your host is "
					+ servername + ", running version " + serverversion);
	serverSend(client.get_fd(), "", "003 " + client.get_name(),
					"The server was created on I don't know how long ago...");
	return serverSend(client.get_fd(), "", "004 " + client.get_name() + " " + IRCSERVNAME + " " + IRCSERVVERSION + " " + IRCSERVUSERMODES + " " + IRCSERVCHANMODES,
					" ");
}

bool	Commands::getChannel(const std::string& name, IrcChannel** channel)
{
	std::string	search_name;
	std::string target_name;
	servChannel::iterator it = _channels.begin();

	for (size_t i = 0; i < name.size(); i++)
		target_name += tolower(name[i]);
	std::cout << "target: "<< target_name << "\n";

	for (; it != _channels.end(); it++)
	{
		for (size_t i = 0; i < it->second->getName().size(); i++)
			search_name += tolower(it->second->getName()[i]);
		std::cout << "search: " << search_name << "\n";
		if (search_name == target_name)
		{
			*channel = &(*(it->second));
			std::cout << "true\n";
			return true;
		}
		search_name = "";
	}
	std::cout << "false\n";
	return false;
}
