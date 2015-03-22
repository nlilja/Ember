/*
 * Copyright (c) 2015 Ember
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "GameVersion.h"
#include "LoginHandler.h"
#include <logger/Logging.h>
#include <shared/database/daos/UserDAO.h>

namespace ember {

class LoginHandlerBuilder {
	log::Logger* logger_;
	const Patcher& patcher_;
	dal::UserDAO& user_dao_;

public:
	LoginHandlerBuilder(log::Logger* logger, const Patcher& patcher, dal::UserDAO& user_dao)
	                    : logger_(logger), patcher_(patcher), user_dao_(user_dao) {}

	LoginHandler create(const std::string& source) {
		return LoginHandler(source, user_dao_, patcher_, logger_);
	}
};

} // ember