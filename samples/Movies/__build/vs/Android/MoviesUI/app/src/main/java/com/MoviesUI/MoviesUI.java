/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoviesUI.java
****/

package com.MoviesUI;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;
import com.microsoft.pmod.*;
import com.microsoft.movies.*;
import android.util.Log;

public class MoviesUI extends Activity
{
    private static final String DEBUG_TAG= "MoviesUI";

    static {
        System.loadLibrary("pmod");
        System.loadLibrary("pmod_lib");
        System.loadLibrary("movies");
    }
    static MoviesApiService apiService;

    void getNowPlaying()
    {
        final AsyncOperation<PageMovies> pageMoviesResult = apiService.getNowPlaying(1,"");

        pageMoviesResult.addCompletedListener(new CompletedEventListener(){    //this is fine
            @Override
            public void onCompleted(CompletedEvent eventArgs){
                PageMovies pageMovies = pageMoviesResult.getResult();
            }
        });
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        ClassFactory factory = new ClassFactory(FactoryTypes.getTypes());
        MoviesApiServiceFactory apiServiceFactory = factory.<MoviesApiServiceFactory>getActivationFactory("pmod.MoviesApiService");

        Dispatcher uiDispatcher = new Dispatcher(new DispatcherCallback() {

            @Override
            public boolean hasThreadAccess() {
                return false;
            }

            @Override
            public void runAsync(Runnable runnable) {
                Log.d(DEBUG_TAG,"runAsync");
                runOnUiThread(runnable);
             }
        });

        apiService = apiServiceFactory.createMoviesApiService(uiDispatcher);
        apiService.setApiKey("60f44851c4211b396f346eec9db29c22");

        getNowPlaying();

        /* Create a TextView and set its text to "Hello world" */
        TextView  tv = new TextView(this);
        tv.setText("Hello World!");
        setContentView(tv);
    }
}
