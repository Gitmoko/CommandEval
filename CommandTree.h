#pragma once
#include <MyParser_Boost_Cfg.h>
#include<boost/variant/variant.hpp>
#include<boost/optional/optional.hpp>
#include<vector>

namespace Command {

	namespace Actions {

		class SameTime;
		class Press;
		class Sequence;
		class Press;
		class Hold;
		class Release;
		class Press_Dir;
		class Hold_Dir;
		class Release_Dir ;

		using Action = boost::variant<
			Sequence,
			SameTime,
			Press,
			Hold,
			Release,
			Press_Dir,
			Hold_Dir,
			Release_Dir
		>;

		class Sequence {
		public:
			std::vector<Action> data;
		};

		class SameTime {
		public:
			std::vector<Action> data;
		};

		class Press {
		public:
			std::string button;

		};

		class Hold {
		public:
			std::string button;
			int time;

		};

		class Release {
		public:
			std::string button;
			int time;
		};

		enum class Vertical {
			UP,
			DOWN
		};

		enum class Horizontal {
			LEFT,
			RIGHT
		};

		class Press_Dir {
		public:
			boost::optional<Vertical> v;
			boost::optional<Horizontal> h;
			bool only;
		};

		class Hold_Dir {
		public:

			boost::optional<Vertical> v;
			boost::optional<Horizontal> h;
			int time;

		};

		class Release_Dir {
		public:

			boost::optional<Vertical> v;
			boost::optional<Horizontal> h;
			int time;
		};


	}

}