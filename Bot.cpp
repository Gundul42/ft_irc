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


Bot::Bot(std::string address, std::string port, std::string password): _port(port), _addr(address), 
		_passwd(password)
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
				
int const & Bot::getFd(void) const {return this->_bfd;}

std::string	const & Bot::getPort(void) const {return this->_port;}

std::string const & Bot::getAddr(void) const {return this->_addr;}

std::string const & Bot::getPwd(void) const {return this->_passwd;}
