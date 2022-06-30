#ifndef CHANNEL_H
# define CHANNEL_H

# include "ftClient.hpp"
# include "lib.hpp"
# include "Message.hpp"
# include <vector>

class ChannelMode
{
public:
	enum Flag
	{
		/* 'a' - Anonymous channel flag*/
		ANONYMOUS = 1,
		/* 'i' - Invite-only Channel flag */
		INVITE_ONLY = 1 << 1,
		/* 'm' - Moderated Channel flag */
		MODERATED = 1 << 2,
		/* 'n' - No IRC::Messages to Channel from User on the outside */
		NO_OUTSIDE_MSG = 1 << 3,
		/* 'q' - Quiet channel flag */
		QUIET = 1 << 4,
		/* 'p' - Private channel flag */
		PRIVATE = 1 << 5,
		/* 's' - Secret channel flag */
		SECRET = 1 << 6,
		/* 't' - Topic settable by Channel operator only flag */
		TOPIC_SETTABLE_BY_CHANOP = 1 << 7,
		/* 'k' - Channel key */
		KEY = 1 << 8,
		/* 'l' - User limit */
		LIMIT = 1 << 9,
		/* 'b' - Ban mask */
		BAN_MASK = 1 << 10,
		/* 'e' - Exception mask */
		EXCEPTION_MASK = 1 << 11,
		/* 'I' - Invitation mask */
		INVITATION_MASK = 1 << 12,
		/* 'z' - Operator moderated flag */
		OP_MODERATED = 1 << 13,
	};

	struct Masks
	{
		std::string	_key;
		std::string	_limit;
		std::string	_ban;
		std::string	_exception;
		std::string	_invitation;

		Masks();
		~Masks();
		void	setMasks(unsigned flag, const std::string& str);
	};

	ChannelMode(unsigned flags = 0);
	~ChannelMode();

	static Flag	parse(char c);
	std::string	toString() const;

protected:
	unsigned					_flags;

private:
	/* Quick lookup table for lower alphabet */
	static const unsigned short _lowerFlagTable[26];
};

class IrcChannel : public ChannelMode
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
		std::string								_key;

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
		unsigned				getFlags(void);
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
		void					setFlags(const std::string& add_remove, unsigned flag);

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

