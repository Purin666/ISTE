#pragma once

namespace MurmurHash {


	//-----------------------------------------------------------------------------
	// MurmurHash2, 64-bit versions, by Austin Appleby

	// The same caveats as 32-bit MurmurHash2 apply here - beware of alignment 
	// and endian-ness issues if used across multiple platforms.

	typedef unsigned __int64 uint64_t;

	// 64-bit hash for 64-bit platforms

	static uint64_t MurmurHash2(const void* key, int len, unsigned int seed)
	{
		const uint64_t m = 0xc6a4a7935bd1e995;
		const int r = 47;

		uint64_t h = seed ^ (len * m);

		const uint64_t* data = (const uint64_t*)key;
		const uint64_t* end = data + (len / 8);

		while (data != end)
		{
			uint64_t k = *data++;

			k *= m;
			k ^= k >> r;
			k *= m;

			h ^= k;
			h *= m;
		}

		const unsigned char* data2 = (const unsigned char*)data;

		switch (len & 7)
		{
		case 7: h ^= uint64_t(data2[6]) << 48;
		case 6: h ^= uint64_t(data2[5]) << 40;
		case 5: h ^= uint64_t(data2[4]) << 32;
		case 4: h ^= uint64_t(data2[3]) << 24;
		case 3: h ^= uint64_t(data2[2]) << 16;
		case 2: h ^= uint64_t(data2[1]) << 8;
		case 1: h ^= uint64_t(data2[0]);
			h *= m;
		};

		h ^= h >> r;
		h *= m;
		h ^= h >> r;

		return h;
	}

	template<class Key>
	static uint64_t Hash(Key& aKey)
	{
		return MurmurHash2(&aKey, sizeof(Key), 3984759834);
	}

	template<>
	static uint64_t Hash(const std::string& aKey)
	{
		return MurmurHash2(aKey.c_str(), aKey.size(), 3984759834);
	}
	template<>
	static uint64_t Hash(const std::wstring& aKey)
	{
		return MurmurHash2(aKey.c_str(), aKey.size(), 3984759834);
	}
}