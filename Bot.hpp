/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: graja <graja@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 08:33:53 by graja             #+#    #+#             */
/*   Updated: 2022/07/29 10:14:50 by graja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_H
# define BOT_H

/*
# include <sys/socket.h>
# include <iostream>
# include <string.h>
# include <unistd.h>
# include <errno.h>
*/
# include "lib.hpp"

class Bot
{
		private:
				int			_bfd;
				std::string	_port;
				std::string	_addr;
				std::string	_passwd;

				Bot(Bot const & cpy);
				Bot operator=(Bot const & cpy);

		public:
				Bot(std::string address, std::string port, std::string password);
				~Bot(void);

				//getters
				int const & 		getFd(void) const;
				std::string const &	getPort(void) const;
				std::string const & getAddr(void) const;
				std::string const & getPwd(void) const;
};

#endif
