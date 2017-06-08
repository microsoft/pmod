/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:enum_flags.h
****/
#pragma once


namespace foundation
{
	template <class TOptions, class TOptionMask>
	inline bool IsOptionEnabled(TOptions options, TOptionMask maskOption)
	{
		return (static_cast<int>(options) & static_cast<int>(maskOption)) ? true : false;
	}

#if defined(_VARIADIC_SUPPORT)

	template <typename TOption, typename TOptionMask>
	inline int _AddMaskOption(TOption& options, TOptionMask maskOption)
	{
		options = (TOption)(static_cast<UINT32>(options) | static_cast<UINT32>(maskOption));
		return 0;
	}

	template<typename... Args> static inline void _Pass_option_values(Args&&...) {}

	template<typename TOption, typename... Args>
	static TOption CreateOptions(TOption option, Args&&... args)
	{
		TOption options = option;
		_Pass_option_values(_AddMaskOption(options, args)...);
		return options;
	}

#endif
}
