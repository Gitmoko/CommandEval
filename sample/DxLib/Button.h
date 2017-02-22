#ifndef ButtonH
#define ButtonH
#include "Buttonlist.h"
#include<functional>
#include<array>
#include<vector>
#include<map>


class CButton {//Menu‚ÌOption‚Å•Ï‚¦‚ç‚ê‚é‚æ‚¤‚É‚µ‚æ‚¤
public:

	CButton() :Buttonnum_name([]()->std::map < int, std::string > {
		std::map < int, std::string > ret;
		for (int i = 0; i < 20; i++) {
			if (i == VDOWN_BUTTON) { ret[i] = "VDOWN_BUTTON"; }
			else if (i == VLEFT_BUTTON) { ret[i] = "VLEFT_BUTTON"; }
			else if (i == VUP_BUTTON) { ret[i] = "VUP_BUTTON"; }
			else if (i == VRIGHT_BUTTON) { ret[i] = "VRIGHT_BUTTON"; }
			else if (i == DOWN_BUTTON) { ret[i] = "DOWN_BUTTON"; }
			else if (i == LEFT_BUTTON) { ret[i] = "LEFT_BUTTON"; }
			else if (i == UP_BUTTON) { ret[i] = "UP_BUTTON"; }
			else if (i == RIGHT_BUTTON) { ret[i] = "RIGHT_BUTTON"; }
			else if (i == SQUARE_BUTTON) { ret[i] = "SQUARE_BUTTON"; }
			else if (i == TRIANGLE_BUTTON) { ret[i] = "TRIANGLE_BUTTON"; }
			else if (i == CROSS_BUTTON) { ret[i] = "CROSS_BUTTON"; }
			else if (i == CIRCLE_BUTTON) { ret[i] = "CIRCLE_BUTTON"; }
			else if (i == L1_BUTTON) { ret[i] = "L1_BUTTON"; }
			else if (i == R1_BUTTON) { ret[i] = "R1_BUTTON"; }
			else if (i == L2_BUTTON) { ret[i] = "L2_BUTTON"; }
			else if (i == R2_BUTTON) { ret[i] = "R2_BUTTON"; }
			else if (i == R3_BUTTON) { ret[i] = "R3_BUTTON"; }
			else if (i == L3_BUTTON) { ret[i] = "L3_BUTTON"; }
			else if (i == SELECT_BUTTON) { ret[i] = "SELECT_BUTTON"; }
			else if (i == PAUSE_BUTTON) { ret[i] = "PAUSE_BUTTON"; }
		}
		return ret; }()) {}
public:

	~CButton() {}
public:

	typedef int PressedTime;
	typedef int ButtonNumber;//Buttonlisth.h‚Ìƒ}ƒNƒ–¼
	typedef std::map < ButtonNumber, std::pair<std::function<int()>, PressedTime> > Button_map_t;
private:

	const std::map<ButtonNumber, std::string>Buttonnum_name;
	Button_map_t Data;
public:

	/*Container< std::pair<ButtonNumber, std::function<int()>>*/
	template<class Container>
	int Set_button_map(const Container& arg){
		Data.clear();
		for (const auto& elem : arg){
			using tmp = decltype(Data);
			Data.insert(tmp::value_type{ elem.first, {elem.second,0} });
		}
		return 0;
	}

	/*Container< std::pair<ButtonNumber, std::function<int()>>*/
	template<class Container>
	int Update(const Container& arg){
		for (const auto& elem:arg){
			if (elem.second() > 0)
				Data[elem.first].second += 1;
			else
				Data[elem.first].second = 0;
		}
		return 0;
	}
	int Update(){
		for (const auto& elem : Data){
			if (elem.second.first != nullptr &&elem.second.first() > 0)
				Data[elem.first].second += 1;
			else
				Data[elem.first].second = 0;
		}
		return 0;
	}

	int Counter(ButtonNumber buttonlist){
		
		return Data.count(buttonlist)== 1 ? Data[buttonlist].second:0;
	}
	const Button_map_t& Getlist(){ return Data; }
	const std::string * Getname(ButtonNumber index){
		if (Buttonnum_name.count(index) == 0)
			return nullptr;
		return &Buttonnum_name.at(index); 
	}
	int Getbuttonnum(){ return Buttonnum_name.size(); }

};


#endif