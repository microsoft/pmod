/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoviesApiService.cpp
****/

#include "MoviesTypeInfo.g.h"
#include "MoviesFastInternal.g.h"
#include <pmod/library/object_factory.h>
#include <pmod/library/library_util.h>
#include <foundation/library/json_statics_util.h>
#include <foundation/dictionary_ptr.h>
#include <foundation/library/com_ptr_singleton.h>

#ifdef _WINDOWS
#define API_EXPORT    __declspec(dllexport) 
#else
#define API_EXPORT
#endif

extern "C" void RegisterMoviesIf();

#define _TURN_OFF_PLATFORM_STRING
// casablanca includes
#include <cpprest/http_client.h>
#include <cpprest/uri_builder.h>

using namespace foundation;
using namespace foundation::library;
using namespace pmod;
using namespace pmod::library;
using namespace Movies;

const CHAR_t *_MoviesApiBase = U("https://api.themoviedb.org/3/");
const CHAR_t * _NowPlaying = U("/movie/now_playing");
const CHAR_t * _Api_key = U("api_key");


static HRESULT LoadJsonProperties(
    IObservableObjectClass *pClass,
    const DictionaryPtr& jsonObject,
    const CHAR_t **Keys,
    UINT32 propertyId,
    ...)
{
    va_list marker;
    va_start(marker, propertyId);     /* Initialize variable arguments. */

    while (propertyId != (UINT32)-1)
    {
        InspectablePtr valuePtr;
        IFR_ASSERT(jsonObject.GetValue(*Keys++, valuePtr.GetAddressOf()));
        IFR_ASSERT(pClass->SetPropertyInternal(propertyId, valuePtr, false));
        // go to next property id
        propertyId = va_arg(marker, UINT32);
    }
    va_end(marker);
    return S_OK;
}

class CImagesConfig :
    public ::_FastImagesConfigBase
{
public:
    HRESULT _InitializeModel(const DictionaryPtr& jsonObject)
    {
        const CHAR_t *_ImagesConfigKeys[] = {
            U("base_url"),
            U("secure_base_url"),
            U("backdrop_sizes"),
            U("logo_sizes"),
        };

        IFR_ASSERT(LoadJsonProperties(
            this->GetClassInterface(),
            jsonObject,
            _ImagesConfigKeys,
            ImagesConfig::Property_BaseUrl,
            ImagesConfig::Property_SecureBaseUrl,
            ImagesConfig::Property_BackdropSizes,
            ImagesConfig::Property_LogoSizes,
            (UINT32)-1
            ));
        return S_OK;
    }
};

class CConfiguration :
    public ::_FastConfigurationBase
{
public:
    HRESULT _InitializeModel(const std::string &json)
    {
        DictionaryPtr jsonObject;
        IFR_ASSERT(json_util::Parse_t(json.c_str(), nullptr, jsonObject));

        DictionaryPtr imagesObject;
        IFR_ASSERT(jsonObject.GetValue(U("images"), imagesObject));

        ComPtr<CImagesConfig> imagesConfigPtr;
        IFR_ASSERT(CreateObjectClass(
            ImagesConfig::IIDType,
            ToFreeThreadApartmentOptions(ObservableObjectOptions::None),
            imagesConfigPtr.GetAddressOf(),
            imagesObject));
        this->_imagesValue = imagesConfigPtr.Get();
        return S_OK;
    }
};

class CMovie :
    public ::_FastMovieBase
{
public:
    HRESULT _InitializeModel(const DictionaryPtr& jsonObject)
    {
        const CHAR_t *_MovieKeys[] = { 
            U("id"),
            U("title"),
            U("original_title"),
            U("release_date"),
            U("overview"),
            U("poster_path"),
            U("popularity"),
            U("vote_average"),
            U("vote_count"),
            U("video"),
            U("adult"),
        };

        IFR_ASSERT(LoadJsonProperties(
            this->GetClassInterface(),
            jsonObject,
            _MovieKeys,
            Movie::Property_Id,
            Movie::Property_Title,
            Movie::Property_OriginalTitle,
            Movie::Property_ReleaseDate,
            Movie::Property_Overview,
            Movie::Property_PosterUri,
            Movie::Property_Popularity,
            Movie::Property_VoteAverage,
            Movie::Property_VoteCount,
            Movie::Property_IsVideo,
            Movie::Property_IsAdult,
            (UINT32)-1
            ));
        return S_OK;
    }
};

class CPageMovies :
    public ::_FastPageMoviesBase
{
public:
    HRESULT _InitializeModel(const std::string &json)
    {
        DictionaryPtr jsonObject;
        IFR_ASSERT(json_util::Parse_t(json.c_str(), nullptr, jsonObject));

        const CHAR_t *_PageMoviesKeys[] = { U("page"), U("total_pages"), U("total_results") };
        IFR_ASSERT(LoadJsonProperties(
            this->GetClassInterface(),
            jsonObject,
            _PageMoviesKeys,
            PageMovies::Property_Page,
            PageMovies::Property_TotalPages,
            PageMovies::Property_TotalResults,
            (UINT32)-1
            ));
        IFR_ASSERT(CreateObservableCollection(
            ObservableCollection_IMovies::IIDType,
            ToFreeThreadApartmentOptions(ObservableCollectionOptions::IsReadOnly),
            _movies.GetAddressOf()
            ));
        // for public access QI to a type safe value
        IFR_ASSERT(foundation::QueryInterface(_movies, this->_moviesValue.GetAddressOf()));

        InspectableArrayWrapper resultsArray;
        IFR_ASSERT(jsonObject.GetValue(U("results"), resultsArray));
        for (UINT32 index = 0; index < resultsArray.GetSize(); ++index)
        {
            DictionaryPtr jsonMovieObject;
            IFR_ASSERT(::QueryInterface(resultsArray[index], jsonMovieObject.GetAddressOf()));

            ComPtr<CMovie> moviePtr;
            IFR_ASSERT(CreateObjectClass(
                Movie::IIDType,
                ToFreeThreadApartmentOptions(ObservableObjectOptions::None),
                moviePtr.GetAddressOf(),
                jsonMovieObject));
            _movies.AppendItemInternal(moviePtr->CastToInspectable());
        }
        return S_OK;
    }
private:
    ObservableCollectionClassPtr _movies;
};

class CMoviesApiService :
    public ::_FastMoviesApiServiceBase
{
public:
    HRESULT _InitializeModel(IDispatcher *pEventDispatcher)
    {
        _IFR_(_FastMoviesApiServiceBase::_InitializeModel());
        this->SetEventDispatcher(pEventDispatcher);
        return S_OK;
    }
protected:
    HRESULT GetNowPlayingAsync(INT32 page, HSTRING language, const foundation::AsyncOperationClassPtr& asyncOperationClassPtr) override
    {
        auto uri = web::http::uri_builder(_NowPlaying)
            .append_query(_Api_key, this->_apiKeyValue.GetRawBuffer())
            .append_query(U("page"), page);
        if (language != nullptr && !_pal_IsStringEmpty(language))
        {
            uri.append_query(U("language"), _pal_GetStringRawBuffer(language, nullptr));
        }
        auto request = JsonRequest(uri.to_string().c_str());
        request.then([=](const std::string &json) {

            ComPtr<CPageMovies> pageMoviesPtr;
            CreateObjectClass(
                PageMovies::IIDType,
                ToFreeThreadApartmentOptions(ObservableObjectOptions::None),
                pageMoviesPtr.GetAddressOf(),
                json);

            asyncOperationClassPtr.SetCompleted(S_OK, pageMoviesPtr->CastToInspectable());
        });
        return S_OK;
    }

    HRESULT GetConfigurationAsync(const foundation::AsyncOperationClassPtr& asyncOperationClassPtr)
    {
        auto uri = web::http::uri_builder(U("/configuration"))
            .append_query(_Api_key, this->_apiKeyValue.GetRawBuffer());
        auto request = JsonRequest(uri.to_string().c_str());
        request.then([=](const std::string &json) {
            ComPtr<CConfiguration> configurationPtr;
            CreateObjectClass(
                Configuration::IIDType,
                ToFreeThreadApartmentOptions(ObservableObjectOptions::None),
                configurationPtr.GetAddressOf(),
                json);

            asyncOperationClassPtr.SetCompleted(S_OK, configurationPtr->CastToInspectable());
        });
        return S_OK;
    }

private:
    pplx::task<std::string> JsonRequest(const CHAR_t *request)
    {
        // Buffer used in the task to read the response. 
        auto buffer = std::make_shared<Concurrency::streams::stringstreambuf>();

        // load client URL
        web::http::client::http_client client(_MoviesApiBase);

        // post request async
        auto task = client.request(web::http::methods::GET, request)
            .then([=](web::http::http_response response)
        {
            if (response.status_code() == web::http::status_codes::OK)
            {
                return response.body().read_to_end(*buffer);
            }
            else
            {
                return pplx::task<size_t>([=] {
                    size_t t(0);
                    return t; });
            }
        }).then([=](size_t size)
        {
            return buffer->collection();
        });
        return task;
    }
};

typedef ObjectFactory<CMoviesApiService, MoviesApiService::IIDType, ObservableObjectOptions::None,true> MoviesApiServiceFactory_type;


typedef _FastMoviesApiServiceFactoryBase CMoviesApiServiceFactory_BaseType;

class CMoviesApiServiceFactory :
    public CMoviesApiServiceFactory_BaseType
{
public:
    static IMoviesApiServiceFactory *GetInstance();
    HRESULT CreateMoviesApiServiceInternal(foundation::IInspectable * eventDispatcher, Movies::IMoviesApiService ** pResult) override
    {
        ComPtr<IDispatcher> dispatcherPtr;
        IFR_ASSERT(::QueryInterfaceIf(eventDispatcher, dispatcherPtr.GetAddressOf()));
        return MoviesApiServiceFactory_type::CreateInstanceAs(pResult, dispatcherPtr);
    }
};

IMoviesApiServiceFactory *CMoviesApiServiceFactory::GetInstance()
{
    return _GetFactoryInstance<CMoviesApiServiceFactory>(
        []() {
        RegisterMoviesIf();
        CMoviesApiServiceFactory *pFactory;
        CreateObjectClass(
            MoviesApiServiceFactory::IIDType,
            (ObservableObjectOptions)(FoundationClassOptions_NoTracking),
            &pFactory);
        return pFactory;
    });
}

extern "C" void _EnsureMoviesApiService()
{
}

static _RegisterActivationFactory _factoryRegister(
    U("pmod.MoviesApiService"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CMoviesApiServiceFactory::GetInstance)
    );

EXTERN_C API_EXPORT HRESULT  STDAPICALLTYPE CreateMoviesApiServiceFactory(IObservableObject **pp)
{
    *pp = CMoviesApiServiceFactory::GetInstance();
    (*pp)->AddRef();
    return S_OK;
}
