#pragma once

class Clock
{
public:
	static inline size_t s_globalClock{};

	Clock() = default;

	inline void tickGlobalClock() { s_globalClock++; }
	inline bool triggerLocalEvent() { return ((s_globalClock >= m_localClock) ? true : false); }
	inline void accumulateLocalIncrement(const int inc) { m_localIncrement += inc; }
	inline void resetLocalIncrement() { m_localIncrement = 0; }
	inline void increaseLocalClock() { m_localClock += m_localIncrement; }
private:
	size_t m_localClock{};
	size_t m_localIncrement{};
};