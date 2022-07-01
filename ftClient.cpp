/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftClient.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwen <mwen@student.42wolfsburg.de>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:27:32 by graja             #+#    #+#             */
/*   Updated: 2022/07/01 21:43:16 by mwen             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftClient.hpp"
#include <unistd.h>

UserMode::UserMode(unsigned flags) : _flags(flags) {}

UserMode::~UserMode() {}

UserMode::Flag UserMode::parse(char c)
{
	unsigned f;
	f = std::islower(c) ? _lowerFlagTable[c - 'a'] : 0;
	return (static_cast<Flag>(f));
}

const unsigned short UserMode::_lowerFlagTable[] =
{
	AWAY, 0, 0, 0, 0, 0, 0, 0, INVISIBLE, 0, 0, 0, 0, 0, OPERATOR, 0, 0,
	RESTRICTED, MARK, 0, 0, 0, WALLOPS, 0, 0, 0
};

//
// it makes no sense to copy clients, FDs are unique
//
ftClient::ftClient(const ftClient & cpy) {*this = cpy;}

ftClient & ftClient::operator=(const ftClient & rgt)
{
		_fd = rgt._fd;
		_name = rgt._name;
		_addr = rgt._addr;
		_val = rgt._val;
		_connect = rgt._connect;
		_lastAction = rgt._lastAction;
		_msgs = rgt._msgs;
		_realname = rgt._realname;
		_username = rgt._username;
		_server = rgt._server;
		_hostname = rgt._hostname;
		return (*this);
}

//
// Constructor needs a FD, a string for the name, a string for it's address
// val will be false until server checks validity of the client
//
ftClient::ftClient(int fd, std::string name, const std::string addr, const std::string host): _fd(fd), _name(name), _addr(addr), _val(false), _hostname(host)
{
		_msgs = 0;
		time(&_connect);
		time(&_lastAction);
}

//
// Destructor closing the FD
//
ftClient::~ftClient(void)
{
		close(_fd);
}

//
// Call once after the client has been validated after login
//
void ftClient::validate(void) {_val = true;}

//
// get Hostname (rDNS) of connected client
//
std::string ftClient::get_hostname(void) const {return _hostname;}

//
// get NICK of client
//
std::string	ftClient::get_name(void) const {return this->_name;}

//
// set NICK of client
//
void ftClient::set_name(const std::string& name) {_name = name;}

//
// get address string of client
//
std::string ftClient::get_addr(void) const {return this->_addr;}

//
// get connection time in seconds
//
int ftClient::getTimeConnected(void) const
{
		time_t	end;

		time(&end);
		return (static_cast<int> (difftime(end, this->_connect)));
}

//
// get number of messages sent
//
int	ftClient::get_msgs(void) const {return _msgs;}

//
// add to message counter
//
void ftClient::add_msgsCount(int nbr = 1) { _msgs += nbr;}

//
// set msg counter to zero
//
void ftClient::set_msgsZero(void) {_msgs = 0;}

// time from last action
//
time_t	ftClient::getLastAction(void) const {return (_lastAction);}

//
// setter for the above
//
void ftClient::setLastAction(time_t newT) {_lastAction = newT;}

//
// get fd of client
//
int ftClient::get_fd(void) const { return _fd; }

bool			ftClient::isRegistered(void) const { return _val==true; }

bool			ftClient::isOperator(void) const { return _flags & OPERATOR; }

void			ftClient::set_names(const std::string& username, const std::string& realname)
{
	this->_username = username;
	this->_realname = realname;
	this->_hostname = this->_addr;
}
void			ftClient::set_pass(const std::string& pass) { this->_password = pass; }

std::string		ftClient::get_prefix(void) const
{
	std::string _prefix;

	_prefix = this->_name;
	if (this->_username.size())
		_prefix += "!" + this->_username;
	_prefix += "@" + this->_addr;
	return _prefix;
}

unsigned		ftClient::get_flags(void) { return _flags; }
void			ftClient::set_flags(const std::string& add_remove, unsigned flag)
{
	if (add_remove == "+")
		_flags |= flag;
	else
		_flags = _flags & ~flag;
}

std::string		ftClient::get_awaymsg(void) { return this->_awayMsg; }
void			ftClient::set_awaymsg(const std::string& msg) { this->_awayMsg = msg; }
std::string		ftClient::get_username(void) const { return this->_username; }

