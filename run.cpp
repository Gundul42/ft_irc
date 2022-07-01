/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwen <mwen@student.42wolfsburg.de>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 18:52:50 by graja             #+#    #+#             */
/*   Updated: 2022/07/01 23:06:10 by mwen             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServ.hpp"
#include "Channel.hpp"
#include "ftClient.hpp"
int	main(void)
{
	IrcServ	srv("6667");

	srv.loop();

	return(0);
}
