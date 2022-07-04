#include <string>
#include <iostream>
int main()
{
	std::string _mask = "wfej!wkje@jhf";
	std::string _nick;
	std::string _user;
	std::string _host;
	int i = -1;
	std::string ret;

	while (_mask.find('!') != std::string::npos && _mask[++i] && _mask[i] != '!')
		_nick.append(1, _mask[i]);
	_nick = _nick.empty() ? "*" : _nick;
	while (_mask[++i] && _mask[i] != '@')
		_user.append(1, _mask[i]);
	_user = _user.empty() ? "*" : _user;
	while (_mask[++i] && _mask[i])
		_host.append(1, _mask[i]);
	_host = _host.empty() ? "*" : _host;

	// std::cout << _nick << "~~\n";
	// std::cout << _user << "--\n";
	// std::cout << _host << "OO\n";
	ret = _nick + "!" + _user + "@" + _host;
	std::cout << ret << "\n";
}