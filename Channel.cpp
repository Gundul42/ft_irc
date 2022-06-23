#include "Channel.hpp"

IrcChannel::IrcChannel(const std::string & newName, ftClient & crt): _name(newName)
{
		_topic == "";
		_limit = -1;
		_passwd = "";
		_chop.push_back(&crt);
		_ban.clear();
		_member.clear();
		_creator = &crt;
		_safe = false;
		_chanBuffer = "";
}

IrcChannel::~IrcChannel(void)
{
		_chop.clear();
		_ban.clear();
		_member.clear();
}

std::vector<ftClient*> IrcChannel::getMembers(void) const {return _member;}

std::string IrcChannel::getName(void) const {return _name;}

std::string IrcChannel::getTopic(void) const {return _topic;}

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

void IrcChannel::notSafe(void) {_safe == false;}

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
