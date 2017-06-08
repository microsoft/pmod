/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MovieHelper.java
****/
package com.microsoft.pmod.moviesui;

import android.graphics.Bitmap;
import android.view.View;
import android.widget.ImageView;

import com.microsoft.movies.Movie;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.listener.SimpleImageLoadingListener;

/**
 * Created by rodrigov on 4/26/2016.
 */
public class MovieHelper {

    static void setImagePoster(View v,Movie movie,int resId)
    {
        final ImageView iv = (ImageView) v.findViewById(resId);
        if (iv != null) {
            String imageUri = MoviesUIApp.getMoviesConfiguration().getImages().getBaseUrl() + "w342" + movie.getPosterUri();
            ImageLoader imageLoader = ImageLoader.getInstance();
            imageLoader.loadImage(imageUri, new SimpleImageLoadingListener() {
                @Override
                public void onLoadingComplete(String imageUri, View view, Bitmap loadedImage) {
                    iv.setImageBitmap(loadedImage);
                }
            });
        }
    }
}
