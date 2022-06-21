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

const std::string&	Message::getInput() { return _input; }
const std::string&	Message::getPrefix() { return _prefix; }
const std::string&	Message::getCommand() { return _command; }
const std::string&	Message::getParam() { return _param; }
const std::string&	Message::getTrailing() { return _trailing; }

void				Message::parse(const std::string& input)
{
	std::stringstream	str(input);
	int					i = 0;

	for (std::string component; std::getline(str, component, ' '); )
	{
		if (*(component.end() - 1) == ' ')
			component.erase(component.end() - 1);
		if (i == 0 && component[0] == ':')
			this->_prefix = component;
		else if (component[0] == ':')
			this->_trailing = component;
		else if ((i == 0 || i == 1) && this->_command.empty())
			this->_command = component;
		else
			this->_param = this->_param + component + " ";
		i++;
	}
	// std::cout << "*prefix: " << this->_prefix << "\n";
	// std::cout << "*command: " << this->_command << "\n";
	// std::cout << "*param: " << this->_param << "\n";
	// std::cout << "*trailing: " << this->_trailing << "\n";
}
