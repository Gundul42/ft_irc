/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: graja <graja@student.42wolfsburg.de>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/07/29 08:33:53 by graja             #+#    #+#             */
/*   Updated: 2022/07/30 15:27:58 by graja            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOT_H
# define BOT_H

# include "lib.hpp"
# include "BotConfig.hpp"

class Bot
{
		private:
				int			_bfd;
				std::string	_port;
				std::string	_addr;
				std::string	_passwd;

				Bot(Bot const & cpy);
				Bot operator=(Bot const & cpy);

				void		write(std::string const & msg) const;
				std::string	read(void) const;
				std::string getName(std::string const & msg) const;
				std::string	getInfo(std::string const & msg) const;
				void		answer(std::string const & msg) const;
				std::string	encrypt(std::string const & msg) const;
				std::string decrypt(std::string const & msg) const;

		public:
				Bot(std::string const & address, std::string const & port,
								std::string const & password);
				~Bot(void);
				
				bool		login(void) const;
				bool		loop(void);
};

#endif
