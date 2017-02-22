#pragma once
#include "MyParser_Boost_Cfg.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "CommandTree.h"
#include "CmdParser.h"
#include <unordered_map>
#include <deque>
namespace Command {
	namespace Parser {

		inline boost::optional<std::pair<boost::optional<Command::Actions::Vertical>, boost::optional<Command::Actions::Horizontal>>> isDirection(const std::string& arg) {
			using ret_t = decltype(isDirection(std::string{}));
			using t1 = ret_t::value_type::first_type;
			using t2 = ret_t::value_type::second_type;
			using namespace Command::Actions;

			auto it = arg.find_first_not_of("DFBU");
			if (arg.size() <= 2 && it == std::string::npos) {
				t1 vert;
				t2 hriz;
				if (arg == "D") {
					vert = Vertical::DOWN;
				}
				else if (arg == "U") {
					vert = Vertical::UP;
				}
				else if (arg == "B") {
					hriz = Horizontal::LEFT;
				}
				else if (arg == "F") {
					hriz = Horizontal::RIGHT;
				}
				else if (arg == "DB") {
					vert = Vertical::DOWN;
					hriz = Horizontal::LEFT;
				}
				else if (arg == "DF") {
					vert = Vertical::DOWN;
					hriz = Horizontal::RIGHT;
				}
				else if (arg == "UB") {
					vert = Vertical::UP;
					hriz = Horizontal::LEFT;
				}
				else if (arg == "UF") {
					vert = Vertical::UP;
					hriz = Horizontal::RIGHT;
				}

				if (vert || hriz) {
					return ret_t{ ret_t::value_type{ vert,hriz } };
				}
				else {
					return{};
				}
			}
			else {
				return{};
			}
		}

		using namespace boost::spirit;
		using Iterator = decltype(std::declval<const std::string>().begin());
		//template <class Iterator>
		struct cmd_grammar :public qi::grammar<Iterator, Cmd(), ascii::space_type> {
		private:
			qi::rule<Iterator, Cmd(), ascii::space_type> wholeexpr;

			qi::rule<Iterator, std::string(), ascii::space_type> buttonname;
			qi::rule<Iterator, std::string(), ascii::space_type> cmdname;
			qi::rule<Iterator, Actions::Sequence(), ascii::space_type> seq_action;
			qi::rule<Iterator, Actions::SameTime(), ascii::space_type> plus_action;
			qi::rule<Iterator, Actions::Action(),qi::locals<bool>, ascii::space_type> terminal_action;
			qi::rule<Iterator, unsigned(), ascii::space_type> time;

			qi::rule<Iterator, void(), ascii::space_type> cmd_header, remap_header;
			qi::rule<Iterator, std::unordered_map <std::string, unsigned>(), ascii::space_type> remap;
			qi::rule<Iterator, CmdData(), ascii::space_type> cmd;

			std::string charset = "a-zA-Z";
			std::string numset = "0-9";
		public:
			cmd_grammar() : cmd_grammar::base_type(wholeexpr, "wholeexpr") {

				buttonname = (lexeme[qi::char_(charset) > *(qi::char_(charset + numset))]);
				cmdname = (lexeme[qi::char_(charset) > *(qi::char_(charset + numset))]);

				seq_action = (plus_action % ',')[
					qi::_val = boost::phoenix::bind([&](const std::vector<Actions::SameTime>& arg) {
						Actions::Sequence ret;
						for (const auto& elem : arg) {
							if (elem.data.size() == 1) {
								ret.data.push_back(elem.data[0]);
							}
							else {
								ret.data.push_back(elem);
							}
						}
						return ret;
					}, qi::_1)
				];
				plus_action = terminal_action[boost::phoenix::bind([&](Actions::SameTime & val, Actions::Action arg) {val.data.push_back(arg); }, qi::_val, qi::_1)] >> *((lit("+") > terminal_action)[
					boost::phoenix::bind([&](Actions::SameTime & val, Actions::Action arg) {val.data.push_back(arg); }, qi::_val, qi::_1)
				]);

				terminal_action =
					(lit("/") >> (-qi::uint_) >> buttonname)[qi::_val = boost::phoenix::bind([](std::string arg, boost::optional<unsigned int> arg2) {
					using namespace Actions;
					auto ret = isDirection(arg);
					if (ret) {
						auto val = *ret;
						return Action{ Hold_Dir{ val.first,val.second,static_cast<int>(arg2 ? *arg2 : 0) } };
					}
					else {
						return Action{ Hold{ arg,static_cast<int>(arg2 ? *arg2 : 0) } };
					}}, qi::_2, qi::_1)]
					| (lit("~") >> (-qi::uint_) >> buttonname)[qi::_val = boost::phoenix::bind([](std::string arg, boost::optional<unsigned int>arg2) {
						using namespace Actions;
						auto ret = isDirection(arg);
						if (ret) {
							auto val = *ret;
							return Action{ Release_Dir{ val.first,val.second,static_cast<int>(arg2 ? *arg2 : 0) } };
						}
						else {
							return Action{ Release{ arg,static_cast<int>(arg2 ? *arg2 : 0) } };
						}}, qi::_2, qi::_1)]
						| qi::lit(">") >> buttonname[qi::_val = boost::phoenix::bind([](std::string arg, bool & arg2) {
							using namespace Actions;
							auto ret = isDirection(arg);
							if (ret) {
								arg2 = true;
								auto val = *ret;
								return Action{ Press_Dir{ val.first,val.second,true } };
							}
							else {
								arg2 = false;
								return Action{};
							} }, qi::_1, qi::_a)] >> qi::eps[qi::_pass = (qi::_a)]
								| buttonname[qi::_val = boost::phoenix::bind([](std::string arg) {
								using namespace Actions;
								auto ret = isDirection(arg);
								if (ret) {
									auto val = *ret;
									return Action{ Press_Dir{ val.first,val.second,false} };
								}
								else {
									return Action{ Press{ arg } };
								} }, qi::_1)];


							cmd_header = lit("[") > qi::no_case[lit("Command")] > lit("]");
							remap_header = lit("[") > qi::no_case[lit("Remap")] > lit("]");


							remap = remap_header > (*(buttonname > lit("=") > qi::int_)[boost::phoenix::bind(
								[](std::unordered_map<std::string, unsigned int>& val, std::string const lhs, unsigned int const rhs) {val[lhs] = rhs; return val; }
							, qi::_val, boost::spirit::qi::_1, boost::spirit::qi::_2)]);



							cmd = ((cmd_header) > (lit("name") > lit("=") > lit("\"") > cmdname > lit("\"")) > (lit("command") > lit("=") > seq_action) > (lit("time") > lit("=") > qi::int_))[
								qi::_val = boost::phoenix::bind(
									[](std::string name, Actions::Sequence act, unsigned int const time) {
									return CmdData{ name,act,time };
								}
								, qi::_1, boost::spirit::qi::_2, boost::spirit::qi::_3)];


							wholeexpr = (remap > (*cmd))[qi::_val =
								boost::phoenix::bind([&](std::unordered_map <std::string, unsigned>  arg1, std::vector<CmdData>   arg2) {
								return Cmd{ arg1,arg2 }; }, qi::_1, qi::_2)
							];

							wholeexpr.name("whole");
							buttonname.name("buttonname");
							cmdname.name("cmdname");
							seq_action.name("seq");
							plus_action.name("plus");

							terminal_action.name("terminal");
							time.name("time");
							cmd_header.name("cmd_header");
							remap_header.name("remap_header");
							remap.name("remap");
							cmd.name("cmd");

							qi::on_error<qi::fail>
								(
									wholeexpr
									, boost::phoenix::if_(boost::phoenix::ref(errorwhat) && boost::phoenix::ref(errorpos))[
										errorwhat << qi::_4,
											(boost::phoenix::if_(boost::phoenix::ref(failflag) == false)[(boost::phoenix::ref(errorpos) << boost::phoenix::construct<std::string>(qi::_3, qi::_2)), boost::phoenix::ref(failflag) = true])
									]
									);



			}
		private:
			bool failflag = false;
		public:

			std::ostringstream errorwhat;
			std::ostringstream errorpos;

			bool isfailed() { return failflag; }

		};

		inline Cmd parse_impl(const std::string& arg) {
			auto s = arg;
			std::deque<std::pair<unsigned int, unsigned int>> comment;
			for (auto it = s.begin(), end = s.end(); it != end; it++) {
				if ((*it) == ';') {
					for (auto it2 = it;; it2++) {
						if (it2 == end || (*it2) == '\n'){
							comment.push_front({ std::distance(s.begin(),it),std::distance(s.begin(),it2) });
							it = it2-1;
							break;
						}
					}
				}
			}
			while (!comment.empty()) {
				auto top = comment.front();
				s.erase(s.begin() + top.first, s.begin() + top.second);
				comment.pop_front();
			}

			cmd_grammar grammer;
			Cmd cmd;
			auto b = s.begin();
			auto e = s.end();
			boost::spirit::qi::phrase_parse(b, e, grammer, boost::spirit::ascii::space, cmd);
			if (grammer.isfailed()) {
				std::string pos{ grammer.errorpos.str() };
				std::string what{ "expected: "+ grammer.errorwhat.str() };
				throw parse_error{ pos,what };
			}
			return cmd;
		}

	}
}