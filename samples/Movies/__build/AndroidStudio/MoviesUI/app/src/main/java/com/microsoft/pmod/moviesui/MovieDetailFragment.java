/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MovieDetailFragment.java
****/
package com.microsoft.pmod.moviesui;

import android.app.Activity;
import android.support.design.widget.CollapsingToolbarLayout;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.microsoft.movies.Movie;
import com.microsoft.pmod.*;

/**
 * A fragment representing a single Movie detail screen.
 * This fragment is either contained in a {@link MovieListActivity}
 * in two-pane mode (on tablets) or a {@link MovieDetailActivity}
 * on handsets.
 */
public class MovieDetailFragment extends Fragment {
    /**
     * The fragment argument representing the item ID that this fragment
     * represents.
     */
    public static final String ARG_ITEM_ID = "item_id";

    /**
     * The dummy content this fragment is presenting.
     */
    private Movie mItem;

    /**
     * Mandatory empty constructor for the fragment manager to instantiate the
     * fragment (e.g. upon screen orientation changes).
     */
    public MovieDetailFragment() {
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (getArguments().containsKey(ARG_ITEM_ID)) {

            Activity activity = this.getActivity();
            ObservableCollection<Movie> movies = ((MoviesUIApp)this.getActivity().getApplicationContext()).getNowPlayingMovies().getMovies();

            int movieId = getArguments().getInt(ARG_ITEM_ID);
            for(int i=0;i < movies.size();++i )
            {
                if(movies.getItem(i).getId() == movieId)
                {
                    this.mItem = movies.getItem(i);
                    CollapsingToolbarLayout appBarLayout = (CollapsingToolbarLayout) activity.findViewById(R.id.toolbar_layout);
                    if (appBarLayout != null) {
                        appBarLayout.setTitle(mItem.getTitle());
                    }
                    break;
                }
            }
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View rootView = inflater.inflate(R.layout.fragment_movie_detail, container, false);

        // Show the dummy content as text in a TextView.
        if (mItem != null) {
            MovieHelper.setImagePoster(rootView,mItem,R.id.imageView);

            String ratingStr = String.format("Rating:%.2f/10.0 from %d votes",
                    mItem.getVoteAverage(),
                    mItem.getVoteCount());
            ((TextView) rootView.findViewById(R.id.textRating)).setText(ratingStr);
            ((TextView) rootView.findViewById(R.id.textOverview)).setText(mItem.getOverview());
        }

        return rootView;
    }
}
