/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoviesUIApp.java
****/
package com.microsoft.pmod.moviesui;

import android.app.Application;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

import com.microsoft.movies.*;
import com.microsoft.pmod.*;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;

/**
 * Created by rodrigov on 4/25/2016.
 */
public class MoviesUIApp extends Application {
    static {
        System.loadLibrary("pmod");
        System.loadLibrary("pmod_lib");
        System.loadLibrary("movies");
    }
    private static final String DEBUG_TAG= "MoviesUI";

    static MoviesApiService apiService;
    static Configuration mMoviesConfiguration;
    PageMovies nowPlayingMovies;

    public PageMovies getNowPlayingMovies()
    {
        return nowPlayingMovies;
    }

    public static Configuration getMoviesConfiguration()
    {
        return mMoviesConfiguration;
    }

    private Activity mCurrentActivity = null;
    public Activity getCurrentActivity(){
        return mCurrentActivity;
    }

    public void setCurrentActivity(Activity mCurrentActivity){
        this.mCurrentActivity = mCurrentActivity;
    }

    public void onCreate() {
        super.onCreate();
        ClassFactory factory = new ClassFactory(FactoryTypes.getTypes());
        MoviesApiServiceFactory apiServiceFactory = factory.<MoviesApiServiceFactory>getActivationFactory("pmod.MoviesApiService");

        Dispatcher uiDispatcher = new Dispatcher(new DispatcherCallback() {

            @Override
            public boolean hasThreadAccess() {
                return false;
            }

            @Override
            public void runAsync(Runnable runnable) {
                Log.d(DEBUG_TAG, "runAsync");
                if(mCurrentActivity != null) {
                    mCurrentActivity.runOnUiThread(runnable);
                }
                else
                {
                    runnable.run();
                }
            }

        });
        apiService = apiServiceFactory.createMoviesApiService(uiDispatcher);
        apiService.setApiKey("60f44851c4211b396f346eec9db29c22");
        getMoviesConfigurationAsync();

        // Create global configuration and initialize ImageLoader with this config
        ImageLoaderConfiguration config = new ImageLoaderConfiguration.Builder(this).build();
        ImageLoader.getInstance().init(config);
    }

    public AsyncOperation<PageMovies> getNowPlayingAsync()
    {
        final AsyncOperation<PageMovies> pageMoviesResult = apiService.getNowPlaying(1, "");
        pageMoviesResult.addCompletedListener(new CompletedEventListener() {    //this is fine
            @Override
            public void onCompleted(CompletedEvent eventArgs) {
                nowPlayingMovies = pageMoviesResult.getResult();
            }
        });
        return pageMoviesResult;
    }

    void getMoviesConfigurationAsync()
    {
        final AsyncOperation<Configuration> configurationResult = apiService.getConfiguration();

        configurationResult.addCompletedListener(new CompletedEventListener() {
            @Override
            public void onCompleted(CompletedEvent eventArgs) {
                Log.d(DEBUG_TAG, "getMoviesConfiguration->Completed");
                mMoviesConfiguration = configurationResult.getResult();
            }
        });
    }
}
