#pragma once
#pragma once
#include <MyParser_Boost_Cfg.h>
#include<unordered_map>
#include<deque>
#include<fstream>
#include<iostream>
#include<boost/variant/variant.hpp>
#include"CommandTree.h"
#include"CmdParser.h"

namespace Command {


	using Buffer_t = std::deque<std::unordered_map<unsigned int, int>>;
	using ret_t = std::pair<bool, Buffer_t::const_iterator>;

	using Command_Name = std::string;
	using Mapped_Name = std::string;

	namespace visitors {
		using namespace Actions;
		struct err_no_mapped_buttton {
			std::string err;
		};
		static constexpr unsigned int Sametime_margine = 3;
		class executer : public boost::static_visitor<ret_t> {
		private:
			bool isPress(const Buffer_t::const_iterator& _it, unsigned int button)const {
				if (_it == itend) {
					return false;
				}
				auto ret = (*it).count(button) > 0 && (*it).at(button) == 0;
				return ret;
			}
			bool isHold(const Buffer_t::const_iterator& _it, unsigned int button, unsigned int time = 0)const {
				if (_it == itend) {
					return false;
				}
				auto ret = (*it).count(button) > 0 && (*it).at(button) >= time;
				return ret;
			}
			bool isRelease(const Buffer_t::const_iterator& _it, unsigned int button, int time = -1)const {
				if (_it == itend) {
					return false;
				}
				auto f1 = (*it).count(button) > 0 && (*it).at(button) == -1;
				auto f2 = (time < 0) || (it != itend && (it + 1) != itend && (*(it + 1)).count(button) != 0 && (*(it + 1)).at(button) > time);
				auto ret = f1&&f2;
				return ret;
			}
		public:
			executer(const Buffer_t::const_iterator& _it, const Buffer_t::const_iterator& _itend, const std::unordered_map<Mapped_Name, unsigned int>& _button_map, unsigned int _margine = 1) :it(_it), itend(_itend), button_map(_button_map), margine(_margine) {}
			ret_t operator()(Sequence arg) const {
				decltype(it) ret = it;
				for (auto it_cmd = arg.data.rbegin(), end = arg.data.rend(); it_cmd != end;) {
					auto dt = ret - it;
					if (dt > margine) {
						return ret_t{ false,it };
					}
					auto & elem = *it_cmd;
					auto tmpret = boost::apply_visitor(executer{ ret,itend,button_map,margine - static_cast<unsigned int>(ret - it) }, elem);
					if (tmpret.first) {
						ret = tmpret.second;
						it_cmd++;
					}
					else {
						if (it_cmd == arg.data.rbegin()) {
							return ret_t{ false,it };
						}
					}

					if (ret != itend) {
						ret++;
					}
					else {
						return ret_t{ false,it };
					}
				}
				return{ true,ret };
			}

			ret_t operator()(SameTime arg)const {
				decltype(it) oldest = it;
				bool just_exist = false;
				for (auto&elem : arg.data) {
					bool succ = false;
					for (auto it2 = it, itend2 = itend; it2 - it < Sametime_margine && it2 != itend2; it2++) {
						auto ret = boost::apply_visitor(executer{ it2,itend2,button_map,margine }, elem);
						if (ret.first) {
							if (it2 == it) {
								just_exist = true;
							}
							oldest = it2;
							succ = true;
							break;
						}
					}
					if (!succ) {
						return{ false,it };
					}
				}
				return just_exist ? ret_t{ true, oldest } : ret_t{ false, it };
			}

			ret_t operator()(Press arg) const {
				if (button_map.count(arg.button) == 0) {
					throw err_no_mapped_buttton{ arg.button };
				}
				auto button = button_map.at(arg.button);
				return ret_t{ isPress(it,button),it };
			}


			ret_t operator()(Hold arg) const {
				if (button_map.count(arg.button) == 0) {
					throw err_no_mapped_buttton{ arg.button };
				}
				auto button = button_map.at(arg.button);
				return ret_t{ isHold(it,button,arg.time),it };
			}

			ret_t operator()(Release arg) const {
				if (button_map.count(arg.button) == 0) {
					throw err_no_mapped_buttton{ arg.button };
				}
				auto button = button_map.at(arg.button);
				return ret_t{ isRelease(it,button,arg.time),it };
			}

			ret_t operator()(Press_Dir arg) const {
				if (it == itend) {
					return { false ,it };
				}
				if (arg.v&&arg.h) {
					auto buttonv = button_map.at(arg.v == Actions::Vertical::UP ? "U" : "D");
					auto buttonh = button_map.at(arg.h == Actions::Horizontal::RIGHT ? "F" : "B");
					if ((*it).count(buttonv) > 0 && ((*it).count(buttonh) > 0)) {
						auto timev = (*it).at(buttonv);
						auto timeh = (*it).at(buttonh);
						return{ timev >= 0 && timeh >= 0 && (timev == 0 || timeh == 0),it };
					}
				}
				else if (arg.v) {
					auto button = button_map.at(arg.v == Actions::Vertical::UP ? "U" : "D");
					auto f = button_map.at("F");
					auto b = button_map.at("B");
					auto ret = false;
					if (arg.only) {
						ret = isPress(it, button) && !isHold(it, b) && !isHold(it, f);
					}
					else {
						ret = (isPress(it, button) && !isHold(it, b) && !isHold(it, f)) || isHold(it, button) && (isRelease(it, b) ^ isRelease(it, f));
					}
					return ret_t{ ret,it };
				}
				else if (arg.h) {
					auto button = button_map.at(arg.h == Actions::Horizontal::RIGHT ? "F" : "B");
					auto u = button_map.at("U");
					auto d = button_map.at("D");
					auto ret = false;
					if (arg.only) {
						ret = isPress(it, button) && !isHold(it, u) && !isHold(it, d);
					}
					else {
						ret = (isPress(it, button) && !isHold(it, u) && !isHold(it, d)) || isHold(it, button) && (isRelease(it, u) ^ isRelease(it, d));
					}
					return ret_t{ ret,it };
				}
				return{ false,it };
			}
			ret_t operator()(Hold_Dir arg) const {
				if (it == itend) {
					return { false ,it };
				}
				if (arg.v&&arg.h) {
					auto buttonv = button_map.at(arg.v == Actions::Vertical::UP ? "U" : "D");
					auto buttonh = button_map.at(arg.h == Actions::Horizontal::RIGHT ? "F" : "B");
					if ((*it).count(buttonv) > 0 && ((*it).count(buttonh) > 0)) {
						auto timev = (*it).at(buttonv);
						auto timeh = (*it).at(buttonh);
						return{ timev >= 0 && timeh >= 0,it };
					}
				}
				else if (arg.v) {
					auto button = button_map.at(arg.v == Actions::Vertical::UP ? "U" : "D");
					return ret_t{ (*it).count(button) > 0 && (*it).at(button) >= 0,it };
				}
				else if (arg.h) {
					auto button = button_map.at(arg.h == Actions::Horizontal::RIGHT ? "F" : "B");
					return ret_t{ (*it).count(button) > 0 && (*it).at(button) >= 0,it };
				}
				return{ false,it };
			}
			ret_t operator()(Release_Dir arg) const {
				if (it == itend) {
					return{ false,it };
				}

				if (arg.v&&arg.h) {
					auto buttonv = button_map.at(arg.v == Actions::Vertical::UP ? "U" : "D");
					auto buttonh = button_map.at(arg.h == Actions::Horizontal::RIGHT ? "F" : "B");
					if ((*it).count(buttonv) > 0 && ((*it).count(buttonh) > 0)) {
						auto timev = (*it).at(buttonv);
						auto timeh = (*it).at(buttonh);
						auto pretimev = (*(it + 1)).at(buttonv);
						auto pretimeh = (*(it + 1)).at(buttonh);
						return{ (timev == -1 || timeh == -1) && (pretimev > arg.time && pretimeh > arg.time),it };
					}
				}
				else if (arg.v) {
					auto button = button_map.at(arg.v == Actions::Vertical::UP ? "U" : "D");
					auto f = button_map.at("F");
					auto b = button_map.at("B");
					auto ret = (isRelease(it, button, arg.time) && !isHold(it, f) && !isHold(it, b)) || isHold(it, button) && (!isPress(it, f) ^ !isPress(it, b));
					return{ ret,it };

				}
				else if (arg.h) {
					auto button = button_map.at(arg.h == Actions::Horizontal::RIGHT ? "F" : "B");
					auto u = button_map.at("U");
					auto d = button_map.at("D");
					auto ret = (isRelease(it, button, arg.time) && !isHold(it, u) && !isHold(it, d)) || isHold(it, button) && (!isPress(it, u) ^ !isPress(it, d));
					return{ ret,it };
				}
				return{ false,it };
			}

			Buffer_t::const_iterator it;
			Buffer_t::const_iterator itend;
			unsigned int margine;
			const std::unordered_map<Mapped_Name, unsigned int>& button_map;
		};
		class verifier : public boost::static_visitor<bool> {
			using R = verifier::result_type;
		public:
			verifier(const std::unordered_map<Mapped_Name, unsigned int>& arg) :button_map(arg) {}
			R operator()(Sequence arg) const {
				for (auto& elem : arg.data) {
					auto ret = boost::apply_visitor(verifier{ button_map }, elem);
					if (!ret) {
						return false;
					}
				}
				return true;
			}

			R operator()(SameTime arg)const {
				for (auto& elem : arg.data) {
					auto ret = boost::apply_visitor(verifier{ button_map }, elem);
					if (!ret) {
						return false;
					}
				}
				return true;
			}

			R operator()(Press arg) const {
				auto ret = button_map.count(arg.button) > 0;
				return ret;
			}


			R operator()(Hold arg) const {
				auto ret = button_map.count(arg.button) > 0;
				return ret;
			}

			R operator()(Release arg) const {
				auto ret = button_map.count(arg.button);
				return ret;
			}

			R operator()(Press_Dir arg) const {
				return true;
			}
			R operator()(Hold_Dir arg) const {
				return true;
			}
			R operator()(Release_Dir arg) const {
				return true;
			}
			const std::unordered_map<Mapped_Name, unsigned int>& button_map;
		};
		class printer : public boost::static_visitor<int> {
			using R = verifier::result_type;
		public:
			printer() {}
			R operator()(Sequence arg) {
				for (auto& elem : arg.data) {
					boost::apply_visitor(*this, elem);
					out += ",";
				}
				out.erase(out.find_last_of(","));
				return 0;
			}

			R operator()(SameTime arg) {
				for (auto& elem : arg.data) {
					boost::apply_visitor(*this, elem);
					out += "+";
				}
				out.erase(out.find_last_of("+"));
				return 0;
			}

			R operator()(Press arg) {
				out += arg.button;
				return 0;
			}


			R operator()(Hold arg) {
				out += "/" + arg.button;
				return 0;
			}

			R operator()(Release arg) {
				out += "~" + std::to_string(arg.time) + arg.button;
				return 0;
			}

			R operator()(Press_Dir arg) {
				auto v = std::string{ arg.v ? (arg.v.get() == Vertical::UP ? "U" : "D") : "" };
				auto h = std::string{ arg.h ? (arg.h.get() == Horizontal::RIGHT ? "F" : "B") : "" };
				out += arg.only ? ">" : "" + v + h;
				return 0;
			}
			R operator()(Hold_Dir arg) {
				auto v = std::string{ arg.v ? (arg.v.get() == Vertical::UP ? "U" : "D") : "" };
				auto h = std::string{ arg.h ? (arg.h.get() == Horizontal::RIGHT ? "F" : "B") : "" };
				out += "/" + std::to_string(arg.time) + v + h;
				return 0;
			}
			R operator()(Release_Dir arg) {
				auto v = std::string{ arg.v ? (arg.v.get() == Vertical::UP ? "U" : "D") : "" };
				auto h = std::string{ arg.h ? (arg.h.get() == Horizontal::RIGHT ? "F" : "B") : "" };
				out += "~" + std::to_string(arg.time) + v + h;
				return 0;
			}
			std::string out;
		};
	}

	class CCommand {
	public:
		Actions::Action data;
		unsigned int margine;
	};

	struct err_no_command {
		std::string command;
	};

	struct err_cmddata {
		std::vector<std::string> command;
	};

	class CEval {
	public:
		using isPressedFunc = std::function<bool(unsigned int)>;

	public:
		CEval(isPressedFunc _isPressed, unsigned int _buffer_size = max_buffer_size) : isPressed(_isPressed), buffer_size(_buffer_size) {
		}

		void Update_Buffer() {
			if (buffer.size() >= buffer_size) {
				buffer.pop_back();
			}

			Buffer_t::value_type pushlist;
			for (auto const & elem : button_map) {
				auto pressflag = isPressed(elem.second);
				auto & prelist =  *buffer.begin();
				if (pressflag) {
					if (prelist.count(elem.second) > 0) {
						pushlist[elem.second] = prelist[elem.second] + 1;
					}
					else {
						pushlist[elem.second] = 0;
					}
				}
				else {
					if (prelist.count(elem.second) > 0 && prelist.at(elem.second) != -1) {
						pushlist[elem.second] = -1;
					}
				}
			}
			buffer.push_front(pushlist);



		}
		bool Eval_Command(std::string name) {
			if (data.count(name) == 0) {
				throw err_no_command{ name };
			}
			auto const & cmd = data.at(name);
			auto ret = boost::apply_visitor(visitors::executer{ buffer.begin(),buffer.end(),button_map,cmd.margine }, cmd.data);
			return ret.first;
		}
		void CEval::Load(const Command::Parser::Cmd& cmd) {
			for (auto & elem : cmd.data) {
				data[elem.name].data = elem.action;
				data[elem.name].margine = elem.time;
			}
			button_map = cmd.map;
			err_cmddata err{};
			for (auto & elem : data) {
				auto verify = boost::apply_visitor(visitors::verifier(cmd.map), elem.second.data);
				if (!verify) {
					err.command.push_back(elem.first);
				}
			}
			if (err.command.size() > 0) {
				throw err;
			}

			//init input history with none
			buffer.push_back(Buffer_t::value_type{});
		}
		std::unordered_map<Command_Name, CCommand> getCmddata() {
			return data;
		}
	private:
		std::unordered_map<Command_Name, CCommand> data;
		std::unordered_map<Mapped_Name, unsigned int> button_map;
		Buffer_t buffer;

	public:
		isPressedFunc isPressed;
		static constexpr unsigned int max_buffer_size = 3000;
		const unsigned int buffer_size;

	};
}