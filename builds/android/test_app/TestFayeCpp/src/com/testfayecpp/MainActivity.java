package com.testfayecpp;

import android.app.Activity;
import android.content.ContextWrapper;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;


public class MainActivity extends Activity {

	static {
		//Log.e("Load stlport_shared library", "Load stlport_shared library");
		//System.loadLibrary("stlport_shared");
		//Log.e("Load stlport_shared library OK", "Load stlport_shared library OK");
		
        Log.e("Load fayecpp library", "Load fayecpp library");
		System.loadLibrary("fayecpp");
		Log.e("Load fayecpp library", "Load fayecpp library OK");
		
		Log.e("Load testfayecpp library", "Load testfayecpp library");
		System.loadLibrary("testfayecpp");
		Log.e("Load testfayecpp library", "Load testfayecpp library OK");
	}
	
	//  javah -classpath ../bin/classes:/Users/<USERNAME>/adt-bundle-mac-x86_64-20140702/sdk/platforms/android-16/android.jar -o TestFayeCpp.h com.testfayecpp.MainActivity
	//  0 = fail, 1 = done
	private native int initializeFayeClient();
	private native int connectFayeClient();
	private native int sentTestMessageFayeClient();
	private native int sentTextMessageFayeClient(String messageString);
	private native int disconnectFayeClient();
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        Log.e("onCreate", "onCreate");
        Button button = (Button)findViewById(R.id.button3);
        button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Log.e("", "onClick 3");
				// TODO Auto-generated method stub
				initializeFayeClient();
			}
		});
        
        button = (Button)findViewById(R.id.button1);
        button.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Log.e("", "onClick 1");
				// TODO Auto-generated method stub
				connectFayeClient();
			}
        });
        
        button = (Button)findViewById(R.id.button2);
        button.setOnClickListener(new View.OnClickListener() {
        	public void onClick(View v) {
        		TextView textView = (TextView)findViewById(R.id.editText1);
				Log.e("", "onClick 2");
				// TODO Auto-generated method stub
				String text = textView.getText().toString();
				sentTextMessageFayeClient(text);
			}
        });
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
