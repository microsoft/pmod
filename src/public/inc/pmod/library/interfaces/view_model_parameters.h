/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:view_model_parameters.h
****/
#pragma once

namespace pmod {

    namespace library {

        template <class TBASE, class TViewModelDelegate, class TViewModelOptions>
        struct _ViewModelTypeInfoCreateParametersBase :
            public TBASE
        {
            _ViewModelTypeInfoCreateParametersBase() :
            m_p_viewmodel_delegate(nullptr),
            m_view_model_options((TViewModelOptions)0)
            {
            }

            TViewModelDelegate   *m_p_viewmodel_delegate;    // View Delegate Interface
            TViewModelOptions    m_view_model_options;      // Model Options
        };

        template <class TBASE, class TSource>
        struct _SingleSourceModelCreateParametersBase :
            public TBASE
        {
            _SingleSourceModelCreateParametersBase() :
            m_p_source(nullptr)
            {
            }
            TSource               *m_p_source;               // Source
        };

        template <class TBASE, class TViewModelDelegate, class TViewModelOptions, class TSource>
        struct _SingleSourceViewModelCreateParametersBase :
            public _SingleSourceModelCreateParametersBase
            <
            _ViewModelTypeInfoCreateParametersBase<TBASE, TViewModelDelegate, TViewModelOptions>,
            TSource
            >

        {
        };

        template <class TSource>
        struct _MultiSourceCreateParametersBase
        {
            _MultiSourceCreateParametersBase() :
            m_p_sources(nullptr),
            m_size_sources(0)
            {
            }
            TSource                **m_p_sources;     // Sources
            int                     m_size_sources;   // Size
        };

        template <class TBASE, class TSource>
        struct _MultiSourceCreateParametersBaseEx :
            public TBASE,
            public _MultiSourceCreateParametersBase<TSource>
        {
        };

        template <class TBASE, class TViewModelDelegate, class TViewModelOptions, class TSource>
        struct _MultiSourceViewModelCreateParametersBase :
            public _MultiSourceCreateParametersBaseEx
            <
            _ViewModelTypeInfoCreateParametersBase<TBASE, TViewModelDelegate, TViewModelOptions>,
            TSource
            >
        {
        };
    }
}

