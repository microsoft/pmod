/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MainActivity.java
****/
package com.microsoft.pmod.demoappui;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.view.ViewGroup;
import android.content.Context;
import android.view.LayoutInflater;
import android.widget.*;
import java.beans.PropertyChangeEvent;

import com.microsoft.demoapp.*;
import com.microsoft.pmod.*;

public class MainActivity extends AppCompatActivity {
    private AppDemo app;
    private EditText editTextName;
    private int selectedItem = 0;

    static {
        System.loadLibrary("pmod");
        System.loadLibrary("demoapp_impl");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // bootstrap our DemoApp instance
        long pInterface = DemoAppFactory.createInstance();

        ClassFactory factory = new ClassFactory(FactoryTypes.getTypes());
        this.app = factory.getOrCreateObject(pInterface);

        setContentView(R.layout.activity_main);

        this.editTextName = (EditText) findViewById(R.id.editTextName);
        this.editTextName.setText("rodrigov@microsoft.com", TextView.BufferType.EDITABLE);

        final ListView itemsListView = (ListView)findViewById(R.id.listViewItems);
        itemsListView.setAdapter(new ItemsAdapter(this,this.app.getItems()));
        // Wire up the change button
        final Button buttonChange = (Button) findViewById(R.id.buttonChange);
        buttonChange.setEnabled(false);


        buttonChange.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                String name = editTextName.getText().toString();
                Item item = (Item)app.getItems().getItem(selectedItem);
                item.setName(name);
            } });

        itemsListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int position,
                                    long arg3) {
                selectedItem = position;
                buttonChange.setEnabled(true);
            }
        });


        // Wire up the add button
        Button buttonAdd = (Button) findViewById(R.id.buttonAdd);

        buttonAdd.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                String name =editTextName.getText().toString();
                if(name != null && name.length()>0)
                {
                    app.getAddItem().execute(name);
                }
            } });

        // Wire up the clear button
        Button buttonClear = (Button) findViewById(R.id.buttonClear);
        buttonClear.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                app.getClearItems().execute();
                buttonChange.setEnabled(false);
                itemsListView.setSelected(false);
                selectedItem = -1;
            } });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public abstract class ObservableCollectionAdapter<T> extends BaseAdapter {

        ObservableCollection<T> observableCollection;
        NotifyCollectionChangedListener listener;

        public ObservableCollectionAdapter(ObservableCollection<T> observableCollection) {
            this.observableCollection = observableCollection;
            final ObservableCollectionAdapter<T> _this = this;
            // test event support
            this.listener = new NotifyCollectionChangedListener() {

                @Override
                public void notifyCollectionChanged(
                        NotifyCollectionChangedEvent notifyCollectionChangedEvent) {
                    _this.notifyDataSetChanged();
                }
            };
            this.observableCollection.addNotifyCollectionChangedListener(this.listener);
        }

        public int getCount() {
            return this.observableCollection.size();
        }

        public Object getItem(int position) {
            return this.observableCollection.getItem(position);
        }

        public long getItemId(int position) {
            return position;
        }
    }

    public class ItemsAdapter extends ObservableCollectionAdapter<Item> {

        Context _context;
        public ItemsAdapter(Context context,ObservableCollection<Item> observableCollection) {
            super(observableCollection);
            this._context = context;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            Item item = (Item)this.getItem(position);

            return new ItemView(_context,item);
        }
        @Override
        public boolean areAllItemsEnabled()
        {
            return true;
        }

        @Override
        public boolean isEnabled(int arg0)
        {
            return true;
        }
    }

    public class ItemView extends RelativeLayout implements java.beans.PropertyChangeListener {

        private Item _item;
        private TextView _nameView;

        public ItemView(Context context,Item item) {
            super(context);
            LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);

            addView(inflater.inflate(R.layout.item_layout, null));
            _nameView = (TextView)findViewById(R.id.textViewName);

            this._item = item;
        }

        @Override
        protected void onAttachedToWindow ()
        {
            assert _item != null;
            _item.addPropertyChangeListener(this);
            _nameView.setText(_item.getName());
            super.onAttachedToWindow();
        }
        @Override
        protected void onDetachedFromWindow ()
        {
            _item.removePropertyChangeListener(this);
            _item = null;
            super.onDetachedFromWindow();
        }

        @Override
        public void propertyChange(PropertyChangeEvent event) {
            if(event.getPropertyName().equals("Name"))
            {
                _nameView.setText(event.getNewValue().toString());
            }
        }
    }
}
