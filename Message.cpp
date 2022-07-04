#include "Message.hpp"

Message::Message()
{
	_input = "";
	_prefix = "";
	_command = "";
	
	_param.clear();
	_trailing = "";
	_keys.clear();
	_setFlags.clear();
	_channels.clear();
}

Message::Message(std::string input)
{
	_input = "";
	_prefix = "";
	_command = "";
	_param.clear();
	_trailing = "";
	_keys.clear();
	_setFlags.clear();
	_channels.clear();
	_input = input;
	this->parse(_input);
	this->split_channels();
	this->split_flags();
}

Message::~Message() {};

Message::Message(const Message& cpy)
{
	_input = cpy._input;
	_prefix = cpy._prefix;
	_command = cpy._command;
	_param = cpy._param;
	_trailing = cpy._trailing;
	_keys = cpy._keys;
	_setFlags = cpy._setFlags;
	_channels = cpy._channels;
}

Message& Message::operator=(const Message& cpy)
{
	if (this != &cpy)
	{
		_input = cpy._input;
		_prefix = cpy._prefix;
		_command = cpy._command;
		_param = cpy._param;
		_trailing = cpy._trailing;
		_keys = cpy._keys;
		_setFlags = cpy._setFlags;
		_channels = cpy._channels;
	}
	return (*this);
}

const std::string&				Message::getInput() { return _input; }
const std::string&				Message::getPrefix() { return _prefix; }
const std::string&				Message::getCommand() { return _command; }
const std::vector<std::string>&	Message::getParam() { return _param; }
const std::string&				Message::getTrailing() { return _trailing; }
const std::vector<std::string>&	Message::getKeys() { return _keys; }
const std::vector<std::string>&	Message::getFlags() { return _setFlags; }


void							Message::parse(const std::string& input)
{
	std::stringstream	str(input);
	int					i = 0;

	for (std::string component; std::getline(str, component, ' '); )
	{
		if (*(component.end() - 1) == ' ')
			component.erase(component.end() - 1);
		if (i == 0 && component[0] == ':')
		{
			component.erase(component.begin());
			this->_prefix = component;
		}
		else if (component[0] == ':')
		{
			component.erase(component.begin());
			this->_trailing = component;
		}
		else if (!this->_trailing.empty())
			this->_trailing = this->_trailing + " " + component;
		else if ((i == 0 || i == 1) && this->_command.empty() && std::isupper(component[0]))
			this->_command = component;
		else
			this->_param.push_back(component);
		i++;
	}
	/*
	 std::cout << "*prefix: " << this->_prefix << "~\n";
	 std::cout << "*command: " << this->_command << "~\n";
	 std::cout << "*param: ";
	 for (std::vector<std::string>::iterator it = this->_param.begin(); it != this->_param.end(); it++)
	 	std::cout << *it << " ";
	 std::cout << "~\n";
	 std::cout << "*trailing: " << this->_trailing << "~\n";
	 */
}

Target::Target() {}

Target::~Target() {}

bool	Target::isMask(const std::string& target)
{
	if ((target[0] == '#' || target[0] == '$') && target[1] == '.')
	{
		std::string::size_type pos = target.find_last_of(".");
		if (target[pos + 1] != '*' && target[pos + 1] != '?')
			return true;
	}
	return false;
}

bool	Target::isNickname(const std::string& target)
{
	if (target[0] != '#' && target[0] != '$')
		return true;
	return false;
}

bool	Target::isChannel(const std::string& target)
{
	if ((target[0] == '#' && target[1] != '.') || target[0] == '&' ||  target[0] == '!' ||  target[0] == '+')
		return true;
	return false;
}

void	Message::split_channels(void)
{
	if ((_command == "PART" || _command == "JOIN") && _param.size() >= 1)
	{
		std::vector<std::string> newParam;

		for(int i = 0; i != _param.size(); i++)
		{
			std::stringstream	str(_param[i]);
			for (std::string component; std::getline(str, component, ','); )
			{
				if (*(component.end() - 1) == ',')
					component.erase(component.end() - 1);
				newParam.push_back(component);
			}
			if (i == 0)
				_channels = newParam;
			else
				_keys = newParam;
			newParam.clear();
		}
		// std::vector<std::string>::iterator it = _keys.begin();
		// for (; it != _keys.end(); it++)
		// 	std::cout << (*it) << '\n';
	}
}

void	Message::split_flags(void)
{
	std::string	flags;
	std::string	str;

	if (_command == "MODE" && _param.size() > 1)
	{
		flags = _param[1];
		for (int i = 0; i != flags.size(); i++)
		{
			str = flags[i];
			_setFlags.push_back(str);
		}
	}
}