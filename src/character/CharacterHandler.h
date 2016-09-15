/*
 * Copyright (c) 2016 Ember
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <dbcreader/Storage.h>
#include <game_protocol/ResultCodes.h>
#include <shared/database/daos/CharacterDAO.h>
#include <spark/temp/Character_generated.h>
#include <shared/util/PCREHelper.h>
#include <logger/Logging.h>
//#include <boost/locale.hpp>
#include <boost/optional.hpp>
#include <pcre.h>
#include <locale>
#include <functional>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

namespace ember {

class ThreadPool;

class CharacterHandler {
	typedef std::function<void(protocol::ResultCode)> ResultCB;
	typedef std::function<void(protocol::ResultCode, boost::optional<Character>)> RenameCB;
	typedef std::function<void(boost::optional<std::vector<Character>>)> EnumResultCB;

	// todo, should probably be in a config
	const std::size_t MAX_NAME_LENGTH = 12;
	const std::size_t MIN_NAME_LENGTH = 2;
	const std::size_t MAX_CONSECUTIVE_LETTERS = 2;
	const std::size_t MAX_CHARACTER_SLOTS = 10;

	const std::vector<util::pcre::Result>& profane_names_;
	const std::vector<util::pcre::Result>& reserved_names_;
	const dbc::Storage& dbc_;
	const dal::CharacterDAO& dao_;
	const std::locale locale_;

	ThreadPool& pool_;
	log::Logger* logger_;

	protocol::ResultCode validate_name(const std::string& name) const;
	bool validate_options(const Character& character, std::uint32_t account_id) const;

	/** I/O heavy functions run async in a thread pool **/

	void do_create(std::uint32_t account_id, std::uint32_t realm_id,
	               Character character, const ResultCB& callback) const;

	void do_erase(std::uint32_t account_id, std::uint32_t realm_id,
	              std::uint64_t character_id, const ResultCB& callback) const;

	void do_enumerate(std::uint32_t account_id, std::uint32_t realm_id,
	                  const EnumResultCB& callback) const;

	void do_rename(std::uint32_t account_id, std::uint64_t character_id,
	               const std::string& name, const RenameCB& callback) const;

	void do_restore(std::uint64_t id, const ResultCB& callback) const;

public:
	CharacterHandler(const std::vector<util::pcre::Result>& profane_names,
	                 const std::vector<util::pcre::Result>& reserved_names,
	                 const dbc::Storage& dbc, const dal::CharacterDAO& dao,
	                 ThreadPool& pool, const std::locale& locale, log::Logger* logger);

	void create(std::uint32_t account_id, std::uint32_t realm_id,
	            const messaging::character::CharacterTemplate& options, ResultCB callback) const;

	void erase(std::uint32_t account_id, std::uint32_t realm_id, std::uint64_t character_id,
	              ResultCB callback) const;

	void enumerate(std::uint32_t account_id, std::uint32_t realm_id, EnumResultCB callback) const;

	void rename(std::uint32_t account_id, std::uint64_t character_id, const std::string& name,
	            RenameCB callback) const;

	void restore(std::uint64_t id, ResultCB callback) const;
};

} // ember