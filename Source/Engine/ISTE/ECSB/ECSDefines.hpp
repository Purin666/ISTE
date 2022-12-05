#pragma once
#include <bitset>

typedef unsigned int EntityIndex;
typedef unsigned int EntityVersion;
typedef unsigned int ComponentID;
typedef unsigned long long EntityID;

const int MAX_COMPONENTS = 64;

typedef std::bitset<MAX_COMPONENTS> ComponentMask;

inline EntityID CreateEntityId(EntityIndex aIndex, EntityVersion aVersion)
{
	return ((EntityID)aIndex << 32) | ((EntityID)aVersion);
}
inline EntityIndex GetEntityIndex(EntityID aId)
{
	return aId >> 32;
}
inline EntityVersion GetEntityVersion(EntityID aId)
{
	return (EntityVersion)aId;
}
inline bool IsEntityIndexValid(EntityID aId)
{
	return (aId >> 32) != EntityIndex(-1);
}

#define MAX_ENTITIES 10000
#define INVALID_ENTITY CreateEntityId(EntityIndex(-1), 0)
