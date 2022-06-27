#ifndef CONFIG_H
# define CONFIG_H

#include <list>

# define IRCSERVNAME "ftIrcServ.nowhere.xy"
# define IRCSERVWLC "Welcome to ft_IrcServ V 0.1beta"
# define IRCSERVVERSION "V0.1beta"
# define IRCSERVCDATE "180622"
# define IRCSERVUSERMODES "aiwroOs"
# define IRCSERVCHANMODES "birkfsmqz"
# define IRCFLOODCONTROL 2								//time intervall it is checked for msgs
# define IRCMAXMSGCOUNT 5								//that many can be sent in floodcontrol
# define OPERNUM 1
# define OPER1 "leilawen", "admin", "Meimac.local"

# define Verify_oper(...) Verify_oper_X(__VA_ARGS__)


#endif
