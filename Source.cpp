
# include "CommandEval.h"
# include "CmdParser.h"
# include<iostream>
# include<fstream>
# include<atlstr.h>





int main() 
{
	
	Command::CEval eval{ {} };
	eval.Load_file("Text.txt");
	for (auto& elem : eval.getCmddata()) {
		Command::visitors::printer p{};
		boost::apply_visitor(p, elem.second.data);
		std::cout << elem.first << std::endl;
		std::cout << p.out << std::endl << std::endl;;
	}
	return 0;
}
