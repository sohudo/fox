#include "StdAfx.h"
#include <algorithm>
#include ".\eventdispatcher.h"
#include <iostream>
using namespace std;

CEventDispatcher::CEventDispatcher(void)
{
} // CEventDispatcher

CEventDispatcher::~CEventDispatcher(void)
{
	removeEventListeners();
} // ~CEventDispatcher

void CEventDispatcher::addEventListener(const std::string &type, IDelegate *fun)
{
	if (type == "" || fun == 0)
	{
		return;
	}

	EVT_LIST &lt            = m_mapEvts[type];
	EVT_LIST::iterator ltIt = lt.begin();

	for (; ltIt != lt.end(); ++ltIt)
	{
		IDelegate *tmp = (*ltIt);

		if ((*tmp == *fun))
		{
			return;
		}
	}

	lt.push_back(fun);

} // addEventListener

void CEventDispatcher::removeEventListener(const std::string &type, IDelegate *fun)
{
	if (type == "" || fun == 0)
	{
		return;
	}

	EVT_MAP::iterator mapIt = m_mapEvts.find(type);

	if (mapIt == m_mapEvts.end())
	{
		return;
	}

	EVT_LIST &lt            = mapIt->second;
	EVT_LIST::iterator ltIt = lt.begin();

	for (; ltIt != lt.end(); ++ltIt)
	{
		IDelegate *tmp = (*ltIt);

		if ((*tmp == *fun))
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

void CEventDispatcher::removeEventListeners(const std::string &type)
{
	if (type == "")
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

void CEventDispatcher::removeEventListeners()
{
	m_mapEvts.clear();
} // removeEventListeners

void CEventDispatcher::dispatchEvent(const std::string &type, unsigned int data)
{
	if (type == "")
	{
		return;
	}

	EVT_MAP::iterator mapIt = m_mapEvts.find(type);

	if (mapIt == m_mapEvts.end())
	{
		return;
	}

	EVT_LIST &lt            = mapIt->second;
	EVT_LIST::iterator ltIt = lt.begin();
	EVT_LIST::iterator ltItCur;

	for (; ltIt != lt.end();)
	{
		ltItCur = ltIt++;

		if ((*ltItCur)->invoke(data))
		{
			break;
		}
	}
} // dispatchEvent