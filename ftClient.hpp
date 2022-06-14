/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ftClient.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: graja <graja@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:08:32 by graja             #+#    #+#             */
/*   Updated: 2022/06/14 15:14:22 by graja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

class ftClient
{
		private:
				int			_fd;			// File descriptor
				std::string	_name;			// NICK name
				std::string	_addr;			// addr string
				bool		_val;			// flag if server has validated the client 
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

				//to be continued
};
