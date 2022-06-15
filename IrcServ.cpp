
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

	addrinfo hints, *ai, *p;

	// Get us a socket and bind and listen to it
	memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, port, &hints, &ai)) != 0) 
	{
    	std::cerr << "selectserver: " << gai_strerror(rv) << std::endl;
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
		std::cerr << "Error: socket could not bind" << std::endl;
        exit(1);
	}
	freeaddrinfo(ai);
	if (listen(_socketfd, 10) == -1)
	{
		std::cerr << "Error: socket could not listen" << std::endl;
        exit(1);
	}
	std::cout << "Success: Socket opened and listening at FD#" <<_socketfd << std::endl;
}

IrcServ::~IrcServ(void)
{
		close(_socketfd);
		std::cout << "Byebye" << std::endl;
}

int	IrcServ::getSocketFd(void) const
{
		return (_socketfd);
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
	pfds[i] = pfds[*fd_count-1];
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
	std::map<int, ftClient*>				connections;
	std::map<int, ftClient*>::iterator		iter;

	pfds[0].fd=_socketfd;
	pfds[0].events = POLLIN;
	fd_count=1;
	addrlen = sizeof remoteaddr;

	while(true)
	{
		poll_count = poll(pfds, fd_count, -1);

		if (poll_count == -1)
		{
			std::cerr << "Error while poll" << std::endl;
			exit(1);
		}
		std::cout << "Connected hosts: " << connections.size() << std::endl;
		for (int i = 0; i < fd_count; i++)
		{
			if (pfds[i].revents & POLLIN)
			{
				if (pfds[i].fd == _socketfd) //server
				{
					newfd = accept(_socketfd, (sockaddr*)&remoteaddr, &addrlen);
					if (newfd == -1)
						std::cerr << "Error while accept" << std::endl;
					else
					{
						std::cout << _printTime() << ":";
						_add_to_pfds(&pfds, newfd, &fd_count, &fd_size);
						addrStr = inet_ntop(remoteaddr.ss_family, _get_in_addr(
									(sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN);
						//inet_ntop() not allowed?
						connections.insert(std::pair<int, ftClient*>(newfd, 
									new ftClient(newfd, "", addrStr)));
					}
				}
				else //clients
				{
					int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
					if (nbytes <= 0)
					{
						//Got error or connection closed by client
						if (nbytes < 0)
							std::cerr << "Error while receiving" << std::endl;
						std::cout << _printTime() << ":";
						delete connections.find(pfds[i].fd)->second;
						connections.erase(pfds[i].fd);
						close (pfds[i].fd);
						_del_from_pfds(pfds, i, &fd_count);
					}
					else
					{
						std::cout << "Last action before " << updateTimeDiff(*(connections.find(
							pfds[i].fd)->second)) << " seconds ";
						std::cout << "fd#" << pfds[i].fd << " - " << nbytes << ": " << buf;
						this->_commands.handle_command(*(connections.find(pfds[i].fd)->second), buf);
						//we have received something
						// client fd is pfds[i].fd
						// string is buf
						// nbytes is size of string
						// here is where the string should enter parsing.
						memset(buf, 0, sizeof(buf));
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

std::string	IrcServ::_printTime(void)
{
   time_t now = time(0);
   std::string	mytime(ctime(&now));
   
   return (mytime.substr(0, mytime.size() - 1));
}

