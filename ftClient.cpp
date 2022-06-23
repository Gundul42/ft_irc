/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftClient.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwen <mwen@student.42wolfsburg.de>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:27:32 by graja             #+#    #+#             */
/*   Updated: 2022/06/23 13:11:18 by graja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftClient.hpp"
#include <unistd.h>

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
		_oper = rgt._oper;
		_connect = rgt._connect;
		_lastAction = rgt._lastAction;
		_msgs = rgt._msgs;
		_host = rgt._host;
		_username = rgt._username;
		_server = rgt._server;
		return (*this);
}

//
// Constructor needs a FD, a string for the name, a string for it's address
// val will be false until server checks validity of the client
//
ftClient::ftClient(int fd, std::string name, const std::string addr): _fd(fd), _name(name), 
		_addr(addr), _val(false), _oper(false)
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

bool			ftClient::isOperator(void) const { return _oper==true; }
