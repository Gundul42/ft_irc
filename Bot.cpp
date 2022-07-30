#include "Bot.hpp"

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

		this->_bfd = socket(AF_INET, SOCK_STREAM, 0);
		if (this->_bfd < 0)
		{
				std::cerr << "Error: building socket failed" << std::endl;
				exit(EXIT_FAILURE);
		}
		if (getaddrinfo((this->_addr).c_str(), (this->_port).c_str(), NULL, &result) != 0)
		{
				std::cerr << "Error in getaddrinfo" << std::endl;
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
				exit(EXIT_FAILURE);
		}
		std::cerr << "Connected to " << this->_addr << ":" << this->_port << std::endl;
		freeaddrinfo(result);
}

Bot::~Bot(void)
{
		close(this->_bfd);
}
				
bool Bot::login(void) const
{
	std::ostringstream	oss;
	std::string			ret;

	oss << "PASS " << this->_passwd;
	this->write(oss.str());
	sleep(1);
	this->write("NICK MrBot");
	ret = this->read();
	if (ret.find("NICK") == std::string::npos)
			return false;
	oss.str("");
	oss << "USER " << FT_BOTNAME << " 0 * :" << FT_BOTNAME;
	this->write(oss.str());
	ret = this->read();
	if (ret.find("375") == std::string::npos)
			return false;
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

std::string	Bot::read(void) const
{
		char	buf[FT_BUFSIZE];

		memset(buf, 0, FT_BUFSIZE);
		if (recv(this->_bfd, buf, FT_BUFSIZE, 0) < 0)
				std::cerr << "Error while receiving" << std::endl;
		return (buf);
}

bool Bot::loop(void)
{
		std::string msg;

		while (msg != "QUIT")
		{
				msg = this->read();
				if (msg.length() > 0)
				{
						std::cout << msg;
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
		std::ostringstream oss;

		oss << "PRIVMSG " << this->getName(msg) << " :" << "---> " << this->getInfo(msg);
		this->write(oss.str());
}
