/*
 * Copyright (c) 2015 Ember
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include "Actions.h"
#include "PacketBuffer.h"
#include "Authenticator.h"
#include "GameVersion.h"
#include "Protocol.h"
#include <logger/Logging.h>
#include <shared/database/daos/UserDAO.h>
#include <shared/misc/PacketStream.h>
#include <botan/bigint.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace ember {

class Patcher;

class LoginHandler {
	enum class State {
		INITIAL_CHALLENGE, LOGIN_PROOF, RECONN_PROOF, REQUEST_REALMS,
		FETCHING_USER, FETCHING_SESSION, WRITING_SESSION, CLOSED
	};

	State state_ = State::INITIAL_CHALLENGE;
	log::Logger* logger_;
	const Patcher& patcher_;
	dal::UserDAO& user_src_;
	boost::optional<User> user_;
	Botan::BigInt server_proof_;
	std::string source_, username_;
	std::unique_ptr<LoginAuthenticator> login_auth_;
	std::unique_ptr<ReconnectAuthenticator> reconn_auth_;

	void send_realm_list(const PacketBuffer& buffer);
	void process_challenge(const PacketBuffer& buffer);
	void check_login_proof(PacketBuffer& buffer);
	void send_reconnect_proof(const PacketBuffer& buffer);
	void send_login_failure(protocol::ResultCodes result);
	void build_login_challenge(PacketStream<Packet>& resp);
	void send_login_challenge(FetchUserAction* action);
	void send_login_success(StoreSessionAction* action);
	void send_reconnect_challenge(FetchSessionKeyAction* action);

	void accept_client(protocol::ClientOpcodes opcode);
	void reject_client(const GameVersion& version);
	void patch_client(const GameVersion& version);

	bool check_opcode(const PacketBuffer& buffer, protocol::ClientOpcodes opcode);

public:
	std::function<void(std::shared_ptr<Action> action)> on_action;
	std::function<void(std::shared_ptr<Packet>)> on_send;

	bool update_state(std::shared_ptr<Action> action);
	bool update_state(PacketBuffer& buffer);

	LoginHandler(std::string source, dal::UserDAO& users, const Patcher& patcher, log::Logger* logger)
	             : source_(std::move(source)), user_src_(users), patcher_(patcher), logger_(logger) {}
	LoginHandler& operator=(LoginHandler&&);
	LoginHandler(LoginHandler&&);
};

} // ember