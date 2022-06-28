#include "Message.hpp"

Message::Message(std::string input)
{
	_input.clear();
	_prefix.clear();
	_command.clear();
	_param.clear();
	_trailing.clear();
	_input = input;
	this->parse(_input);
}

Message::~Message() {};

const std::string&				Message::getInput() { return _input; }
const std::string&				Message::getPrefix() { return _prefix; }
const std::string&				Message::getCommand() { return _command; }
const std::vector<std::string>&	Message::getParam() { return _param; }
const std::string&				Message::getTrailing() { return _trailing; }

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
		else if ((i == 0 || i == 1) && this->_command.empty())
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
	if ((target[0] == '#' && target[1] != '.') || target[0] == '&')
		return true;
	return false;
}
