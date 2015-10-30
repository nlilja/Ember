/*
 * Copyright (c) 2014, 2015 Ember
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <botan/secmem.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>

namespace ember {

typedef std::vector<char> Packet;

template<typename T>
class PacketStream {
	T* buffer_;

public:
	PacketStream(T* buffer) : buffer_(buffer) {}

	inline PacketStream& operator <<(const Botan::SecureVector<Botan::byte>& data) {
		for(auto b : data) {
			*this << b;
		}

		return *this;
	}

	inline PacketStream& operator <<(const std::string& data) {
		std::copy(data.begin(), data.end(), std::back_inserter(*buffer_));
		return *this;
	}

	template<typename U>
	inline PacketStream& operator <<(const U& data) {
		const char* copy = static_cast<const char*>(static_cast<const void*>(&data));
		std::copy(copy, copy + sizeof(data), std::back_inserter(*buffer_));
		return *this;
	}

	inline T* swap(T* buffer) {
		T* old = buffer_;
		buffer_ = buffer;
		return old;
	}
};

} // ember