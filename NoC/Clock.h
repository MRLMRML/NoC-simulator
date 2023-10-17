#pragma once

class Clock
{
public:
	static inline float s_globalClock{ 0.0f };

	Clock() = default;

	inline void tickGlobalClock() { s_globalClock++; }
	inline void tickLocalClock() { m_localClock++; }
	inline void tickLocalClock(const float cycles) { m_localClock += cycles; }
	inline void tickLocalClock(const float cycles) const { m_localClock += cycles; }
	inline bool triggerLocalEvent() { return ((s_globalClock >= m_localClock) ? true : false); }
	inline bool triggerLocalEvent() const { return ((s_globalClock >= m_localClock) ? true : false); }
	inline void synchronizeClock() { if (s_globalClock == m_localClock) m_localClock++; }
	inline void synchronizeClock() const { if (s_globalClock == m_localClock) m_localClock++; }

	inline void accumulateLocalIncrement(const float inc) { m_localIncrement += inc; }
	inline void increaseLocalClock() { m_localClock += m_localIncrement; }
	inline void resetLocalIncrement() { m_localIncrement = 0.0f; }
private:
	mutable float m_localClock{ 0.0f };
	float m_localIncrement{ 0.0f };
};