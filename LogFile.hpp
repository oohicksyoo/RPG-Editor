//
// Created by Alex on 1/25/2021.
//

#pragma once

#include <string>

namespace RPG {
	enum LogFileType {
		NormalLog,
		Warning,
		Error
	};

	struct LogFile {
		public:
			LogFileType type;
			std::string caller;
			std::string message;
			bool isLua;
	};
}
