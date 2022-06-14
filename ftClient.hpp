/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftClient.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: graja <graja@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:08:32 by graja             #+#    #+#             */
/*   Updated: 2022/06/14 19:00:12 by graja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <ctime>

class ftClient
{
		private:
				int			_fd;			// File descriptor
				std::string	_name;			// NICK name
				std::string	_addr;			// addr string
				bool		_val;			// flag if server has validated the client 
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

				time_t			getLastAction(void);

				//to be continued
};
