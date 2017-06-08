/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:auto_delete_vector.h
****/
#pragma once

template <class T>
class auto_delete_vector : public std::vector<T*>
{
public:
	~auto_delete_vector()
	{
		clear_and_delete();
	}

	void clear_and_delete()
	{
		for (typename auto_delete_vector::const_iterator iter = this->begin();
		iter != this->end();
			++iter)
		{
			delete (*iter);
		}
		this->clear();
	}
};
