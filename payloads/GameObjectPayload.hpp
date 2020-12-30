//
// Created by Alex on 12/30/2020.
//

#pragma once

#include "../engine/core/GameObject.hpp"
#include <string>
#include <memory>

namespace RPG {
	struct GameObjectPayload {
		public:
			std::string guid;
			std::shared_ptr<RPG::GameObject> gameObject;
	};
}


