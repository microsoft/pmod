/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODEventHandlerAdapter.h
****/

#include <foundation/ctl/com_library.h>

template <class T,class TEventHandler,class TEventArgs,const IID *pIIdType>
class CEventHandlerAdapter : public foundation::ctl::Implements
	<
		TEventHandler,
		pIIdType
	>
{
public:
	static HRESULT CreateInstance(void *self,TEventHandler **ppEventHandler)
	{
		foundation::ctl::ComObject<CEventHandlerAdapter<T,TEventHandler,TEventArgs,pIIdType> > *pEventHandlerAdapter;
		foundation::ctl::ComObject<CEventHandlerAdapter<T,TEventHandler,TEventArgs,pIIdType> >::CreateInstance(&pEventHandlerAdapter);
		pEventHandlerAdapter->_self = self;
		*ppEventHandler = static_cast<TEventHandler *>(pEventHandlerAdapter);
		return S_OK;
	}
	
	void Dispose()
	{
		_self = nullptr;
	}
protected:
	STDMETHOD(Invoke)(foundation::IUnknown* pSender, TEventArgs* pArgs)
	{
		if(_self)
		{
            @try {
                T::fireEventArgs(_self,pSender,pArgs);
            }
            @catch(PMODException *pmodException)
            {
                return pmodException.error;
            }
		}
		
		return S_OK;
	}
private:
	void *_self;
};
