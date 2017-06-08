/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollection.cpp
****/

#include "pch.h"


#include "ObservableCollection.h"
#include "CreateFoundationHelper.h"
#include "PropertyTypeInfo.h"

// from model API
#include "../model/ModelTypeInfoStatics.h"

#include "FastItemsContainer.h"

#include <pmod/errors.h>
#include <foundation/pv_util.h>
#include <foundation/library/foundation_adapter_base.h>
#include <foundation/library/iterator_impl.h>
#include <foundation/library/util_stl.h>

#include <foundation/library/perf_counter_util.h>
#if defined(DEBUG)
#include <pmod/perf_type_info.h>
#endif

// {7C83B728-89E3-4F96-AF19-33EE4419EDD1}
const GUID pmod::library::IID_IObservableCollectionClassFactory =
{ 0x7c83b728, 0x89e3, 0x4f96,{ 0xaf, 0x19, 0x33, 0xee, 0x44, 0x19, 0xed, 0xd1 } };

using namespace pmod;
using namespace pmod::library;
using namespace foundation;
using namespace foundation::library;

template <class vector_t>
void CopyItems(
    const foundation::InspectableArrayWrapper& array_items_wrapper,
    vector_t& items)
{
    items.resize(array_items_wrapper.GetSize());
    // copy wrapper items on null allocated values
    for (UINT32 index = 0; index < array_items_wrapper.GetSize(); ++index)
    {
        items[index] = array_items_wrapper[index];
    }
}
struct _FuncComparer : public CInspectablePtrItemsContainerBase::IContextComparer
{
    _FuncComparer(pmod::library::CompareItemValueFunc compareItemValueFunc,void *Context):
        _compareItemValueFunc(compareItemValueFunc),
        _context(Context)
    {
    }
protected:
    int compare(const foundation::InspectablePtr& item1,const foundation::InspectablePtr& item2) const override
    {
        return _compareItemValueFunc(item1,item2,_context);
    }
private:
    pmod::library::CompareItemValueFunc _compareItemValueFunc;
    void *_context;
};

struct _IVoid
{
};
typedef vector_items_container<foundation::InspectablePtr,_IVoid> inspectable_vector_items_container_type;

//------------------------------------------------------------------------------
// Class:   CObservableCollectionEventSourceAdapter
// IEventSourceClass implementation for a CObservableCollection class
//------------------------------------------------------------------------------
class CObservableCollectionEventSourceAdapter :
    public foundation::AdapterBase
    <
    pmod::library::IObservableCollectionClass,
    foundation::library::IEventSourceClass,
    &foundation::library::IID_IEventSourceClass
    >
{
protected:
      STDMETHOD(AddEventHandler)(
          _In_ GUID eventSourceType,
          _In_opt_ foundation::IUnknown *pEventHandler,
          _In_opt_ foundation::IInspectable *pOptions,
          _COM_Outptr_ foundation::IInspectable **ppResult)
      {
          // lock before attempting a add any handler
          CObservableCollection::_FoundationBaseLock csl(*this->GetObservableCollectionClass());

          _IFR_(GetObservableCollectionClass()->_AddEventHandlerInternal(eventSourceType, pEventHandler));
          _IFR_(GetObservableCollectionClass()->_ReturnAddHandlerWithOptions(pOptions, ppResult));
          return S_OK;
      }

      STDMETHOD(RemoveEventHandler)(
          _In_ GUID eventSourceType,
          _In_ foundation::IUnknown *pEventHandler)
      {
          return GetObservableCollectionClass()->_RemoveEventHandlerInternal(eventSourceType, pEventHandler);
      }
private:
    CObservableCollection *GetObservableCollectionClass()
    {
        return static_cast<CObservableCollection *>(this->GetInner());
    }
};

HRESULT RegisterFactoryObservableCollectionClassAdapters(
    foundation::library::IGenericAdapterFactory *pGenericAdapterFactory)
{
    foundation_assert(pGenericAdapterFactory != nullptr);
    pGenericAdapterFactory->AddAdapterEntry(
        IID_IObservableCollection,
        foundation::library::IID_IEventSourceClass,
        pmod::library::IID_IObservableCollectionClass,
        foundation::CreateAdapter<CObservableCollectionEventSourceAdapter>);
    return S_OK;
}

class CObservableCollectionValueConverterCallback:
    public foundation::ctl::ComBase,
    public pmod::library::IValueConverterCallback
{
public:
    HRESULT _Initialize(CObservableCollection *pObservableCollection)
    {
        foundation_assert(pObservableCollection != nullptr);
        _pCollectionModel = pObservableCollection;
        foundation::ctl::AddRefInterface(pObservableCollection);
        return S_OK;
    }
protected:
    void FinalRelease() override
    {
        foundation::ctl::ReleaseInterface(_pCollectionModel);
        foundation::ctl::ComBase::FinalRelease();
    }
    // Interface IValueConverterCallback
    STDMETHOD(Invoke)(
        _In_ foundation::IInspectable* pValue,
        _COM_Outptr_ foundation::IInspectable **ppValue) override
    {
        return _pCollectionModel->ConvertItemValue(
            foundation::NINDEX,
            pValue,
            false,
            ppValue);
    }
private:
    CObservableCollection *_pCollectionModel;
};
//------------------------------------------------------------------------------
// Class:   CCollectionModelIterator
// foundation::IIterator_impl<IInspectable*> implementation using the Items Container implementation
//------------------------------------------------------------------------------
class CCollectionModelIterator :
    public foundation::ctl::ComInspectableBase,
    public foundation::IIterator_impl<foundation::IInspectable*>
{
public:
    static HRESULT CreateInstance(
        CObservableCollection *pObservableCollection,
        foundation::IIterator_impl<foundation::IInspectable*> *pItemsContainerIterator,
        foundation::IIterator_impl<foundation::IInspectable*> **ppIterator)
    {
        foundation_assert(pObservableCollection);
        foundation_assert(pItemsContainerIterator);

        CCollectionModelIterator *pIterator = nullptr;
        foundation::ctl::ComInspectableObject<CCollectionModelIterator>::CreateInstance(&pIterator);
        pIterator->_pCollectionModel = pObservableCollection;
        pIterator->_pItemsContainerIterator = pItemsContainerIterator;
        // add ref
        foundation::ctl::inspectable::AddRefInterface(pObservableCollection);
        pItemsContainerIterator->AddRef();

        *ppIterator = pIterator;
        return S_OK;
    }
protected:
    CCollectionModelIterator() :
        _indexIterator(0),
        _pCollectionModel(nullptr),
        _pItemsContainerIterator(nullptr)
    {}
    void FinalRelease()
    {
        // release used interafces
        RELEASE_INTERFACE(_pItemsContainerIterator);
        foundation::ctl::inspectable::ReleaseInterface(_pCollectionModel);
    }

    STDMETHOD(GetCurrent)(_Outptr_ foundation::IInspectable* *current)
    {
        IFCPTR_ASSERT(current);
        foundation::InspectablePtr spItem;
        IFR_ASSERT(_pItemsContainerIterator->GetCurrent(spItem.GetAddressOf()));

        return _pCollectionModel->ConvertItemValue(_indexIterator,spItem, false, current);
    }

    STDMETHOD(GetHasCurrent)(_Outptr_ bool *hasCurrent)
    {
        return _pItemsContainerIterator->GetHasCurrent(hasCurrent);
    }

    STDMETHOD(MoveNext)(_Outptr_ bool *hasCurrent)
    {
        _IFR_(_pItemsContainerIterator->MoveNext(hasCurrent));
        ++_indexIterator;
        return S_OK;
    }
private:
    UINT32          _indexIterator;
    CObservableCollection *_pCollectionModel;
    foundation::IIterator_impl<foundation::IInspectable*> *_pItemsContainerIterator;
};
//------------------------------------------------------------------------------
// Class:   CVectorItemsContainer
// IItemsContainer implementation using std::vector
//------------------------------------------------------------------------------
class CVectorItemsContainer :
    public foundation::ctl::ComInspectableBase,
    public pmod::library::IItemsContainer
{
private:
    friend class CObservableCollection;

    inspectable_vector_items_container_type _container;
    STDMETHOD(GetCount)(_Outptr_ UINT32 *pSize)
    {
        *pSize = _container.getCount();
        return S_OK;
    }

    STDMETHOD(GetItems)(_Outptr_ UINT32 *pSize,foundation::IInspectable ***items)
    {
        ContainerRefCountToArray(
            _container.getItems(),
            pSize,
            items);
        return S_OK;
    }

    STDMETHOD(Get)(_In_ UINT32 index,_COM_Outptr_ foundation::IInspectable **value)
    {
        * value = _container.get(index);
        if(*value)
        {
            (*value)->AddRef();
        }
        return S_OK;
    }
    STDMETHOD(Set)(_In_ UINT32 index, _In_ foundation::IInspectable *value)
    {
        _container.set(index,value);
        return S_OK;
    }
    STDMETHOD(Append)(_In_ foundation::IInspectable *value)
    {
        _container.append(value);
        return S_OK;
    }
    STDMETHOD(Insert)(_In_ UINT32 index,_In_ foundation::IInspectable *value)
    {
        _container.insert(index,value);
        return S_OK;
    }

    STDMETHOD(Remove)(_In_ foundation::IInspectable *value) 
    {
        foundation_assert(false);
        return E_NOTIMPL;
    }

    STDMETHOD(RemoveAt)(_In_ UINT32 index)
    {
        _container.removeAt(index);
        return S_OK;
    }

    STDMETHOD(InsertPositionOf)(
        _In_ foundation::IInspectable *value,
        _In_ pmod::library::CompareItemValueFunc compare,
        _In_ void *Context,
        _Outptr_ UINT32 *pIndex)
    {
        foundation_assert(pIndex);

        _FuncComparer _funcComparer(compare,Context);
        *pIndex = _container.insertPositionOf(value,&_funcComparer);
        return S_OK;
    }

    STDMETHOD(Find)(
        _In_ foundation::IInspectable *value,
        _In_ pmod::library::CompareItemValueFunc compare,
        _In_ void *Context,
        _Outptr_ UINT32 *pIndex)
    {
        foundation_assert(pIndex);
        _FuncComparer _funcComparer(compare,Context);
        *pIndex = _container.find(value,&_funcComparer);
        return S_OK;
    }

    STDMETHOD(Sort)(_In_ pmod::library::CompareItemValueFunc compare,_In_ void *Context)
    {
        _FuncComparer _funcComparer(compare,Context);
        _container.sort(&_funcComparer);
        return S_OK;
    }
    STDMETHOD(Clear)()
    {
        _container.clear();
        return S_OK;
    }
    STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable*> **first)
    {
        typedef foundation::library::_IteratorImpl<
            std::vector<foundation::InspectablePtr>,
            foundation::IInspectable *,
            foundation::_InspectableConverter> _VectorInspectableIterator_Type;
        IFR_ASSERT(foundation::ctl::inspectable::CreateInstanceWithInitialize<_VectorInspectableIterator_Type>(
            first,
            this->CastToInspectable(),
            _container.getItems()));
        return S_OK;
    }

    STDMETHOD(Clone)(_COM_Outptr_ IItemsContainer **ppItemsContainer)
    {
        CVectorItemsContainer *pVectorItemsContainer;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CVectorItemsContainer>::CreateInstance(&pVectorItemsContainer));
        pVectorItemsContainer->_container.copyItems(this->_container.getItems());
        *ppItemsContainer = pVectorItemsContainer;
        return S_OK;
    }

    STDMETHOD(Dispose)()
    {
        _container.clear();
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   CSetItemsContainer
// IItemsContainer implementation using std::set
//------------------------------------------------------------------------------
class CSetItemsContainer :
    public foundation::ctl::ComInspectableBase,
    public pmod::library::IItemsContainer
{
protected:
    CSetItemsContainer()
    {
    }
    STDMETHOD(GetCount)(_Outptr_ UINT32 *pSize)
    {
        *pSize = (UINT32)_items.size();
        return S_OK;
    }

    STDMETHOD(GetItems)(_Outptr_ UINT32 *pSize,foundation::IInspectable ***items)
    {
        ContainerRefCountToArray(
            _items,
            pSize,
            items);
        return S_OK;
    }

    STDMETHOD(Get)(_In_ UINT32 index,_COM_Outptr_ foundation::IInspectable **value)
    {
        UINT32 iter_index = (UINT32)std::distance(_items.begin(), _itemsIterator);
        if (iter_index < index)
        {
            advance_items_iterator(index - iter_index);
        }
        else if (iter_index > index)
        {
            backward_items_iterator(iter_index - index);
        }
        *value = *_itemsIterator;
        if (*value)
        {
            (*value)->AddRef();
        }
        return S_OK;
    }

    STDMETHOD(Set)(_In_ UINT32 index, _In_ foundation::IInspectable *value)
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    STDMETHOD(Append)(_In_ foundation::IInspectable *value)
    {
        HRESULT hr = _items.insert(value).second ? S_OK : S_FALSE;
        reset_items_iterator();
        return hr;
    }

    STDMETHOD(Insert)(_In_ UINT32 index,_In_ foundation::IInspectable *value)
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    STDMETHOD(Remove)(_In_ foundation::IInspectable *value) 
    {
        HRESULT hr = _items.erase(value) ? S_OK:S_FALSE;
        reset_items_iterator();
        return hr;
    }

    STDMETHOD(RemoveAt)(_In_ UINT32 index)
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    STDMETHOD(InsertPositionOf)(
        _In_ foundation::IInspectable *value,
        _In_ pmod::library::CompareItemValueFunc compare,
        _In_ void *Context,
        _Outptr_ UINT32 *pIndex)
    {
        foundation_assert(pIndex);
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    STDMETHOD(Find)(
        _In_ foundation::IInspectable *value,
        _In_ pmod::library::CompareItemValueFunc compare,
        _In_ void *Context,
        _Outptr_ UINT32 *pIndex)
    {
        UNREFERENCED_PARAMETER(compare);
        UNREFERENCED_PARAMETER(Context);

        foundation_assert(pIndex);
        set_inspectable_ptr_type::const_iterator iter = _items.find(value);
        if(iter != _items.end())
        {
            *pIndex = (UINT32)std::distance(_items.begin(), iter);
        }
        return S_OK;
    }

    STDMETHOD(Sort)(_In_ pmod::library::CompareItemValueFunc compare,_In_ void *Context)
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }

    STDMETHOD(Clear)()
    {
        _items.clear();
        reset_items_iterator();
        return S_OK;
    }

    STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable*> **first)
    {
        typedef foundation::library::_IteratorImpl<
            set_inspectable_ptr_type,
            foundation::IInspectable *,
            foundation::_InspectableConverter> _SetInspectableIterator_Type;
        IFR_ASSERT(foundation::ctl::inspectable::CreateInstanceWithInitialize<_SetInspectableIterator_Type>(
            first,
            this->CastToInspectable(),
            _items));
        return S_OK;
    }

    STDMETHOD(Clone)(_COM_Outptr_ IItemsContainer **ppItemsContainer)
    {
        CSetItemsContainer *pSetItemsContainer;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CSetItemsContainer>::CreateInstance(&pSetItemsContainer));
        pSetItemsContainer->_items = this->_items;
        pSetItemsContainer->reset_items_iterator();

        *ppItemsContainer = pSetItemsContainer;
        return S_OK;
    }

    STDMETHOD(Dispose)()
    {
        _items.clear();
        return S_OK;
    }
private:
    inline void reset_items_iterator()
    {
        _itemsIterator = _items.begin();
    }
    inline void advance_items_iterator(UINT32 size)
    {
        while (size--)
        {
            ++_itemsIterator;
        }
    }
    inline void backward_items_iterator(UINT32 size)
    {
        while (size--)
        {
            --_itemsIterator;
        }
    }
private:
    typedef std::set<foundation::InspectablePtr,foundation::library::PropertyValueLessThan> set_inspectable_ptr_type;
    set_inspectable_ptr_type                 _items;
    set_inspectable_ptr_type::const_iterator _itemsIterator;
};


const CLogCategory*CObservableCollectionLogFactory::GetLogCategory()
{
    static _ComPtrSingleton<const CLogCategory, foundation::_NoRefCountPtrImpl> _collectionModelLogCategoryInstance;

    return _collectionModelLogCategoryInstance._GetInstance([]() {
        return CLogger::GetCategory(
            Foundation_CategoryEnum::TypeId,
            Object_CategoryEnum::TypeId,
            ObjectNode_CategoryEnum::TypeId,
            Collection_CategoryEnum::TypeId,
            -1);
    });
}

HRESULT CObservableCollection::_Initialize(const pmod::library::ObservableCollectionCreateParametersBase *pCollectionModelParam)
{
    IFR_ASSERT(CObservableCollectionBase_Type::_Initialize(pCollectionModelParam));

    m_IsReadOnly = IsOptionEnabled(pCollectionModelParam->m_options,ObservableCollectionOptions::IsReadOnly);
    m_IsCollectionSet = IsOptionEnabled(pCollectionModelParam->m_options,ObservableCollectionOptions::IsSet);
    m_UseFastItemsContainer = IsOptionEnabled(pCollectionModelParam->m_options, ObservableCollectionOptions::UseFastItemsContainer);

    return S_OK;
}

HRESULT CObservableCollection::_Initialize(const pmod::library::ObservableCollectionCreateParameters* pCollectionModelParam)
{
    IFR_ASSERT(_Initialize(static_cast<const pmod::library::ObservableCollectionCreateParametersBase*>(pCollectionModelParam)));

    if (!!pCollectionModelParam->m_initial_values_ptr)
    {
        IFR_ASSERT(EnsureItemsContainer());

        InspectableArrayWrapper arr_items;
        IFR_ASSERT(pv_util::GetInspectableArray(pCollectionModelParam->m_initial_values_ptr.Get(), arr_items.GetSizeAddressOf(), arr_items.GetBufferAddressOf()));

        if (arr_items.GetSize() > 0)
        {
            _Inspectable_ptr_vector_type vecItems;
            CopyArrayToVector(arr_items.GetSize(), arr_items.GetBuffer(), vecItems);

            return InsertItemsInternal(
                IsCollectionSet() ? NINDEX : 0,
                vecItems,
                /* fireNotify = */ false,
                /* isProtected = */ IsReadOnly());
        }
    }

    return S_OK;
}

void CObservableCollection::OnFinalRelease()
{
    CObservableCollectionBase_Type::OnFinalRelease();
    DisposeItems();
}

HRESULT CObservableCollection::DisposeItems()
{
    if(m_spItemsContainer != nullptr)
    {
        m_spItemsContainer->Dispose();
    }
    return S_OK;
}

HRESULT CObservableCollection::DisposeInternal()
{
    IFR_ASSERT(CObservableCollectionBase_Type::DisposeInternal());
    {
        _FoundationBaseLock csl(*this);
        DisposeItems();
        m_spItemsContainer.Release();
    }
    return S_OK;
}

HRESULT CObservableCollection::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    HRESULT hr = S_OK;
    if (iid == IEnumerable::GetIID())
    {
        *ppInterface = static_cast<IEnumerable *>(static_cast<CObservableCollectionBase_Type *>(this));
    }
    else if (iid == IObservableList::GetIID())
    {
        if(this->IsReadOnly())
        {

            hr = E_NOINTERFACE;
        }
        else
        {
            *ppInterface = static_cast<IObservableList *>(this);
        }
    }
    else
    {
        hr = CObservableCollectionBase_Type::QueryInterfaceImpl(iid, ppInterface);
    }
    return hr;
}

HRESULT CObservableCollection::FireNotifyCollectionChanged(
    _FoundationBaseLock& csl,
    NotifyCollectionModelChangedAction action,
    _Inspectable_ptr_vector_type& items,
    UINT32 index)
{
    foundation_assert(action == NotifyCollectionModelChangedAction::ItemAdded || action == NotifyCollectionModelChangedAction::ItemRemoved);

    if (IsNotifyCollectionChanged())
    {
        foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
        IFR_ASSERT(pmod::library::NotifyCollectionChangedEventArgsImpl::CreateInstance(action, items, index, spNotifyCollectionChangedEventArgs.GetAddressOf()));
        // Note: we assume item valus are from container
        IFR_ASSERT(InitializeIfValueConverterCallback(spNotifyCollectionChangedEventArgs));
        return FireNotifyCollectionChanged(csl,spNotifyCollectionChangedEventArgs);
    }

    return S_OK;
}

HRESULT CObservableCollection::FireReplaceNotifyCollectionChanged(
    _FoundationBaseLock& csl,
    _Inspectable_ptr_vector_type& oldItems,
    _Inspectable_ptr_vector_type& newItems,
    UINT32 index)
{

    if (IsNotifyCollectionChanged())
    {
        foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
        IFR_ASSERT(pmod::library::NotifyCollectionChangedEventArgsImpl::CreateReplaceInstance(index, oldItems, newItems, spNotifyCollectionChangedEventArgs.GetAddressOf()));
        // Note: we assume item valus are from container
        IFR_ASSERT(InitializeIfValueConverterCallback(spNotifyCollectionChangedEventArgs));

        return FireNotifyCollectionChanged(csl,spNotifyCollectionChangedEventArgs);
    }

    return S_OK;
}

HRESULT CObservableCollection::FireMoveNotifyCollectionChanged(
    _FoundationBaseLock& csl,
    NotifyCollectionModelChangedAction action,
    _Inspectable_ptr_vector_type& items,
    UINT32 nOldIndex,
    UINT32 nNewIndex)
{
    foundation_assert(action == NotifyCollectionModelChangedAction::ItemAdded || action == NotifyCollectionModelChangedAction::ItemRemoved);

    if (IsNotifyCollectionChanged())
    {
        foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
        IFR_ASSERT(pmod::library::NotifyCollectionChangedEventArgsImpl::CreateMoveInstance(action, items, nOldIndex, nNewIndex, spNotifyCollectionChangedEventArgs.GetAddressOf()));
        // Note: we assume item values are from container        
        IFR_ASSERT(InitializeIfValueConverterCallback(spNotifyCollectionChangedEventArgs));

        return FireNotifyCollectionChanged(csl,spNotifyCollectionChangedEventArgs);
    }

    return S_OK;
}

HRESULT CObservableCollection::FireResetNotifyCollectionChanged(
    _FoundationBaseLock& csl,
    _Inspectable_ptr_vector_type& oldItems,
    _Inspectable_ptr_vector_type& newItems)
{
    if (IsNotifyCollectionChanged())
    {
        foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
        IFR_ASSERT(pmod::library::NotifyCollectionChangedEventArgsImpl::CreateResetInstance(oldItems, newItems, spNotifyCollectionChangedEventArgs.GetAddressOf()));
        // Note: we assume item valus are from container        
        IFR_ASSERT(InitializeIfValueConverterCallback(spNotifyCollectionChangedEventArgs));

        IFR(FireNotifyCollectionChanged(csl,spNotifyCollectionChangedEventArgs));
    }
    return S_OK;
}

STDMETHODIMP CObservableCollection::GetCount(_Outptr_ UINT32 *pSize)
{
    IFCPTR(pSize);

    PERF_CONTEXT(Core_PerfId_CollectionModel_GetCount_Id)

    _FoundationBaseLock csl(*this);
    IFR_ASSERT(EnsureItemsContainer());
    return m_spItemsContainer->GetCount(pSize);
}

STDMETHODIMP CObservableCollection::GetItem(_In_ UINT32 nIndex,_Outptr_ foundation::IInspectable ** ppItem)
{
    PERF_CONTEXT(CoreProxyFactory_PerfId_CollectionModel_GetItem_Id)
    
    _FoundationBaseLock csl(*this);

    // logging support for 'GetItem Call' 
    if (IsLevelEnabled(
        foundation::LoggingLevel::Trace,
            Collection_Category_GetItem_Id))
    {
        LogGetItem(nIndex);
    }

    IFR(CheckArrayBounds(nIndex));
    foundation_assert(m_spItemsContainer != nullptr);

    return this->GetContainerItem(nIndex,true,ppItem);
}

STDMETHODIMP CObservableCollection::SetItem(_In_ UINT32 nIndex,_In_ foundation::IInspectable *pItem)
{
    // logging support for 'SetItem Call' 
    if (IsLevelEnabled(foundation::LoggingLevel::Trace,
            Collection_Category_SetItem_Id))
    {
        LogSetItem(nIndex);
    }

    _Inspectable_ptr_vector_type items;
    items.push_back(pItem);
    return ReplaceItemsInternal(nIndex,items,true,false);
}


STDMETHODIMP CObservableCollection::GetItems(_Outptr_ UINT32 *size, foundation::IInspectable *** ppItems)
{
    _FoundationBaseLock csl(*this);

    // logging support for 'GetItems Call' 
    if (IsLevelEnabled(foundation::LoggingLevel::Trace,
            Collection_Category_GetItems_Id))
    {
        LogGetItems();
    }

    IFR_ASSERT(EnsureItemsContainer());

    _Inspectable_ptr_vector_type items;
    CopyContainerItems(items);

    // convert the items
    IFR_ASSERT(ConvertItemValues(items,false));
    
    // return to public array
    ContainerToArray(items,size,ppItems);
    return S_OK;
}

STDMETHODIMP CObservableCollection::AppendItem(_In_ foundation::IInspectable *pItem)
{
    return InsertItemInternal(
        NINDEX, // nIndex
        pItem,
        true,
        false);
}

STDMETHODIMP CObservableCollection::InsertItem(_In_ UINT32 nIndex,_In_ foundation::IInspectable *pItem)
{
    return InsertItemInternal(
        nIndex,
        pItem,
        true,
        false);
}

STDMETHODIMP CObservableCollection::RemoveItem(_In_ foundation::IInspectable *pItem)
{
    return RemoveItemInternal(pItem,false);
}

STDMETHODIMP CObservableCollection::RemoveAt(_In_ UINT32 nIndex)
{
    return RemoveAtInternal(nIndex,true,false);
}

STDMETHODIMP CObservableCollection::RemoveAll()
{
    return RemoveAllInternalHelper(false);
}

STDMETHODIMP CObservableCollection::GetNotifyCollectionChangedEventSource(_Outptr_ INotifyCollectionChangedEventSource** ppEventSource)
{
    IFR_ASSERT(EnsureEventSourceModel());

    *ppEventSource = GetEventSourceImpl();
    (*ppEventSource)->AddRef();
    return S_OK;
}

STDMETHODIMP CObservableCollection::GetItemsContainer(_COM_Outptr_ pmod::library::IItemsContainer **ppItemsContainer)
{
    _FoundationBaseLock csl(*this);
    IFR_ASSERT(EnsureItemsContainer());
    return m_spItemsContainer.CopyTo(ppItemsContainer);
}

STDMETHODIMP CObservableCollection::GetItemConverter(_COM_Outptr_ pmod::library::IValueConverter **ppValueConverter)
{
    return m_spItemConverter.CopyTo(ppValueConverter);
}

STDMETHODIMP CObservableCollection::SetItemConverter(_In_ pmod::library::IValueConverter *pValueConverter)
{
    m_spItemConverter = pValueConverter;
    return S_OK;
}

STDMETHODIMP CObservableCollection::Sort(_In_ pmod::library::CompareItemValueFunc  comparer,void *Context)
{
    _FoundationBaseLock csl(*this);
    if(m_spItemsContainer == nullptr)
    {
        // nothing to sort
        return S_OK;
    }

    _Inspectable_ptr_vector_type oldItems,newItems;

    CopyContainerItems(oldItems);
    IFR_ASSERT(m_spItemsContainer->Sort(comparer,Context));
    CopyContainerItems(newItems);

    if(!IsInModelChanges())
    {
        return FireResetNotifyCollectionChanged(csl,oldItems,newItems);
    }
    return S_OK;
}

STDMETHODIMP CObservableCollection::InsertInOrder(
    _In_ foundation::IInspectable *newValue,
    _In_ pmod::library::CompareItemValueFunc comparer,
    _In_ void *Context,
    _Outptr_ UINT32 *index)
{
    return InsertInOrderInternal(
        newValue,
        comparer,
        Context,
        index);
}

STDMETHODIMP CObservableCollection::AppendItemInternal(_In_ foundation::IInspectable *item)
{
    return InsertItemInternal(
        NINDEX, // nIndex
        item,
        true,
        true);
}

STDMETHODIMP CObservableCollection::InsertItemInternal(_In_ UINT32 nIndex,_In_ foundation::IInspectable *item)
{
    return InsertItemInternal(
        nIndex,
        item,
        true,
        true);
}

STDMETHODIMP CObservableCollection::ReplaceItemInternal(_In_ UINT32 nIndex,_In_ foundation::IInspectable *item)
{
    _Inspectable_ptr_vector_type items;
    items.push_back(item);
    return ReplaceItemsInternal(nIndex, items, true, true);
}

STDMETHODIMP CObservableCollection::InsertItems(_In_ UINT32 nIndex,_In_ UINT32 size,_In_ foundation::IInspectable **items)
{
    IFCPTR(items);

    _Inspectable_ptr_vector_type _items;
    CopyArrayToVector(size,items,_items);
    return InsertItemsInternal(
        nIndex,
        _items,
        true,
        true);
}

STDMETHODIMP CObservableCollection::ReplaceItems(_In_ UINT32 nIndex,_In_ UINT32 size,_In_ foundation::IInspectable **items)
{
    IFCPTR(items);

    _Inspectable_ptr_vector_type _items;
    CopyArrayToVector(size,items,_items);
    return ReplaceItemsInternal(
        nIndex,
        _items,
        true,
        true);
}

STDMETHODIMP CObservableCollection::RemoveItems(_In_ UINT32 nIndex,_In_ UINT32 count)
{
    return RemoveAtInternal(nIndex,count,true,true);
}

STDMETHODIMP CObservableCollection::RemoveAllInternal()
{
    return RemoveAllInternalHelper(true);
}

STDMETHODIMP CObservableCollection::RemoveItemInternal(_In_ foundation::IInspectable *item)
{
    return this->RemoveItemInternal(item,true);
}

HRESULT CObservableCollection::VerifyItemTypeInfo(_Inout_ foundation::IInspectable** item)
{
    foundation::ComPtr<IPropertyTypeInfo> spItemTypeInfo;
    IFR_ASSERT(this->m_pTypeInfo->GetItemTypeInfo(spItemTypeInfo.GetAddressOf()));
    IFR(CModelTypeInfoStatics::_VerifyPropertyTypeInfo(
        spItemTypeInfo,
        item));
    return S_OK;
}

HRESULT CObservableCollection::VerifyItemsTypeInfo(_In_ _Inspectable_ptr_vector_type& items)
{
    for(_Inspectable_ptr_vector_type::iterator iter = items.begin();
        iter != items.end();
        ++iter)
    {
        IFR(VerifyItemTypeInfo((*iter).GetAddressOfPtr()));
    }
    return S_OK;
}

HRESULT CObservableCollection::InsertItemsInternal(
        _In_ UINT32 nIndex,
        _In_ _Inspectable_ptr_vector_type& items,
        bool fireNotify,
        bool isProtected)
{
    _FoundationBaseLock csl(*this);

    HRESULT hr;
    IFHR_ASSERT(EnsureItemsContainer());

    if (!IsCollectionSet() && nIndex == NINDEX)
    {
        m_spItemsContainer->GetCount(&nIndex);
    }

    if(!isProtected && IsReadOnly())
    {
        IFR(pmod::Errors::E_COLLECTION_READ_ONLY);
    }

    foundation_assert(nIndex != NINDEX || IsCollectionSet());

    foundation_assert(m_spItemsContainer != nullptr);
    if(!IsCollectionSet())
    {
        UINT32 count;
        m_spItemsContainer->GetCount(&count);

        // this code will assume a lock into the critical section
        if(nIndex > count)
        {
            return E_BOUNDS;
        }
    }

    if(!isProtected || IsVerifyTypeInfoInternal())
    {
        IFHR(VerifyItemsTypeInfo(items));
    }

    _Inspectable_ptr_vector_type appendItems;
    UINT32 index = 0;
    for(_Inspectable_ptr_vector_type::const_iterator iter = items.begin();
        iter != items.end();
        ++iter,
        ++index)
    {
        foundation::InspectablePtr spItemValue;
        if(!isProtected)
        {
            IFHR_ASSERT(ConvertItemValue(
                nIndex + index,
                *iter,
                true,
                spItemValue.GetAddressOf()));
        }
        else
        {
            spItemValue = *iter;
        }

        // add it
        if(nIndex == NINDEX)
        {
            IFHR_ASSERT(m_spItemsContainer->Append(spItemValue));
            if(hr == S_OK)
            {
                appendItems.push_back(*iter);
            }
        }
        else
        {
            IFHR_ASSERT(m_spItemsContainer->Insert(nIndex + index, spItemValue));
        }

        if(!IsFinalRelease())
        {
            // set ownership
            SetAsParentNodeImpl((*iter));
        }
    }

    // track model changes if needed
    if (IsInModelChanges())
    {
        m_CollectionModelChanges->AddCollectionModelChangeAction(
            this,
            NotifyCollectionModelChangedAction::ItemAdded,
            nIndex, 
            nIndex == NINDEX ? appendItems : items);
    }

    if(fireNotify && !IsInModelChanges() && (nIndex != NINDEX || appendItems.size()))
    {
        IFHR(FireNotifyCollectionChanged(csl, NotifyCollectionModelChangedAction::ItemAdded, (nIndex == NINDEX ? appendItems : items), nIndex));
    }
    return S_OK;
}

HRESULT CObservableCollection::ReplaceItemsInternal(
    _In_ UINT32 nIndex,
    _In_ _Inspectable_ptr_vector_type& items,
    bool fireNotify,
    bool isProtected)
{
    _FoundationBaseLock csl(*this);

    if (!isProtected && IsReadOnly())
    {
        IFR(pmod::Errors::E_COLLECTION_READ_ONLY);
    }

    foundation_assert(items.size() > 0);
    // this code will assume a lock into the critical section
    IFR(CheckArrayBounds(nIndex));
    if(items.size() > 1)
    {
        IFR(CheckArrayBounds(nIndex + (UINT32)items.size()-1));
    }

    if(!isProtected || IsVerifyTypeInfoInternal())
    {
        IFR(VerifyItemsTypeInfo(items));
    }

    _Inspectable_ptr_vector_type oldItems;
    UINT32 index = 0;
    for(_Inspectable_ptr_vector_type::const_iterator iter = items.begin();
        iter != items.end();
        ++iter,
        ++index)
    {
        foundation::InspectablePtr oldItem;
        IFR_ASSERT(m_spItemsContainer->Get(nIndex + index,oldItem.GetAddressOf()));
        oldItems.push_back(oldItem);

        IFR_ASSERT(m_spItemsContainer->Set(nIndex + index,(*iter)));
        // change ownership
        ClearParentNodeImpl(oldItem);
        if(!IsFinalRelease())
        {
            SetAsParentNodeImpl((*iter));
        }
    }

    // track model changes if needed
    if (IsInModelChanges())
    {
        m_CollectionModelChanges->AddCollectionModelChangeAction(
            this, NotifyCollectionModelChangedAction::ItemReplaced, nIndex, items);
    }

    if(fireNotify && !IsInModelChanges())
    {
        return FireReplaceNotifyCollectionChanged(csl,oldItems, items, nIndex);
    }
    return S_OK;
}

HRESULT CObservableCollection::RemoveAtInternal(
    _In_ UINT32 nIndex,
    _In_ UINT32 nCount,
    bool fireNotify,
    bool isProtected)
{
    _FoundationBaseLock csl(*this);
    return RemoveAtInternal(csl, nIndex, nCount, fireNotify, isProtected);
}

HRESULT CObservableCollection::RemoveAtInternal(
    _FoundationBaseLock& csl,
    _In_ UINT32 nIndex,
    _In_ UINT32 nCount,
    bool fireNotify,
    bool isProtected)
{
    if(!isProtected && IsReadOnly())
    {
        IFR(pmod::Errors::E_COLLECTION_READ_ONLY);
    }

    foundation_assert(!IsCollectionSet());
    foundation_assert(nCount > 0);

    // this code will assume a lock into the critical section
    IFR(CheckArrayBounds(nIndex));
    if(nCount > 1)
    {
        IFR(CheckArrayBounds(nIndex + nCount - 1));
    }

    _Inspectable_ptr_vector_type removedItems;
    while(nCount)
    {
        foundation::InspectablePtr item;
        IFR_ASSERT(m_spItemsContainer->Get(nIndex,item.GetAddressOf()));
        removedItems.push_back(item);
        // remove from our container
        IFR_ASSERT(m_spItemsContainer->RemoveAt(nIndex));
        --nCount;
        // clear ownership
        ClearParentNodeImpl(item);
    }

    // track model changes if needed
    if (IsInModelChanges())
    {
        m_CollectionModelChanges->AddCollectionModelChangeAction(
            this,
            NotifyCollectionModelChangedAction::ItemRemoved,
            nIndex,
            removedItems);
    }

    if(fireNotify && !IsInModelChanges())
    {
        IFR(FireNotifyCollectionChanged(csl, NotifyCollectionModelChangedAction::ItemRemoved, removedItems, nIndex));
    }
    return S_OK;
}

HRESULT CObservableCollection::MoveItemsInternal(
    _In_ UINT32 nOldIndex,
    _In_ UINT32 nNewIndex,
    _In_ UINT32 nCount,
    _In_ bool fireNotify,
    _In_ bool isProtected)
{
    _FoundationBaseLock csl(*this);
    return MoveItemsInternal(csl, nOldIndex, nNewIndex, nCount, fireNotify, isProtected);
}

HRESULT CObservableCollection::MoveItemsInternal(
        _FoundationBaseLock& csl,
        _In_ UINT32 nOldIndex,
        _In_ UINT32 nNewIndex,
        _In_ UINT32 nCount,
        _In_ bool fireNotify,
        _In_ bool isProtected)
{
    HRESULT hr = S_OK;
    _Inspectable_ptr_vector_type movedItems;

    if (!isProtected && IsReadOnly())
    {
        IFCL(pmod::Errors::E_COLLECTION_READ_ONLY);
    }

    foundation_assert(nOldIndex != nNewIndex);
    foundation_assert(nCount > 0);
   
    // this code will assume a lock into the critical section
    IFCL(CheckArrayBounds(nOldIndex));
    IFCL(CheckArrayBounds(nNewIndex));
    if (1 < nCount)
    {
        IFCL(CheckArrayBounds(nOldIndex + nCount - 1));
    }

    // Remove the items at their old position in the list
    //
    // Note: we don't bother to clear the parent attribute or call UpdateIfPropertyModel,
    // because we immediately re-insert the items back into the collection.  If we hit a
    // failure, we will make sure to do this in the Cleanup section.
    for (UINT32 i = 0; i < nCount; i++)
    {
        foundation::InspectablePtr item;
        IFR_ASSERT(m_spItemsContainer->Get(nOldIndex,item.GetAddressOf()));
        movedItems.push_back(item);
        IFR_ASSERT(m_spItemsContainer->RemoveAt(nOldIndex));
    }

    // Fire the move-remove event
    if (fireNotify && !IsInModelChanges())
    {
        IFCL(FireMoveNotifyCollectionChanged(csl, NotifyCollectionModelChangedAction::ItemRemoved, movedItems, nOldIndex, nNewIndex));
    }
    
    // Add the items back in to the list at their new location
    for(_Inspectable_ptr_vector_type::const_iterator iter = movedItems.begin();
        iter != movedItems.end();
        ++iter)
    {
        IFR_ASSERT(m_spItemsContainer->Insert(nNewIndex + (UINT32)(iter - movedItems.begin()), (*iter)));
    }

    // Fire the move-insert event
    if(fireNotify && !IsInModelChanges())
    {
        hr = FireMoveNotifyCollectionChanged(csl, NotifyCollectionModelChangedAction::ItemAdded, movedItems, nOldIndex, nNewIndex);
    }
    movedItems.clear();
    IFHR(hr);

Cleanup:
    // if we arrive here we can still have moved items pending since
    // basically we failed on a IFCL
    if (0 < movedItems.size())
    {
        // this code will make sure we leave the collection in a correct state
        // and also we Clear the parent for the non moved items
        foundation_assert(FAILED(hr));
        for(_Inspectable_ptr_vector_type::const_iterator iter = movedItems.begin();
            iter != movedItems.end();
            ++iter)
        {
            ClearParentNodeImpl((*iter));
        }
    }
    
    return hr;
}

HRESULT CObservableCollection::RemoveAllInternalHelper(bool isProtected)
{
    _FoundationBaseLock csl(*this);

    IFR_ASSERT(EnsureItemsContainer());
    
    if(!isProtected && IsReadOnly())
    {
        IFR(pmod::Errors::E_COLLECTION_READ_ONLY);
    }

    _Inspectable_ptr_vector_type oldItems;
    {

        CopyContainerItems(oldItems);
        m_spItemsContainer->Clear();

        // remove Ownership on every item
        for(_Inspectable_ptr_vector_type::const_iterator iter = oldItems.begin();
            iter != oldItems.end();
            ++iter)
        {
            ClearParentNodeImpl((*iter));
        }
    }


    // track model changes if needed
    if (IsInModelChanges())
    {
        m_CollectionModelChanges->AddResetAction(this);
    }

    if((!IsInModelChanges() && this->IsObserved()) || m_pDelegate != nullptr)
    {
        _Inspectable_ptr_vector_type newItems;
        IFR(FireResetNotifyCollectionChanged(csl,oldItems,newItems));
    }
    return S_OK;
}

HRESULT CObservableCollection::RemoveItemInternal(_In_ foundation::IInspectable *item,bool isProtected)
{
    _FoundationBaseLock csl(*this);

    IFR_ASSERT(EnsureItemsContainer());
    if(IsCollectionSet())
    {
        if(!isProtected && IsReadOnly())
        {
            IFR(pmod::Errors::E_COLLECTION_READ_ONLY);
        }

        HRESULT hr;
        _Inspectable_ptr_vector_type removedItems;
        removedItems.push_back(item);
        IFHR_ASSERT(m_spItemsContainer->Remove(item));
        // clear ownership
        ClearParentNodeImpl(item);

        if(hr == S_OK) // proceed only if item was removed from std::set
        {
            if (IsInModelChanges())
            {
                m_CollectionModelChanges->AddCollectionModelChangeAction(
                    this,
                    NotifyCollectionModelChangedAction::ItemRemoved,
                    NINDEX,
                    removedItems);
            }
            else
            {
                IFR(FireNotifyCollectionChanged(csl, NotifyCollectionModelChangedAction::ItemRemoved, removedItems, NINDEX));
            }
        }
        return S_OK;
    }
    else
    {
        UINT32 index;
        if ((index = FindItemInternal(item)) != NINDEX)
        {
            return RemoveAtInternal(csl,index,1,true,isProtected);
        }
        return foundation::Errors::E_PROPERTY_NOT_FOUND;
    }
}

HRESULT CObservableCollection::RemoveAtInternal(_In_ UINT32 nIndex,bool fireNotify,bool isProtected)
{
    return RemoveAtInternal(nIndex,1,fireNotify,isProtected);
}

HRESULT CObservableCollection::InsertItemInternal(
    _In_ UINT32 nIndex,
    _In_ foundation::IInspectable *item,
    bool fireNotify,
    bool isProtected)
{
    _Inspectable_ptr_vector_type items;
    items.push_back(item);
    return InsertItemsInternal(nIndex,items,fireNotify,isProtected);
}

HRESULT CObservableCollection::MoveItemInternal(
    _In_ UINT32 nOldIndex,
    _In_ UINT32 nNewIndex,
    _In_ bool fireNotify,
    _In_ bool isProtected)
{
    return MoveItemsInternal(nOldIndex,nNewIndex,1,fireNotify,isProtected);
}

bool CObservableCollection::IsItemEqual(_In_ foundation::IInspectable *item1,_In_ foundation::IInspectable *item2)
{
    return pv_util::CompareInspectableValues(item1,item2) == 0;
}

int __cdecl CObservableCollection::_ItemsComparerAdapter(foundation::IInspectable *elem1, foundation::IInspectable *elem2,void *Context)
{
    if((reinterpret_cast<CObservableCollection *>(Context))->IsItemEqual(elem1,elem2))
    {
        return 0;
    }
    return -1;
}

UINT32 CObservableCollection::FindItemInternal(_In_ foundation::IInspectable *item)
{
    foundation_assert(m_spItemsContainer != nullptr);

    UINT32 index;
    IFR_ASSERT(m_spItemsContainer->Find(item,&_ItemsComparerAdapter,this,&index));
    return index;
}

HRESULT CObservableCollection::InsertInOrderInternal(
        _In_ foundation::IInspectable *newValue, 
        _In_ pmod::library::CompareItemValueFunc comparer,
        _In_ void *Context,
        _Outptr_ UINT32 *pIndex,
        bool fireNotify)
{
    foundation_assert(pIndex);
    foundation_assert(m_spItemsContainer != nullptr);
    UINT32 index;
    IFR_ASSERT(m_spItemsContainer->InsertPositionOf(newValue,comparer,Context,&index));
    if(pIndex)
    {
        *pIndex = index;
    }
    return InsertItemInternal(
        index,
        newValue,
        fireNotify,
        true);
}

HRESULT CObservableCollection::CheckArrayBounds(_In_ UINT32 nIndex)
{
    IFR_ASSERT(EnsureItemsContainer());

    UINT32 count = 0;
    m_spItemsContainer->GetCount(&count);
    if(nIndex >= count)
    {
        return E_BOUNDS;
    }
    return S_OK;
}

STDMETHODIMP CObservableCollection::FireCollectionChanged(
        _In_ NotifyCollectionModelChangedAction action,
        _In_ UINT32 oldStartingIndex,
        _In_ UINT32 newStartingIndex,
        _In_ UINT32 sizeOldItems,
        _In_ foundation::IInspectable **ppOldItems,
        _In_ UINT32 sizeNewItems,
        _In_ foundation::IInspectable **ppNewItems
        )
{
    _Inspectable_ptr_vector_type oldItems,newItems;

    CopyArrayToVector(sizeOldItems,ppOldItems,oldItems);
    CopyArrayToVector(sizeNewItems,ppNewItems,newItems);

    _FoundationBaseLock csl(*this);

    if (IsInModelChanges())
    {
        if (action == NotifyCollectionModelChangedAction::Reset)
        {
            m_CollectionModelChanges->AddResetAction(this);
        }
        else
        {
            m_CollectionModelChanges->AddCollectionModelChangeAction(
                this,
                action,
                action == NotifyCollectionModelChangedAction::ItemRemoved ? oldStartingIndex : newStartingIndex,
                action == NotifyCollectionModelChangedAction::ItemRemoved ? oldItems : newItems);
        }
        return S_OK;
    }
    else
    {
        if (!this->IsObserved())
        {
            return S_FALSE;
        }
        foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
        IFR_ASSERT(pmod::library::NotifyCollectionChangedEventArgsImpl::CreateInstance(
            action,
            oldStartingIndex,
            newStartingIndex,
            oldItems,
            newItems, spNotifyCollectionChangedEventArgs.GetAddressOf()));
        IFR_ASSERT(InitializeIfValueConverterCallback(spNotifyCollectionChangedEventArgs));

        return FireNotifyCollectionChanged(csl,spNotifyCollectionChangedEventArgs);
    }
}

HRESULT CObservableCollection::InitializeIfValueConverterCallback(
    _In_ pmod::INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs)
{
    if (m_spItemConverter != nullptr)
    {
        pmod::library::NotifyCollectionChangedEventArgsImpl *pCNotifyCollectionChangedEventArgs =
            static_cast<pmod::library::NotifyCollectionChangedEventArgsImpl *>(pNotifyCollectionChangedEventArgs);
        IFR_ASSERT(foundation::ctl::CreateInstanceWithInitialize<CObservableCollectionValueConverterCallback>(
            pCNotifyCollectionChangedEventArgs->GetValueConverterCallbackPtr(), this));
    }
    return S_OK;
}

HRESULT CObservableCollection::FireResetNotifyCollectionChangedWithCallback()
{
    foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
    IFR_ASSERT(pmod::library::NotifyCollectionChangedEventArgsWithCallback::CreateResetInstance(
        static_cast<IObservableList *>(this),
        spNotifyCollectionChangedEventArgs.GetAddressOf()));
    _FoundationBaseLock csl(*this);
    IFR(this->FireNotifyCollectionChanged(csl,spNotifyCollectionChangedEventArgs));

    return S_OK;
}

HRESULT CObservableCollection::FireItemNotifyCollectionChangedWithCallback(
    _In_ UINT32 nNewIndex,
    foundation::NotifyCollectionModelChangedAction action
    )
{
    foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
    IFR_ASSERT(pmod::library::NotifyCollectionChangedEventArgsWithCallback::CreateItemInstance(
        action,
        static_cast<IObservableList *>(this),
        nNewIndex,
        spNotifyCollectionChangedEventArgs.GetAddressOf()));
    _FoundationBaseLock csl(*this);
    IFR(this->FireNotifyCollectionChanged(csl,spNotifyCollectionChangedEventArgs));
    return S_OK;
}

bool CObservableCollection::IsNotifyCollectionChanged()
{
    return this->IsObserved() || this->m_pDelegate != nullptr;
}

HRESULT CObservableCollection::FireNotifyCollectionChanged(
    _FoundationBaseLock& csl,
    INotifyCollectionChangedEventArgs *pEventArgs)
{
    if(m_pDelegate != nullptr && !IsInitialize())
    {
        IGNOREHR(m_pDelegate->OnCollectionChanged(pEventArgs));
    }

    if (this->IsObserved())
    {
        PERF_CONTEXT(Core_PerfId_CollectionModel_FireNotifyCollectionChanged_Id)

        return FireWithCallback(csl,&CObservableCollectionBase_Type::FireEvent, pEventArgs);
    }
    return S_OK;
}

HRESULT  CObservableCollection::CreateInstance(
    _In_ const pmod::library::ObservableCollectionCreateParameters *pParameters,
    _In_ foundation::IInspectable *pOuter,
    _Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CObservableCollection>(pParameters, pOuter, ppNewInstance);
}

void CObservableCollection::CopyContainerItems(_Inspectable_ptr_vector_type& items)
{
    foundation_assert(m_spItemsContainer != nullptr);

    foundation::InspectableArrayWrapper itemsWrapper;
    m_spItemsContainer->GetItems(itemsWrapper.GetSizeAddressOf(), itemsWrapper.GetBufferAddressOf());

    CopyItems(itemsWrapper, items);
}

STDMETHODIMP CObservableCollection::IndexOfInternal(_In_ foundation::IInspectable *item, _Outptr_ UINT32* pnIndex)
{
    IFCPTR(pnIndex);
    
    _FoundationBaseLock csl(*this);
    IFR_ASSERT(EnsureItemsContainer());

    *pnIndex = FindItemInternal(item);
    return S_OK;
}

STDMETHODIMP CObservableCollection::IndexOf(_In_ foundation::IInspectable *item, _Out_opt_ UINT32* pnIndex)
{
    foundation::InspectablePtr spItemValue;
    IFR_ASSERT(ConvertItemValue(NINDEX,item,true, spItemValue.GetAddressOf()));

    return IndexOfInternal(spItemValue,pnIndex);
}

STDMETHODIMP CObservableCollection::First(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable *> **first)
{
    IFR_ASSERT(EnsureItemsContainer());

    foundation::ComPtr<foundation::IIterator_impl<foundation::IInspectable *>> spIterator;
    IFR_ASSERT(m_spItemsContainer->First(spIterator.GetAddressOf()));
    return CCollectionModelIterator::CreateInstance(this, spIterator, first);
}

HRESULT CObservableCollection::OnBeginModelChangesInternal(pmod::library::BatchModelChangesOptions options)
{
    if(IsOptionEnabled(options,pmod::library::BatchModelChangesOptions::Transaction))
    {
        // TODO: implement a transaction mechanism on Collection Model
        return E_NOTIMPL;
    }
    foundation_assert(m_CollectionModelChanges.get() == nullptr);

    m_CollectionModelChanges.reset(new _CollectionModelChangesType());
    if(m_spItemsContainer != nullptr)
    {
        CopyContainerItems(m_CollectionModelChanges->_oldValues);
    }
    return S_OK;
}

HRESULT CObservableCollection::OnEndModelChangesInternal(
    pmod::library::EndModelChangesOptions options)
{
    foundation_assert(m_CollectionModelChanges.get() != nullptr);

    HRESULT hr = S_OK;
    if (m_CollectionModelChanges->_modelChanges.size() != 0)
    {
        if (m_CollectionModelChanges->CanFireModelChanges(this))
        {
            hr = m_CollectionModelChanges->FireModelChanges(this);
        }
        else
        {
            _FoundationBaseLock csl(*this);
            _Inspectable_ptr_vector_type newItems;
            if (m_spItemsContainer != nullptr)
            {
                CopyContainerItems(newItems);
            }
            hr = FireResetNotifyCollectionChanged(
                csl,
                m_CollectionModelChanges->_oldValues,
                newItems);
        }
    }
    // delete current tracking
    m_CollectionModelChanges.reset();
    return hr;
}

void CObservableCollection::_CollectionModelChangesType::AddCollectionModelChangeAction(
    CObservableCollection *pThis,
    foundation::NotifyCollectionModelChangedAction  action,
    UINT32 index,
    const _Inspectable_ptr_vector_type& items
    )
{
    if (pThis->IsLevelEnabled(foundation::LoggingLevel::Debug,
        Collection_Category_PushChangeAction_Id))
    {
        foundation::logger_util::Log(
            CLogger::GetLogger(),
            pThis->AsInspectable(),
            foundation::LoggingLevel::Debug,
            Collection_Category_PushChangeAction_Id,
            CObservableCollectionLogFactory::GetLogCategory()->AsInterface(),
            CollectionPushChangeAction_LogRecordEnum::TypeId,
            // Unique Id
            Model_LogRecord::UniqueId,
            foundation::pv_util::CreateValue((UINT32)pThis->m_UniqueId).Get(),
            // Action
            CollectionPushChangeAction_LogRecord::Action,
            foundation::pv_util::CreateValue((UINT32)action).Get(),
            // Index
            CollectionPushChangeAction_LogRecord::Index,
            foundation::pv_util::CreateValue(index).Get(),
            // NItems
            CollectionPushChangeAction_LogRecord::NItems,
            foundation::pv_util::CreateValue((UINT32)items.size()).Get(),
            -1);
    }

    if (this->_modelChanges.size())
    {
        _CollectionModelChangedActionType& lastAction = this->_modelChanges.back();
        if (lastAction._action == action &&
            ((index == NINDEX || action== NotifyCollectionModelChangedAction::ItemRemoved) ?
                index == lastAction._index : 
                index == (lastAction._index + 1)))
        {
            lastAction._items.insert(
                lastAction._items.end(),
                items.begin(), 
                items.end());
            // we just merged the last changed action so no further action
            return;
        }
        else if (lastAction._action == action &&
            action == NotifyCollectionModelChangedAction::ItemAdded &&
            index == lastAction._index)
        {
            // insert items at the beginning
            lastAction._items.insert(
                lastAction._items.begin(),
                items.begin(),
                items.end());
            // we just merged the last changed action so no further action
            return;
        }
    }
    // no merge was possible so create a new changed action
    _CollectionModelChangedActionType changedAction;
    changedAction._action = action;
    changedAction._index = index;
    changedAction._items = items;
    this->_modelChanges.push_back(changedAction);
}

void CObservableCollection::_CollectionModelChangesType::AddResetAction(CObservableCollection *pThis)
{
    this->AddCollectionModelChangeAction(
        pThis,
        NotifyCollectionModelChangedAction::Reset,
        NINDEX,
        _Inspectable_ptr_vector_type());
}

bool CObservableCollection::_CollectionModelChangesType::CanFireModelChanges(CObservableCollection *pThis)
{
    // find any Reset action that will invalidate all the batched actions
    if (std::find_if(
        this->_modelChanges.begin(),
        this->_modelChanges.end(),
        [](const _CollectionModelChangedActionType& item) {
            return item._action == NotifyCollectionModelChangedAction::Reset;
        }) != this->_modelChanges.end())
    {
        return false;
    }

    const size_t _MAX_MODEL_CHANGES = 5;
    return this->_modelChanges.size() < _MAX_MODEL_CHANGES &&
        (
        // only one merged action
        this->_modelChanges.size() == 1 ||
        // for Set collections the index should be NINDEX and so we could batch
        // all model changes
        pThis->m_IsCollectionSet);
}

HRESULT CObservableCollection::_CollectionModelChangesType::FireModelChanges(
    CObservableCollection *pThis)
{
    for (std::vector<_CollectionModelChangedActionType>::iterator iter =
        this->_modelChanges.begin();
        iter != this->_modelChanges.end();
        ++iter)
    {
        _FoundationBaseLock csl(*pThis);
        // we could have ItemAdded,ItemRemoved,ItemReplaced
        _IFR_(pThis->FireNotifyCollectionChanged(
            csl,
            (*iter)._action,
            (*iter)._items,
            (*iter)._index));
    }
    return S_OK;
}

HRESULT CObservableCollection::EnsureItemsContainer()
{
    if(m_spItemsContainer != nullptr)
    {
        return S_OK;
    }

    if(m_pDelegate)
    {
        IFR(m_pDelegate->OnCreateItemsContainer(
            m_spItemsContainer.GetAddressOf()));
    }

    if(m_spItemsContainer == nullptr)
    {
        if(IsCollectionSet())
        {
            // a set based container
            CSetItemsContainer *pT = nullptr;
            IFR_ASSERT(foundation::ctl::ComInspectableObject<CSetItemsContainer>::CreateInstance(&pT));
            m_spItemsContainer.Attach(pT);
        }
        else
        {
            if (m_UseFastItemsContainer)
            {
                pmod::library::FastItemsContainerParameters parameters;

                // retrieve Item property Type
                foundation::ComPtr<IPropertyTypeInfo> spItemTypeInfo;
                IFR_ASSERT(this->m_pTypeInfo->GetItemTypeInfo(spItemTypeInfo.GetAddressOf()));
                IFR_ASSERT(spItemTypeInfo->GetPropertyType(&parameters._propertyType));

                IFR_ASSERT(CFastItemsContainer::CreateInstance(
                    &parameters,
                    reinterpret_cast<pmod::library::IFastItemsContainer **>(m_spItemsContainer.GetAddressOf())));
            }
            else
            {
                // a vector based container
                CVectorItemsContainer *pT = nullptr;
                IFR_ASSERT(foundation::ctl::ComInspectableObject<CVectorItemsContainer>::CreateInstance(&pT));
                m_spItemsContainer.Attach(pT);
            }
        }
    }
    return S_OK;
}

HRESULT CObservableCollection::ConvertItemValue(
    _In_ UINT32 nIndex,
    _In_ foundation::IInspectable *pValue,
    _In_ bool isConvertBack,
    _COM_Outptr_ foundation::IInspectable **ppConvertedValue)
{
    UNREFERENCED_PARAMETER(nIndex);

    if (m_spItemConverter != nullptr)
    {
        foundation::ComPtr<IPropertyTypeInfo> spItemTypeInfo;
        IFR_ASSERT(this->m_pTypeInfo->GetItemTypeInfo(spItemTypeInfo.GetAddressOf()));
        return isConvertBack ?
            m_spItemConverter->ConvertBack(pValue,spItemTypeInfo,nullptr,ppConvertedValue):
            m_spItemConverter->Convert(pValue,spItemTypeInfo,nullptr,ppConvertedValue);
    }
    *ppConvertedValue = pValue;
    if(pValue)
    {
        pValue->AddRef();
    }
    return S_OK;
}

HRESULT CObservableCollection::ConvertItemValuesInternal(_Inspectable_ptr_vector_type& items, _In_ bool isConvertBack)
{
    for (_Inspectable_ptr_vector_type::iterator iter = items.begin();
        iter != items.end();
        ++iter)
    {
        foundation::InspectablePtr spItemValue;
        IFR_ASSERT(ConvertItemValue(
            (UINT32)(iter - items.begin()),
            *iter,
            isConvertBack,
            spItemValue.GetAddressOf()));
        *iter = spItemValue;
    }
    return S_OK;
}

HRESULT CObservableCollection::ConvertItemValues(_Inspectable_ptr_vector_type& items,_In_ bool isConvertBack)
{
    if(m_spItemConverter != nullptr)
    {
        return ConvertItemValuesInternal(items, isConvertBack);
    }
    return S_OK;
}

HRESULT CObservableCollection::GetContainerItem(
    _In_ UINT32 nIndex,
    _In_ bool bUseItemConverter,
    _COM_Outptr_ foundation::IInspectable **ppItemValue)
{
    foundation_assert(m_spItemsContainer != nullptr);
    foundation::InspectablePtr spItemValue;
    IFR_ASSERT(m_spItemsContainer->Get(nIndex,spItemValue.GetAddressOf()));
    if(bUseItemConverter)
    {
        return ConvertItemValue(nIndex,spItemValue, false, ppItemValue);
    }
    return spItemValue.CopyTo(ppItemValue);
}

HRESULT CObservableCollection::GetEventSource(
    _COM_Outptr_ INotifyCollectionChangedEventSource **ppEventSource)
{
    return this->GetNotifyCollectionChangedEventSource(ppEventSource);
}

HRESULT CObservableCollection::_ReturnAddHandlerWithOptions(
    _In_ foundation::IInspectable *pOptions,
    _COM_Outptr_ foundation::IInspectable **ppResult
    ) 
{
    IFR_ASSERT(EnsureItemsContainer());

    CObservableCollection *pT;
    pmod::library::ObservableCollectionCreateParametersBase parameters;
    parameters.m_options = ObservableCollectionOptions::IsReadOnly;
    parameters.m_iid_type = GUID_NULL;

    // create a CObservableCollection class
    foundation::ctl::ComInspectableObject<CObservableCollection>::CreateInstance(&pT);
    pT->_Initialize(&parameters);

    HRESULT hr = this->m_spItemsContainer->Clone(pT->m_spItemsContainer.GetAddressOf());
    if (hr == E_NOTIMPL)
    {
        foundation::InspectableArrayWrapper itemsWrapper;
        IFR_ASSERT(m_spItemsContainer->GetItems(
            itemsWrapper.GetSizeAddressOf(), 
            itemsWrapper.GetBufferAddressOf()));

        CVectorItemsContainer *pVectorItemsContainer;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CVectorItemsContainer>::CreateInstance(&pVectorItemsContainer));
        CopyItems(itemsWrapper,pVectorItemsContainer->_container.getItems());
        pT->m_spItemsContainer.Attach(pVectorItemsContainer);
    }
    else
    {
        IFR_ASSERT(hr);
    }
    // assign direct fields
    pT->m_spItemConverter = this->m_spItemConverter;

    *ppResult = pT->AsInspectable();
    return S_OK;
}

void CObservableCollection::LogGetItem(UINT32 index)
{
    // logging support for 'GetItem Call' 
    foundation::logger_util::Log(
        CLogger::GetLogger(),
        this->AsInspectable(),
        foundation::LoggingLevel::Trace,
        Collection_Category_GetItem_Id,
        CObservableCollectionLogFactory::GetLogCategory()->AsInterface(),
        CollectionGetItem_LogRecordEnum::TypeId,
        // Unique Id
        Model_LogRecord::UniqueId,
        foundation::pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
        // Item Index
        CollectionSetItem_LogRecord::Index,
        foundation::pv_util::CreateValue(index).Get(),
        -1);
}

void CObservableCollection::LogSetItem(UINT32 index)
{
    // logging support for 'SetItem Call'                 
    foundation::logger_util::Log(
        CLogger::GetLogger(),
        this->AsInspectable(),
        foundation::LoggingLevel::Trace,
        Collection_Category_SetItem_Id,
        CObservableCollectionLogFactory::GetLogCategory()->AsInterface(),
        CollectionGetItem_LogRecordEnum::TypeId,
        // Unique Id
        Model_LogRecord::UniqueId,
        foundation::pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
        // Item Index
        CollectionSetItem_LogRecord::Index,
        foundation::pv_util::CreateValue(index).Get(),
        -1);

}

void CObservableCollection::LogGetItems()
{
    foundation::logger_util::Log(
        CLogger::GetLogger(),
        this->AsInspectable(),
        foundation::LoggingLevel::Trace,
        Collection_Category_GetItems_Id,
        CObservableCollectionLogFactory::GetLogCategory()->AsInterface(),
        CollectionGetItems_LogRecordEnum::TypeId,
        // Unique Id
        Model_LogRecord::UniqueId,
        foundation::pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
        -1);
}

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

void CObservableCollection::DumpObject() 
{
    this->DumpObjectBase(U("CollectionModel"));
    foundation::IObject *pNextParent = nullptr;
    _traceMessage(U(" "));
    HRESULT hr = CObservableCollectionBase_Type::GetParentNode(&pNextParent);
    if (SUCCEEDED(hr) && pNextParent)
    {
        CObservableCollectionBase_Type::DumpHierarchy(pNextParent);
    }
}
#endif

class CObservableCollectionClassFactory :
    public foundation::ctl::Implements<IObservableCollectionClassFactory, &IID_IObservableCollectionClassFactory>
{
public:
    static IObservableCollectionClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CObservableCollectionClassFactory, IObservableCollectionClassFactory>();
        }
    }

    STDMETHODIMP CreateObservableCollectionInstance(
        _In_ const ObservableCollectionCreateParameters *pCreateParams,
        _In_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance) override
    {
        return CObservableCollection::CreateInstance(pCreateParams, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.ObservableCollectionClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CObservableCollectionClassFactory::GetInstance)
);
