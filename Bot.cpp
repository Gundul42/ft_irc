#include "Bot.hpp"

void	Bot::myHandler(int signum)
{
		std::cerr << std::endl << "Caught <CTRL+C>, bot is terminating ..." << std::endl;
		exit (signum);
}

Bot::Bot(Bot const & cpy) {*this = cpy;}

Bot Bot::operator=(Bot const & cpy)
{
		this->_bfd = cpy._bfd;
		this->_port = cpy._port;
		this->_addr = cpy._addr;
		this->_passwd = cpy._passwd;
		return (*this);
}

Bot::Bot(std::string const & address, std::string const & port, std::string const & password): 
		_port(port), _addr(address), _passwd(password)
{
		struct addrinfo *result, *res;

		this->setTime();
		this->_bfd = socket(AF_INET, SOCK_STREAM, 0);
		if (this->_bfd < 0)
		{
				std::cerr << "Error: building socket failed" << std::endl;
				freeaddrinfo(result);
				exit(EXIT_FAILURE);
		}
		if (getaddrinfo((this->_addr).c_str(), (this->_port).c_str(), NULL, &result) != 0)
		{
				std::cerr << "Error in getaddrinfo" << std::endl;
				freeaddrinfo(result);
				exit(EXIT_FAILURE);
		}
		res = result;
		while (res != NULL)
		{
			if (connect(this->_bfd, res->ai_addr, res->ai_addrlen) == 0) 
				break;	
			res = res->ai_next;
		}
		std::cerr << std::endl;
		if (res == NULL)
		{
				std::cerr << "Error: Could not connect to " << this->_addr << ":" << 
						this->_port << std::endl;
				freeaddrinfo(result);
				exit(EXIT_FAILURE);
		}
		std::cerr << "Connected to " << this->_addr << ":" << this->_port << std::endl;
		freeaddrinfo(result);
}

Bot::~Bot(void)
{
		close(this->_bfd);
}
				
bool Bot::login(void)
{
	std::ostringstream	oss;
	std::string			ret;

	oss << "PASS " << this->_passwd;
	this->write(oss.str());
	sleep(1);
	this->write("NICK MrBot");
	ret = this->read(true);
	if (ret.find("NICK") == std::string::npos)
			return false;
	oss.str("");
	oss << "USER " << FT_BOTNAME << " 0 * :" << FT_BOTNAME;
	this->write(oss.str());
	ret = this->read(true);
	if (ret.find("375") == std::string::npos)
			return false;
	this->setTime();
	return true;
}

void Bot::write(std::string const & msg) const
{
		std::ostringstream	oss;
		std::string			out;

		oss << msg << "\x0d\x0a";
		out = oss.str();
		send(this->_bfd, out.c_str(), out.length(), 0);
}

std::string	Bot::read(bool block) const
{
		char	buf[FT_BOTBUFSIZE];
		int		flag;
		
		if (block)
				flag = 0;
		else
				flag = MSG_DONTWAIT;
		memset(buf, 0, FT_BOTBUFSIZE);
		if (recv(this->_bfd, buf, FT_BOTBUFSIZE, flag) < 0)
		{
				if (block)
					std::cerr << "Error while receiving" << std::endl;
		}
		return (buf);
}

bool Bot::loop(void)
{
		std::string msg;

		signal(SIGINT, &Bot::myHandler);
		while (this->keepAlive())
		{
				sleep(1);
				msg.clear();
				msg = this->read(false);
				if (msg.length() > 0)
				{
						std::cout << this->printTime() << " : " << msg;
						if (msg.find("PONG") != std::string::npos)
						{
								this->setTime();
								continue;
						}
						if (msg.find("PRIVMSG") == std::string::npos)
								continue;
						this->answer(msg);
				}
		}
		return true;
}

std::string Bot::getName(std::string const & msg) const
{
		size_t			pos;

		pos = msg.find("!");
		return (msg.substr(1, pos - 1));
}

std::string	Bot::getInfo(std::string const & msg) const
{
		size_t		pos;

		pos = msg.find_first_of(":", 1);
		if (pos == std::string::npos)
				return ("");
		pos++;
		return (msg.substr(pos, msg.length() - pos));
}


void Bot::answer(std::string const & msg) const
{
		std::ostringstream	oss;
		std::string 		ret;
		std::string 		chk = this->getInfo(msg);

		if (this->getName(msg) == FT_BOTNAME)
				return ;
		if (chk.length() < 2 || chk[0] != '#' )
			ret = ("Syntax Error: use # in front to de- and encrypt ");
		else if (chk[0] == '#')
			ret = this->encrypt(chk.substr(1,std::string::npos));
		oss << "PRIVMSG " << this->getName(msg) << " :" << ret;
		this->write(oss.str());
}

std::string	Bot::encrypt(std::string const & msg) const
{
		std::string					ret;
		std::string::const_iterator	it = msg.begin();

		while (it != msg.end())
		{
				if (*it >= 'a' && *it <= 'm')
						ret.push_back(*it + 13);
				else if (*it >= 'n' && *it <= 'z')
						ret.push_back(*it - 13);
				else if (*it >= 'A' && *it <= 'M')
						ret.push_back(*it + 13);
				else if (*it >= 'N' && *it <= 'Z')
						ret.push_back(*it - 13);
				else
						ret.push_back(*it);
				it++;
		}
		return ret;
}

std::string	Bot::printTime(void) const
{
   time_t now = time(0);
   std::string	mytime(ctime(&now));
   
   return (mytime.substr(0, mytime.size() - 1));
}

int Bot::getTime(void) const
{
		time_t	end;

		time(&end);
		return (static_cast<int> (difftime(end, this->_timer)));
}

void Bot::setTime(void)
{
		time(&(this->_timer));
}
				
bool Bot::keepAlive(void)
{
		std::ostringstream	oss;
		static int			response = 0;

		if (this->getTime() < FT_BOTKEEPALIVE)
				response = 0;
		else if (this->getTime() >= FT_BOTKEEPALIVE && !response)
		{
				oss << "PING " << _addr;
				this->write(oss.str());
				response = 1;
		}
		else if (this->getTime() >= (FT_BOTKEEPALIVE * 2))
				return false;
		return true;
}
