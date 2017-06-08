/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Dictionary.cs
****/
using System;

namespace Microsoft.PropertyModel
{

    /// <summary>
    /// Dictionary Class
    /// </summary>
    public sealed class Dictionary :
        FactoryObject<DictionaryAdapter>,
        System.Collections.Generic.IDictionary<string,object>
    {

        public Dictionary(DictionaryAdapter adapter, ClassFactory classFactory):
            base(adapter, classFactory)
        {
        }
        public Dictionary(ClassFactory classFactory) :
            base(new DictionaryAdapter(), classFactory)
        {
        }
        public Dictionary() :
            this(null)
        {
        }

        public object this[string key]
        {
            get { return this.ToFactoryObject(this.Adapter.Lookup(key)); }
            set
            {
                this.Adapter.Insert(key, ToAdapter(value));
            }
        }

        public void Add(string key, object value)
        {
            this.Adapter.Insert(key, ToAdapter(value));
        }

        public bool ContainsKey(string key)
        {
            return this.Adapter.HasKey(key);
        }

        public System.Collections.Generic.ICollection<string> Keys
        {
            get 
            {
                string[] keys = this.Adapter.Keys;
                return new System.Collections.Generic.List<string>(keys);
            }
        }

        public bool Remove(string key)
        {
            this.Adapter.Remove(key);
            return true;
        }

        public bool TryGetValue(string key, out object value)
        {
            if(this.Adapter.HasKey(key))
            {
                value = this.ToFactoryObject(this.Adapter.Lookup(key));
                return true;
            }
            value = null;
            return false;
        }

        public System.Collections.Generic.ICollection<object> Values
        {
            get
            {
                return ToFactoryObjectItems<object>(this.Adapter.Values);
            }
        }

        public void Add(System.Collections.Generic.KeyValuePair<string, object> item)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public bool Contains(System.Collections.Generic.KeyValuePair<string, object> item)
        {
            throw new NotImplementedException();
        }

        public void CopyTo(System.Collections.Generic.KeyValuePair<string, object>[] array, int arrayIndex)
        {
            throw new NotImplementedException();
        }

        public int Count
        {
            get { return 0; }
        }

        public bool IsReadOnly
        {
            get { return false; }
        }

        public bool Remove(System.Collections.Generic.KeyValuePair<string, object> item)
        {
            throw new NotImplementedException();
        }

        public System.Collections.Generic.IEnumerator<System.Collections.Generic.KeyValuePair<string, object>> GetEnumerator()
        {
            var kvpList = new System.Collections.Generic.List<System.Collections.Generic.KeyValuePair<string, object>>();
            foreach (string key in this.Adapter.Keys)
            {
                kvpList.Add(new System.Collections.Generic.KeyValuePair<string, object>(key, this[key]));
            }
            return kvpList.GetEnumerator();
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            var enumerator = this as System.Collections.Generic.IEnumerable<System.Collections.Generic.KeyValuePair<string, object>>;
            return enumerator.GetEnumerator();
        }
    }
}
