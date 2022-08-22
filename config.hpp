/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwen <mwen@student.42wolfsburg.de>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/20 13:01:33 by graja             #+#    #+#             */
/*   Updated: 2022/08/22 10:40:52 by mwen             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_H
# define CONFIG_H

# define IRCSERVNAME "irc.gundul.net"
# define IRCSERVWLC "Welcome to IrcServ V1.0"
# define IRCSERVVERSION "ircserv V1.0 [mwen graja]"
# define IRCSERVCDATE "200822"
# define IRCSERVUSERMODES "ao"
# define IRCSERVCHANMODES "ovbiklmnt"
# define IRCFLOODCONTROL 2								//time intervall it is checked for msgs
# define IRCMAXMSGCOUNT 5								//that many can be sent in floodcontrol
# define IRCMOTDFILE "ft_irc.motd"						//ascii file for motd
# define IRCADMIN "admin"
# define IRCADMINPWD "admin"
# define IRCTIMEOUT 20
#endif
