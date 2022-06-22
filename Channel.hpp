#ifndef CHANNEL_H
# define CHANNEL_H

# include "ftClient.hpp"
# include "lib.hpp"
# include <vector>

class IrcChannel
{
	private:
		std::string								_name;
		std::string								_topic;
		int										_limit;
		std::string								_passwd;
		std::vector<ftClient*>					_chop;
		std::vector<ftClient*>					_ban;
		ftClient*								_creator;
		bool									_safe;
		std::string								_chanBuffer;

		IrcChannel(const std::string & newName, ftClient & crt);
		IrcChannel & operator=(const IrcChannel & cpy);

	public:
		IrcChannel(const std::string & newName);
		~IrcChannel(void);

		//getters
		std::string  		getName(void) const;
		std::string  		getTopic(void) const;
		int 				getLimit(void) const;
		std::string  		getPasswd(void) const;
		bool				isChop(const ftClient & member) const;
		bool				isCreator(const ftClient & member) const;
		bool				isSafe(void) const;
		std::string 		getBuffer(void) const;
		bool				isBanned(const ftClient & member) const;

		//setters
		void				setName(const std::string newName);
		void				setTopic(const std::string newTopic);
		void				setLimit(const int newLimit);
		void				setPasswd(const std::string newPasswd);
		void				setCreator(ftClient & member);
		bool				banMember(ftClient & member);
		void				setSafe(void);
		void				setBuffer(const std::string & buffer);
		
		//manipulators
		void				notSafe(void);
		bool				unbanMember(const ftClient & member);
		bool				addChop(ftClient & member);
		bool				removeChop(ftClient & member);
};

#endif

