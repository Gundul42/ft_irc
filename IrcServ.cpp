
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
	int										fd_count=0;
	int										fd_size=5;
	int										poll_count = 0;
	pollfd									*pfds = (pollfd*) malloc(sizeof (*pfds) * fd_size);
	std::string								addrStr;

	pfds[0].fd=_socketfd;
	pfds[0].events = POLLIN;
	fd_count=1;
	addrlen = sizeof remoteaddr;
	std::ostringstream oss;

	while(true)
	{
		poll_count = poll(pfds, fd_count, -1);

		if (poll_count == -1)
		{
			_logAction("Error while poll: exiting");
			exit(1);
		}
		//check_valid_client(pfds, &fd_count);
		oss << "Connected hosts: " << _connections.size();
		_logAction(oss.str());
		oss.str("");
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
									new ftClient(newfd, "", addrStr)));
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
						oss << client->get_addr() << " has left the server. ";
						oss << "Connection time: ";
						oss << client->getTimeConnected();
						oss << " sec" << std::endl;
						_logAction(oss.str());
						oss.str("");
						_del_from_pfds(pfds, i, &fd_count);
					}
					else
					{
						oss << "Last action before " << updateTimeDiff(*(_connections.find(
							pfds[i].fd)->second)) << " seconds ";
						oss << "fd#" << pfds[i].fd << " - " << nbytes << ": " << buf;
						_logAction(oss.str());
						oss.str("");
						//we got something in -> parse command
						this->_commands.handle_command(*client, buf);
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
						serverSend(it->first, "", "Auth failed. Connection timed out", "");
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
				
void IrcServ::serverSend(int fd, std::string prefix, std::string msg, std::string trl)
{
		std::string	tosend;

		if (prefix.length() == 0)
				prefix = IRCSERVNAME;
		if (trl.length() == 0)
				trl = IRCSERVNAME;

		tosend = ":" + prefix + " " + msg + " :" + trl + "\n";
		if (send(fd, tosend.c_str(), tosend.length(), 0) == -1)
				perror("serverSend");
		usleep(100); // break of 0.1s to avoid of omitting this msg in case of a following close()
}

