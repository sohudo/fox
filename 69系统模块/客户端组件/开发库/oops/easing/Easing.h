#ifndef _Easing_H_
#define _Easing_H_
#include <cmath>
#include <vector>


#define X_PI       3.14159265358979323846
#define X_PI_2     1.57079632679489661923
#define X_PI_4     0.785398163397448309616

namespace easing
{
	// 缓动函数
	typedef float (*EASING_FUNC)(float rate);

	// 线性缓动
	inline float Linear(float r)
	{
		return r;
	} // Linear

	inline float Reverse(float r)
	{
		return 1 - r;
	} // Reverse

	inline float QuadIn(float r)
	{
		return std::powf(r, 2);
	} // QuadIn

	inline float QuadOut(float r)
	{
		return 1 - std::powf((r - 1), 2);
	} // QuadOut

	inline float QuadInOut(float r)
	{
		if ((r *= 2) < 1)
		{
			return 0.5f * std::powf(r, 2);
		}

		return -0.5f * ((r -= 2) * r - 2);
	} // QuadInOut

	inline float CubicIn(float r)
	{
		return std::powf(r, 3);
	} // CubicIn

	inline float CubicOut(float r)
	{
		return std::powf((r - 1), 3) + 1;
	} // float

	inline float CubicInOut(float r)
	{
		if ((r *= 2) < 1)
		{
			return 0.5f * std::powf(r, 3);
		}

		return 0.5f * (std::sqrtf(1 - (r -= 2) * r) + 1);
	} // CubicInOut

	inline float QuartIn(float r)
	{
		return std::powf(r, 4);
	} // QuartIn

	inline float QuartOut(float r)
	{
		return 1 - std::powf((r - 1), 4);
	} // QuartOut

	inline float QuartInOut(float r)
	{
		if ((r *= 2) < 1)
		{
			return 0.5f * std::powf(r, 4);
		}

		return -0.5f * ((r -= 2) * std::powf(r, 3) - 2);
	}

	inline float QuintIn(float r)
	{
		return std::powf(r, 5);
	} // QuintInt

	inline float QuintOut(float r)
	{
		return std::powf((r - 1), 5) + 1;
	} // QuintOut

	inline float QuintInOut(float r)
	{
		if ((r *= 2) < 1)
		{
			return 0.5f * std::powf(r, 5);
		}

		return 0.5f * (std::powf((r - 2), 5) + 2);
	} // QuintInOut

	inline float SineIn(float r)
	{
		return 1 - std::cosf(r * (float)X_PI_2);
	} // SineIn

	inline float SineOut(float r)
	{
		return std::sinf(r * (float)X_PI_2);
	} // SineOut

	inline float SineInOut(float r)
	{
		return -0.5f * (std::cosf((float)X_PI * r) - 1);
	} // SineInOut

	inline float ExpoIn(float r)
	{
		return ((r == 0) ? 0 : std::powf(2, 10 * (r - 1)));
	} // ExpoIn

	inline float ExpoOut(float r)
	{
		return ((r == 1) ? 1 : -std::powf(2, -10 * r)) + 1;
	} // ExpoOut

	inline float ExpoInOut(float r)
	{
		if (r == 0) return 0;
		if (r == 1) return 1;

		if ((r *= 2) < 1)
		{
			return 0.5f * std::powf(2, 10 * (r - 1));
		}

		return 0.5f * (-std::powf(2, -10 * (--r)) + 2);
	} // ExpoInOut

	inline float CircIn(float r)
	{
		return 1 - std::sqrtf(1 - r * r);
	} // CircIn

	inline float CircOut(float r)
	{
		return std::sqrtf(1 - std::powf((r - 1), 2));
	} // CircOut

	inline float CircInOut(float r)
	{
		if ((r *= 2) < 1)
		{
			return -0.5f * (std::sqrtf(1 - r * r) - 1);
		}

		return 0.5f * (std::sqrtf(1 - (r -= 2) * r) + 1);
	} // CircInOut

	inline float BounceOut(float r)
	{
		if ((r) < (1 / 2.75f)) 
		{
			return (7.5625f * r * r);
		} 
		else if (r < (2 / 2.75f)) 
		{
			return (7.5625f * (r -= (1.5f / 2.75f)) * r + .75f);
		} 
		else if (r < (2.5f / 2.75f)) 
		{
			return (7.5625f * (r -= (2.25f / 2.75f)) * r + .9375f);
		} 

		return (7.5625f * (r -= (2.625f / 2.75f)) * r + .984375f);
	} // BounceOut

	inline float BounceIn(float r)
	{
		return 1 - BounceOut(1 - r);
	} // BounceIn

	inline float BounceInOut(float r)
	{
		if (r < 0.5)
		{
			return BounceIn(r * 2) * 0.5f;
		}

		return BounceOut(r * 2 - 1) * 0.5f + 0.5f;
	} // BounceInOut

	inline float BouncePast(float r) 
	{
		if ((r) < (1 / 2.75f)) 
		{
			return (7.5625f * r * r);
		} 
		else if (r < (2 / 2.75f)) 
		{
			return 2 - (7.5625f * (r -= (1.5f / 2.75f)) * r + .75f);
		} 
		else if (r < (2.5f / 2.75f)) 
		{
			return 2 - (7.5625f * (r -= (2.25f / 2.75f)) * r + .9375f);
		} 

		return 2 - (7.5625f * (r -= (2.625f / 2.75f)) * r + .984375f);
	} // BouncePast

	inline float BackIn(float r)
	{
		float s = 1.70158f;
		return r * r * ((s + 1) * r - s);
	}

	inline float BackOut(float r)
	{
		float s = 1.70158f;
		return (r -= 1) * r * ((s + 1) * r + s) + 1;
	}

	inline float BackInOut(float r)
	{
		float s = 1.70158f;

		if ((r *= 2) < 1)
		{
			return 0.5f * r * r * (((s *= 1.525f) + 1) * r - s);
		}

		return 0.5f * ((r -= 2) * r * (((s *= 1.525f) + 1) * r + s) + 2);
	} // BackInOut

	inline float Elastic(float r)
	{
		return 1 - std::powf(4, -8 * r) * std::sinf((r * 6 - 1) * 2 * (float)X_PI_2);
	} // Elastic

	inline float Mirror(float r, EASING_FUNC fnEase = NULL)
	{
		fnEase = fnEase == NULL ? SineInOut : fnEase;
		return (r < 0.5f) ? fnEase(r * 2) : fnEase(1 - (r - 0.5f) * 2);
	} // Mirror

	inline float Flicker(float r)
	{
		r = r + (rand() / (float)RAND_MAX - 0.5f) / 5;
		return SineInOut(r < 0 ? 0 : (r > 1 ? 1 :  r));
	} // Flicker

	inline float Wobble(float r)
	{
		return 0.5f - std::cosf(r * (float)X_PI * 9 * r) * 0.5f; 
	} // Wobble

	inline float Spring(float r)
	{
		return 1 - (std::cosf(r * 4.5f * (float)X_PI) * expf(-r * 6));
	} // Spring

	template<typename T>
		inline T Easing(T start, T end, float rate, EASING_FUNC ease = Linear)
	{
		rate = max(min(1.0f, rate), 0.0f);
		ease = ease == NULL ? Linear : ease;
		return T(start + (end - start) * ease(rate));
	} // Easing

	/** 属性缓动 */
	template<class T>
	struct STweenData
	{
		T from;
		T to;
		EASING_FUNC fnEase;
		STweenData(T from, T to, EASING_FUNC fnEase)
			: from(from), to(to), fnEase(fnEase)
		{}
	}; // STweenData

	/** 缓动类 */
	template<class T>
	class CTween
	{
	public:
		void Clear()
		{
			m_properties.clear();
			m_rets.clear();
		} // Clear

		void Add(const T& from, const T& to, EASING_FUNC fnEase = Linear)
		{
			m_properties.push_back(STweenData<T>(from, to, fnEase));
			m_rets.push_back(from);
		} // Add
		void Start(float duration, DWORD dwDelay = 0)
		{
			m_dwStart = GetTickCount();

			if (duration <= 0)
			{
				duration = 0.001f;
			}

			m_invDuration = 1 / duration;
			m_fRate		  = 0;
			m_dwDelay     = dwDelay <= 0 ? 0 : dwDelay;
		} // FromTo

		void EndDelay()
		{
			if (m_dwDelay != 0)
			{
				m_dwDelay = 0;
				m_dwStart = GetTickCount();
			}
		}

		bool Update()
		{
			if (m_dwDelay != 0)
			{
				if (GetTickCount() - m_dwStart >= m_dwDelay)
				{
					EndDelay();
				}

				return false;
			}


			if (m_fRate == 1)
			{
				return true;
			}


			DWORD dwDelta = GetTickCount() - m_dwStart;
			m_fRate    = dwDelta * 0.001f * m_invDuration;

			if (m_fRate >= 1)
			{
				for (int i = 0, l = (int)m_properties.size(); i < l; ++i)
				{
					m_rets[i] = m_properties[i].to;
				}

				m_fRate = 1;
			}
			else
			{
				for (int i = 0, l = (int)m_properties.size(); i < l; ++i)
				{
					m_rets[i] = Easing(m_properties[i].from, m_properties[i].to, m_fRate, m_properties[i].fnEase);
				}
			}

			return m_fRate == 1;
		} // Update

		T GetValue(int index) const
		{
			return m_rets[index];
		} // GetValue

		int GetSize() const
		{
			return (int)m_rets.size();
		}

		bool IsDelaying() const
		{
			if (m_dwDelay != 0)
			{
				return (GetTickCount() - m_dwStart < m_dwDelay);
			}

			return false;
		}
	private:
		float			m_invDuration;
		float			m_fRate;
		DWORD			m_dwStart;
		DWORD			m_dwDelay;
		std::vector<T>	m_rets;
		std::vector<STweenData<T> >	m_properties;
	}; // CTween
}; // easing

#endif // _Easing_H_