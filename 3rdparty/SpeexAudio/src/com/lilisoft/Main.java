package com.lilisoft;

import java.applet.Applet;
import netscape.javascript.*;

@SuppressWarnings("serial")
public class Main extends Applet {
	
	private Audio sound;
	static final String appletInfo = "SpeexAudio Stream playing Applet";

	public String getAppletInfo() {
		return appletInfo;
	}
	
	public void init() {
		// Do init here if needed
		sound = new Audio();
		sound.init();
	}
	
	public void start()
	{

	}
	
    public void stop() {
    	
    }	
    
    public void getAudio(JSObject data) {
    	System.out.println("getaudio method called");
    	sound.setData(data);
    	System.out.println("done");
    }
	
}

