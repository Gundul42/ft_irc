
#include "IrcServ.hpp"

std::string	IrcServ::_rdns(std::string addr)
{
	struct sockaddr_in sa;
	int res = inet_pton(AF_INET, addr.c_str(), &sa.sin_addr);
	if (res == -1)
		return ("Illegal IPv4, please check your debug");
	else if (res == 0)
	{
		if (inet_pton(AF_INET6, addr.c_str(), &sa.sin_addr) != 1)
			return ("Illegal IPv6, please check your debug");
		return (addr);
	}
	sa.sin_family = AF_INET;
	char node[NI_MAXHOST];
	memset(node, 0, NI_MAXHOST);
	res = getnameinfo((struct sockaddr *)&sa, sizeof sa, node, sizeof node, NULL, 0, 0);
	if (res)
        return (gai_strerror(res));
	return (node);
}

//these are private, no use of copying
IrcServ::IrcServ(const IrcServ & cpy) {*this = cpy;}
IrcServ & IrcServ::operator=(const IrcServ & rgt) 
{
		_socketfd = rgt._socketfd; 
		return *this;
}

IrcServ::IrcServ(const char *port)
{
	int yes=1;
	int rv;
	std::ostringstream oss;

	addrinfo hints, *ai, *p;

	// Get us a socket and bind and listen to it
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) 
	{
    	oss << "selectserver: " << gai_strerror(rv);
		_logAction(oss.str());
	    exit(1);
    }
	for(p = ai; p != NULL; p = p->ai_next)
	{
		_socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    	if (_socketfd < 0) 
		    continue;
		setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(_socketfd, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(_socketfd);
			continue;
		}
		break;
	}
	if (p == NULL)
	{
		_logAction("Error: socket could not bind: exiting");
		exit(1);
	}
	freeaddrinfo(ai);
	if (listen(_socketfd, 10) == -1)
	{
		_logAction("Error: socket could not listen: exiting");
		exit(1);
	}
	oss << "Success: Socket opened and listening at FD#" << _socketfd;
	_logAction(oss.str());
}

IrcServ::~IrcServ(void)
{
		close(_socketfd);
		_logAction("Server closing: Byebye");
}

int	IrcServ::getSocketFd(void) const
{
		return (_socketfd);
}

void IrcServ::_logAction(const std::string & log) const
{
		std::cout << "[" << this->_printTime() << "] " << log << std::endl;
}

//
// dynamically change size of the pollfd array
//
void IrcServ::_add_to_pfds( pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
	if(*fd_count==*fd_size)
	{
		*fd_size*=2;
		*pfds=(pollfd*)realloc(*pfds,sizeof(*pfds)*(*fd_size));
	}
	(*pfds)[*fd_count].fd=newfd;
	(*pfds)[*fd_count].events=POLLIN;
	(*fd_count)++;
}

//
// delete one entry, means copy the last over it and reduce size by one
//
void IrcServ::_del_from_pfds(pollfd pfds[], int i, int *fd_count)
{
	delete _connections.find(pfds[i].fd)->second;
	_connections.erase(pfds[i].fd);
	close (pfds[i].fd);
	pfds[i] = pfds[*fd_count - 1];
	(*fd_count)--;
}

//
// check for IPv4 or IPv6 and return address string
//
void* IrcServ::_get_in_addr(sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in *)sa)->sin_addr);
    }

    return &(((sockaddr_in6 *)sa)->sin6_addr);
}

//
// the loop, wait who comes and goes and send
// string to parser
//
void IrcServ::loop(void)
{
	int										newfd, i = 0;
	sockaddr_storage						remoteaddr;
	socklen_t								addrlen;
	char									buf[512];
	char									remoteIP[INET6_ADDRSTRLEN];
	int										fd_count = 0;
	int										fd_size = 5;
	int										poll_count = 0;
	pollfd									*pfds = (pollfd*) calloc(fd_size, sizeof (*pfds));
	std::string								addrStr;
	bool									block;
	bool									theEnd = false;

	pfds[0].fd = _socketfd;
	pfds[0].events = POLLIN;
	fd_count = 1;
	addrlen = sizeof remoteaddr;
	std::ostringstream oss;

	while(!theEnd)
	{
		oss.str("");
		poll_count = poll(pfds, fd_count, -1);

		if (poll_count == -1)
		{
			_logAction("Error while poll: exiting");
			exit(1);
		}
		//check_valid_client(pfds, &fd_count); // temp deactivated, 20 sec no valid -> kick client
		for (i = 0; i < fd_count; i++)
		{
			ftClient *client = _connections.find(pfds[i].fd)->second; //local copy for less find calls
			if (pfds[i].fd != _socketfd) //reset msgs counter after time has passed
			{
				if (getTimeDiff(*client) > IRCFLOODCONTROL)
						client->set_msgsZero();
			}
			if (pfds[i].revents & POLLIN)
			{
				if (pfds[i].fd == _socketfd) //server
				{
					newfd = accept(_socketfd, (sockaddr*)&remoteaddr, &addrlen);
					if (newfd == -1)
						_logAction("Error while accept");
					else
					{
						_add_to_pfds(&pfds, newfd, &fd_count, &fd_size);
						addrStr = inet_ntop(remoteaddr.ss_family, _get_in_addr(
									(sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
						oss << "New connection from " << addrStr << " with FD#" << newfd;
						_logAction(oss.str());
						oss.str("");
						_connections.insert(std::pair<int, ftClient*>(newfd, 
									new ftClient(newfd, "", addrStr, "")));
					}
				}
				else //clients
				{
					memset(buf, 0, sizeof(buf));
					int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
					if (nbytes <= 0)
					{
						//Got error or connection closed by client
						if (nbytes < 0)
							_logAction("Error while receiving");
						oss << client->get_name() << ":" << client->get_addr();
						oss << " has left the server. " << "Connection time: ";
						oss << client->getTimeConnected();
						oss << " sec" << std::endl;
						_logAction(oss.str());
						oss.str("");
						_del_from_pfds(pfds, i, &fd_count);
					}
					else
					{
						block = false;
						//we got something in -> parse command
						//IRC adds CR,LF to each end of a buffer line, remove it first !
						//std::cout << "Last Char with value: " <<
						//		static_cast<int>(buf[strlen(buf) - 1]) << std::endl;
						if (buf[strlen(buf) - 2] == '\x0d' && buf[strlen(buf) - 1] == '\x0a')
							memset(buf + strlen(buf) - 2, 0, 2);
						else if (buf[strlen(buf) - 1] != '\x0a')
						{
								oss << client->tmpBuffer << buf;
								client->tmpBuffer = oss.str();
								oss.str("");
								continue;
						}
						oss << client->tmpBuffer << buf;
						client->tmpBuffer = oss.str();
						oss.str("");
						if (client->get_msgs() > IRCMAXMSGCOUNT)
								block = true;
						if (!block)
						{
							// std::cout << "connection:\n";
							// std::map<int, ftClient*>::iterator it = _connections.begin();
							// for (; it != _connections.end(); it++)
							// 	std::cout << (*it).second->get_name() << "\n";
							// std::cout << "****\n";
							client->add_msgsCount(1);
							oss << "Last action " << updateTimeDiff(*client) << 
									" seconds ago " << client->get_msgs() << " fd#" << pfds[i].fd <<
									" - " << client->tmpBuffer.size() << " Bytes" << std::endl <<
									">>> " << client->tmpBuffer;
							_logAction(oss.str());
							oss.str("");
							this->_commands.handle_command(_connections, pfds[i].fd,
											client->tmpBuffer);
							client->tmpBuffer.clear();
							if (client->get_send())
							{
									theEnd = true;
									break;
							}
						}
					}
				}
			}
		}
	}
}
				
int	IrcServ::getTimeDiff(ftClient & start)
{
		time_t	end;

		time(&end);
		return (static_cast<int> (difftime(end, start.getLastAction())));
}

int		IrcServ::updateTimeDiff(ftClient & start)
{
		time_t	end;
		int		rst;

		time(&end);
		rst = (static_cast<int> (difftime(end, start.getLastAction())));
		start.setLastAction(end);
		return (rst);
}

std::string	IrcServ::_printTime(void) const
{
   time_t now = time(0);
   std::string	mytime(ctime(&now));
   
   return (mytime.substr(0, mytime.size() - 1));
}

void IrcServ::check_valid_client(pollfd *pfds,int *fd_count)
{
		std::map<int, ftClient*>::iterator	it = this->_connections.begin();
		std::ostringstream					oss;
		int									distance;

		while (it != this->_connections.end())
		{
			if ((it->second)->isRegistered() == false)
			{
					if (it->second->getTimeConnected() > FT_IRC_TIMEOUT)
					{
						oss << "Timeout of FD#" << it->first;
						_logAction(oss.str());
						_commands.serverSend(it->first, "", "Auth failed. Connection timed out", "");
						distance = 1 + std::distance(this->_connections.begin(), it);
						it++;
						_del_from_pfds(pfds, distance, fd_count);
						continue;
					}
			}
			it++;
		}
}

void IrcServ::_debugBuffer(const char *buf) const
{
		int		i = 0;

		std::cout << "BUFFER dumped:" << std::endl;
		while (buf[i])
		{
				std::cout << static_cast<int>(buf[i]) << ", ";
				i++;
		}
		std::cout << std::endl;
}
