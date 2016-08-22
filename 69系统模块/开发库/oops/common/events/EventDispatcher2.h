#ifndef _events_EventDispatcher2_H_
#define _events_EventDispatcher2_H_

#include <list>
#include <map>
//#include <algorithm>
class CEventDispatcher2;

class IEventHandler
{
public:
	IEventHandler(int iOrder = 0):m_iEventOrder(iOrder){}
	virtual ~IEventHandler(){}
	virtual void SetEOrder(int iOrder){ m_iEventOrder = iOrder; }
	virtual int  GetEOrder() const { return m_iEventOrder; }
	virtual unsigned int OnEvent(CEventDispatcher2* dispatcher, int type, unsigned int data) = 0;

	bool operator< (const IEventHandler& rhs) const
	{
		return GetEOrder() < rhs.GetEOrder();
	}

protected:
	int m_iEventOrder;
}; // IEventHandler

inline bool SortHandlers(IEventHandler* lhs, IEventHandler* rhs)
{
	return lhs->GetEOrder() < rhs->GetEOrder();
}

class CEventDispatcher2
{
public:
	CEventDispatcher2(void)
	{
	} // CEventDispatcher2

	~CEventDispatcher2(void)
	{
		removeEventListeners();
	} // ~CEventDispatcher2


	void addEventListener(int type, IEventHandler* handler)
	{
		if (type == 0 || handler == 0)
		{
			return;
		}

		EVT_HANDLES &lt            = m_mapEvts[type];
		EVT_HANDLES::iterator ltIt = lt.begin();

		for (; ltIt != lt.end(); ++ltIt)
		{
			IEventHandler* tmp = (*ltIt);

			if (tmp == handler)
			{
				lt.erase(ltIt);
				break;
			}
		}

		lt.push_back(handler);

		lt.sort(SortHandlers);
	} // addEventListener

	void removeEventListener(int type, IEventHandler* handler)
	{
		if (type == 0 || handler == 0)
		{
			return;
		}

		EVT_MAP::iterator mapIt = m_mapEvts.find(type);

		if (mapIt == m_mapEvts.end())
		{
			return;
		}

		EVT_HANDLES &lt            = mapIt->second;
		EVT_HANDLES::iterator ltIt = lt.begin();

		for (; ltIt != lt.end(); ++ltIt)
		{
			IEventHandler* tmp = (*ltIt);

			if (tmp == handler)
			{
				lt.erase(ltIt);
				break;
			}
		}

		if (lt.size() == 0)
		{
			m_mapEvts.erase(mapIt);
		}

	} // removeEventListener

	void removeEventListeners(int type)
	{
		if (type == 0)
		{
			return;
		}

		EVT_MAP::iterator mapIt = m_mapEvts.find(type);

		if (mapIt == m_mapEvts.end())
		{
			return;
		}

		m_mapEvts.erase(mapIt);
	} // removeEventListeners

	void removeEventListeners()
	{
		m_mapEvts.clear();
	} // removeEventListeners

	void dispatchEvent(int type, unsigned int data)
	{
		if (type == 0)
		{
			return;
		}

		EVT_MAP::iterator mapIt = m_mapEvts.find(type);

		if (mapIt == m_mapEvts.end())
		{
			return;
		}

		EVT_HANDLES &lt            = mapIt->second;
		EVT_HANDLES::iterator ltIt = lt.begin();
		EVT_HANDLES::iterator ltItCur;

		for (; ltIt != lt.end();)
		{
			ltItCur = ltIt++;

			if ((*ltItCur)->OnEvent(this, type, data))
			{
				break;
			}
		}
	} // dispatchEvent
private:
	typedef std::list<IEventHandler*>	EVT_HANDLES;
	typedef std::map<int, EVT_HANDLES>	EVT_MAP;
	
	EVT_MAP m_mapEvts;
};

#endif // _events_EventDispatcher2_H_
