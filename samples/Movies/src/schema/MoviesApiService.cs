/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoviesApiService.cs
****/
using PropertyModel;
using System;

namespace Movies
{
    public interface IMovie
    {
        int Id { get; }
        string Title { get; }
        string OriginalTitle { get; }
        DateTime ReleaseDate { get; }
        string[] Generes { get; }
        string Overview { get; }
        string PosterUri { get; }
        float Popularity { get; }
        float VoteAverage { get; }
        int VoteCount { get; }
        bool IsVideo { get; }
        bool IsAdult { get; }
    }

    [CollectionAttribute(ItemType = typeof(IMovie))]
    public interface ICollection_IMovies
    {
    }
    public interface IPageMovies
    {
        int Page { get; }
        int TotalPages { get; }
        int TotalResults { get; }
        ICollection_IMovies Movies { get; }
    }

    public interface IImagesConfig
    {
        string BaseUrl { get; }
        string SecureBaseUrl { get; }
        string[] BackdropSizes { get; }
        string[] LogoSizes { get; }
    }

    public interface IConfiguration
    {
        IImagesConfig Images { get; }
    }

    public interface IMoviesApiService
    {
        /// <summary>
        /// Api key for Rotten tomatoes service
        /// </summary>
        string ApiKey { get; set; }

        [MethodAttribute(IsAsync = true)]
        IConfiguration GetConfiguration();

        [MethodAttribute(IsAsync = true)]
        IPageMovies GetNowPlaying(int page, string language);
    }

    public interface IMoviesApiServiceFactory
    {
        IMoviesApiService CreateMoviesApiService(object eventDispatcher);
    }
}
