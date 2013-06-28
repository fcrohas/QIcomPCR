package com.lilisoft;

import java.applet.Applet;

@SuppressWarnings("serial")
public class Main extends Applet {
	
	static final String appletInfo = "SpeexAudio Stream playing Applet";

	public String getAppletInfo() {
		return appletInfo;
	}
	
	public void init() {
		// Do init here if needed
	}
	
	public void start()
	{

	}
	
    public void stop() {
    	
    }	
    
    public Audio getAudio() {
    	return new Audio();
    }
	
}

