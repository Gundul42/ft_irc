/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftClient.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwen <mwen@student.42wolfsburg.de>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:08:32 by graja             #+#    #+#             */
/*   Updated: 2022/06/15 23:59:15 by mwen             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <ctime>

class ftClient
{
		private:
				int			_fd;			// File descriptor
				std::string	_name;			// NICK name
				std::string	_addr;			// addr string
				bool		_val;			// flag if server has validated the client 
				bool		_oper;		// if the client is an operator
				time_t		_connect;		// time of connection
				time_t		_lastAction;	// when was the last action
				//maybe more to come, at least a timestamp

				ftClient(const ftClient & cpy);
				ftClient operator=(const ftClient & rgt);

		public:
				ftClient(int fd,const std::string name, const std::string addr);
				~ftClient(void);

				void			validate(void);

				std::string		get_name(void) const;
				void			set_name(const std::string nname);
				std::string 	get_addr(void) const;
				const int		get_fd(void) const;

				time_t			getLastAction(void);
				void			setLastAction(time_t newT);

				bool			isRegistered(void) const;
				bool			isOperator(void) const;

				//to be continued
};
