
#include "IrcServ.hpp"

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
	int										newfd;
	sockaddr_storage						remoteaddr;
	socklen_t								addrlen;
	char									buf[512];
	char									remoteIP[INET6_ADDRSTRLEN];
	int										fd_count = 0;
	int										fd_size = 5;
	int										poll_count = 0;
	pollfd									*pfds = (pollfd*) malloc(sizeof (*pfds) * fd_size);
	std::string								addrStr;
	std::string								emptynick;//emptynick.clear() is better than ""
	bool									block;

	pfds[0].fd = _socketfd;
	pfds[0].events = POLLIN;
	fd_count = 1;
	addrlen = sizeof remoteaddr;
	emptynick.clear();
	std::ostringstream oss;

	while(true)
	{
		oss.str("");
		poll_count = poll(pfds, fd_count, -1);

		if (poll_count == -1)
		{
			_logAction("Error while poll: exiting");
			exit(1);
		}
		//check_valid_client(pfds, &fd_count); // temp deactivated, 20 sec no valid -> kick client
		for (int i = 0; i < fd_count; i++)
		{
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
									new ftClient(newfd, emptynick, addrStr)));
					}
				}
				else //clients
				{
					memset(buf, 0, sizeof(buf));
					int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
					ftClient *client = _connections.find(pfds[i].fd)->second;
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
						memset(buf + strlen(buf) - 2, 0, 2);
						if (getTimeDiff(*(_connections.find(pfds[i].fd)->second)) < IRCFLOODCONTROL)
								block = true;
						if (!block)
						{
							oss << "Last action " << updateTimeDiff(*(_connections.find(
								pfds[i].fd)->second)) << " seconds ago ";
							oss << "fd#" << pfds[i].fd << " - " << nbytes << ": " << buf;
							_logAction(oss.str());
							oss.str("");
							this->_commands.handle_command(_connections, pfds[i].fd, buf);
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

bool IrcServ::NickExists(const std::string & nick) const
{
		std::map<int, ftClient*>::const_iterator	it = this->_connections.begin();

		while (it != this->_connections.end())
		{
			if (nick == it->second->get_name())
					return true;
			it++;
		}
		return false;
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
