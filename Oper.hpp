#pragma once

# include <string>
# include <fstream>
# include <iostream>
# include <map>
# include <memory>

class Oper
{
	std::string _nick;
	std::string _pass;
	std::string _host;

	Oper();
	Oper(const Oper& other);
	Oper& operator=(const Oper& other);

	public:
		Oper(std::string nick, std::string pass, std::string host);
		~Oper();

		const std::string&	getNick() const;
		const std::string&	getPass() const;
		const std::string&	getHost() const;

};

class OperList
{
	public:
		typedef std::map<std::string, Oper*>	operMap;

	private:
		operMap	_operList;

		OperList(const OperList& other);
		OperList& operator=(const OperList& other);

	public:
		OperList();
		~OperList();

		const operMap&	getOperList() const;

	private:
		bool			getOper(std::string& oper);
		std::string*	getString(const std::string& from, const std::string& tofind) const;

};