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
		this->_bfd = socket(AF_INET, SOCK_STREAM, 0);
}

Bot::~Bot(void)
{
		close(this->_bfd);
}
				
int const & Bot::getFd(void) const {return this->_bfd;}

std::string	const & Bot::getPort(void) const {return this->_port;}

std::string const & Bot::getAddr(void) const {return this->_addr;}

std::string const & Bot::getPwd(void) const {return this->_passwd;}
