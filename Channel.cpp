#include "Channel.hpp"

ChannelMode::ChannelMode(unsigned flags) : _flags(flags) {}

ChannelMode::~ChannelMode() {}

ChannelMode::Flag ChannelMode::parse(char c)
{
	unsigned f;
	f = islower(c) ? _lowerFlagTable[c - 'a'] : (c == 'I') * INVITATION_MASK;
	return (static_cast<Flag>(f));
}

const unsigned short ChannelMode::_lowerFlagTable[] =
{
	0, BAN_MASK, 0, 0, EXCEPTION_MASK, 0, 0, 0, INVITE_ONLY, 0, KEY,
	LIMIT, MODERATED, NO_OUTSIDE_MSG, 0, PRIVATE, 0, 0, SECRET,
	TOPIC_SETTABLE_BY_CHANOP, 0, 0, 0, 0, 0, 0
};

IrcChannel::IrcChannel(const std::string & newName, ftClient & crt): _name(newName)
{
		time(&_ctime);

		_topic = "";
		_limit = -1;
		_passwd = "";
		_chop.push_back(&crt);
		_ban.clear();
		_member.clear();
		_creator = &crt;
		_safe = false;
		_chanBuffer = "";
}

IrcChannel::IrcChannel(void)
{
		time(&_ctime);

		_name.clear();
		_topic.clear();
		_limit = -1;
		_passwd.clear();
		_chop.clear();
		_ban.clear();
		_member.clear();
		_creator = NULL;
		_safe = false;
		_chanBuffer.clear();
}

IrcChannel::IrcChannel(const IrcChannel & cpy) {*this = cpy;}

IrcChannel & IrcChannel::operator=(const IrcChannel & cpy)
{
		_name = cpy._name;
		_ctime = cpy._ctime;
		_topic = cpy._topic;
		_limit = cpy._limit;
		_passwd = cpy._passwd;
		_chop = cpy._chop;
		_ban = cpy._ban;
		_member = cpy._member;
		_creator = cpy._creator;
		_safe = cpy._safe;
		_chanBuffer = cpy._chanBuffer;
		_flags = cpy._flags;
		return (*this);
}

IrcChannel::~IrcChannel(void)
{
		_chop.clear();
		_ban.clear();
		_member.clear();
}

std::vector<ftClient*> IrcChannel::getMembers(void) const {return _member;}

std::string IrcChannel::getName(void) const {return _name;}
		
std::string IrcChannel::getCtime(void) const
{
		std::ostringstream	ost;

		ost << _ctime;
		return ost.str();
}

std::string IrcChannel::getTopic(void) const {return _topic;}

ftClient* IrcChannel::getCreator(void) const {return _creator;}

int IrcChannel::getLimit(void) const {return _limit;}

std::string IrcChannel::getPasswd(void) const {return _passwd;}

bool IrcChannel::isChop(const ftClient & member) const
{
		std::vector<ftClient*>::const_iterator	in;

		if (_chop.size() == 0)
				return false;
		in = _chop.begin();
		while (in != _chop.end())
		{
				if ((*in)->get_name() == member.get_name())
						return true;
				in++;
		}
		return false;
}

bool IrcChannel::isCreator(const ftClient & clt) const
{
		if (clt.get_name() == _creator->get_name())
				return true;
		return false;
}

bool IrcChannel::isSafe(void) const {return _safe;}

std::string IrcChannel::getBuffer(void) const {return _chanBuffer;}

bool IrcChannel::isBanned(const ftClient & member) const
{
		std::vector<ftClient*>::const_iterator	in;

		if (_ban.size() == 0)
				return false;
		in = _ban.begin();
		while (in != _ban.end())
		{
				if ((*in)->get_name() == member.get_name())
						return true;
				in++;
		}
		return false;
}

bool IrcChannel::isMember(const ftClient & candid) const
{
		std::vector<ftClient*>::const_iterator	in;

		if (_member.size() == 0)
				return false;
		in = _member.begin();
		while (in != _member.end())
		{
				if ((*in)->get_name() == candid.get_name())
						return true;
				in++;
		}
		return false;
}

bool IrcChannel::addMember(ftClient & member)
{
		if (isMember(member) == true)
				return false;
		_member.push_back(&member);
		return true;
}

void IrcChannel::setName(const std::string newName) {_name = newName;}

void IrcChannel::setTopic(const std::string newTopic) {_topic = newTopic;}

void IrcChannel::setLimit(const int newLimit) {_limit = newLimit;}

void IrcChannel::setPasswd(const std::string newPasswd) {_passwd = newPasswd;}

void IrcChannel::setCreator(ftClient & member) {_creator = &member;}

bool IrcChannel::banMember(ftClient & member)
{
		if (isBanned(member) == true)
				return false;
		_ban.push_back(&member);
		return true;
}

void IrcChannel::setSafe(void) {_safe = true;}

void IrcChannel::setBuffer(const std::string & buffer) {_chanBuffer = buffer;}

void IrcChannel::notSafe(void) {_safe = false;}

bool IrcChannel::unbanMember(const ftClient & member)
{
		std::vector<ftClient*>::const_iterator	in;

		if (_ban.size() == 0)
				return false;
		in = _ban.begin();
		while (in != _ban.end())
		{
				if ((*in)->get_name() == member.get_name())
				{
						_ban.erase(in);
						return true;
				}
				in++;
		}
		return false;
}

bool IrcChannel::addChop(ftClient & member) 
{
		if (isChop(member) == true)
				return false;
		_chop.push_back(&member);
		return true;
}

bool IrcChannel::removeChop(ftClient & member)
{
		std::vector<ftClient*>::const_iterator	in;

		if (_chop.size() == 0 || isChop(member) == false)
				return false;
		in = _chop.begin();
		while (in != _chop.end())
		{
				if ((*in)->get_name() == member.get_name())
				{
						_chop.erase(in);
						return true;
				}
				in++;
		}
		return false;
}

bool IrcChannel::removeMember(ftClient & member)
{
		std::vector<ftClient*>::const_iterator	in;

		if (_member.size() == 0 || isMember(member) == false)
				return false;
		in = _member.begin();
		while (in != _member.end())
		{
				if ((*in)->get_name() == member.get_name())
				{
						_member.erase(in);
						return true;
				}
				in++;
		}
		return false;
}
		
bool IrcChannel::valChanName(const std::string name) const
{
		std::string::const_iterator	it = name.begin();

		if (name.length() > 50)
			return false;
		if (name[0] != '#' && name[0] != '!' && name[0] != '+' && name[0] != '&')
			return false;
		while (it != name.end())
		{
				if (*it == '\x07' || *it == ' ' || *it == ',' || *it == ':')
					return false;
				it++;
		}
		return true;
}
