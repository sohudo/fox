#ifndef _events_EventDispatcher_H_
#define _events_EventDispatcher_H_

#include "Delegate.h"
#include <list>
#include <map>
#include <string>

class CEventDispatcher
{
public:
	CEventDispatcher(void);
	~CEventDispatcher(void);
	
	void addEventListener(const std::string &type, IDelegate *fun);
	void removeEventListener(const std::string &type, IDelegate *fun);
	void removeEventListeners(const std::string &type);
	void removeEventListeners();

	void dispatchEvent(const std::string &type, unsigned int data);

private:
	typedef std::list<IDelegate *>           EVT_LIST;
	typedef std::map<std::string, EVT_LIST > EVT_MAP;
	
	EVT_MAP m_mapEvts;
};

#endif // _events_EventDispatcher_H_
