#ifndef CHANNEL_H
# define CHANNEL_H

# include "ftClient.hpp"
# include "lib.hpp"
# include "Message.hpp"
# include <vector>

class IrcChannel
{
	private:
		std::string								_name;
		time_t									_ctime;
		std::string								_topic;
		int										_limit;
		std::string								_passwd;
		std::vector<ftClient*>					_chop;
		std::vector<ftClient*>					_ban;
		std::vector<ftClient*>					_member;
		ftClient*								_creator;
		bool									_safe;
		std::string								_chanBuffer;

	public:
		IrcChannel(void);
		IrcChannel(const std::string & newName, ftClient & crt);
		
		~IrcChannel(void);
		
		IrcChannel(const IrcChannel & cpy);
		IrcChannel & operator=(const IrcChannel & cpy);

		//getters
		std::string  			getName(void) const;
		std::string				getCtime(void) const;
		std::string  			getTopic(void) const;
		int 					getLimit(void) const;
		std::vector<ftClient*>	getMembers(void) const;
		ftClient*				getCreator(void) const;
		std::string  			getPasswd(void) const;
		std::string 			getBuffer(void) const;
		bool					isChop(const ftClient & member) const;
		bool					isCreator(const ftClient & member) const;
		bool					isSafe(void) const;
		bool					isBanned(const ftClient & member) const;
		bool					isMember(const ftClient & candid) const;

		//setters
		bool					addMember(ftClient & member);
		void					setName(const std::string newName);
		void					setTopic(const std::string newTopic);
		void					setLimit(const int newLimit);
		void					setPasswd(const std::string newPasswd);
		void					setCreator(ftClient & member);
		bool					banMember(ftClient & member);
		void					setSafe(void);
		void					setBuffer(const std::string & buffer);
		
		//manipulators
		void					notSafe(void);
		bool					unbanMember(const ftClient & member);
		bool					addChop(ftClient & member);
		bool					removeChop(ftClient & member);
		bool					removeMember(ftClient & member);

		//other
		bool					valChanName(const std::string name) const;
};

#endif

