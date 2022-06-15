/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftClient.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: graja <graja@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:27:32 by graja             #+#    #+#             */
/*   Updated: 2022/06/15 12:49:35 by graja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ftClient.hpp"
#include <unistd.h>

//
// it makes no sense to copy clients, FDs are unique
//
ftClient::ftClient(const ftClient & cpy) {*this = cpy;}

ftClient ftClient::operator=(const ftClient & rgt)
{
		_fd = rgt._fd;
		_name = rgt._name;
		_addr = rgt._addr;
		_val = rgt._val;
		_connect = rgt._connect;
		_lastAction = rgt._lastAction;
		return (*this);
}

//
// Constructor needs a FD, a string for the name, a string for it's address
// val will be false until server checks validity of the client
//
ftClient::ftClient(int fd, std::string name, const std::string addr): _fd(fd), _name(name), 
		_addr(addr), _val(false) 
{
		time(&_connect);
		time(&_lastAction);
		std::cout << "NEW CLIENT: " << _addr << " joined with fd#" << fd << std::endl;
}

//
// Destructor closing the FD
//
ftClient::~ftClient(void)
{
		std::cout << _addr << " from fd#" << _fd << " says byebye" << std::endl;
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
void ftClient::set_name(std::string nname) {_name = nname;}

//
// get address string of client
//
std::string ftClient::get_addr(void) const {return this->_addr;}

//
// time from last action
//
time_t	ftClient::getLastAction(void) {return (_lastAction);}
//
// setter for the above
//
void ftClient::setLastAction(time_t newT) {_lastAction = newT;}
