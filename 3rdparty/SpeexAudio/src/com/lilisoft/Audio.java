package com.lilisoft;

import java.nio.ByteBuffer;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

import org.xiph.speex.*;
import netscape.javascript.*;

public class Audio {

	private byte[] samples;
	private byte[] decoded;
	private SpeexDecoder speexDecoder;
	private int packetOffset;
	private int packetSize;
	private Bits bits;
	private SourceDataLine speaker = null;
	
	public void init() {

		speexDecoder = new SpeexDecoder();
		// arg1 is quality 0 for narrow band
		// arg2 is samplerate 8000 as defautl
		// arg3 is channels number only one for me
		// arg4 is enhanced mode activated or not
	    boolean state = speexDecoder.init(0,22050,1,true);
	    if (state) {
	    	// Init Bits struct
	    	bits = new Bits();
	    	bits.init();
	    }

		samples = new byte[256];
		decoded = new byte[256];
		
		packetOffset = 0;
		packetSize = 256;

		// Get sound audio
		
		try {
		    DataLine.Info speakerInfo = new DataLine.Info(SourceDataLine.class, getAudioFormat(256));
		    speaker = (SourceDataLine) AudioSystem.getLine(speakerInfo);
		} catch (LineUnavailableException e) {
		    e.printStackTrace();
		}
		
	}
	
	public void play() {
		try
		{
			speexDecoder.processData(samples, packetOffset, packetSize);
			speexDecoder.getProcessedData(decoded, 0);
			speaker.write(decoded, 0, speexDecoder.getProcessedDataByteSize());
		}
		catch (Exception e)
		{
			 System.out.println("test "+e.getMessage());
		}
	}
	
	public void setData(JSObject value) {
		// Get array buffer object
        Object len = value.getMember("length");
        if (len instanceof Number){
          // if length is ok
          int n = ((Number)len).intValue();
          for (int i = 0; i < n; i++){
        	  if (value.getSlot(i) instanceof Integer) {
            	  samples[i] = (byte)((Integer)value.getSlot(i) & 0xff);
            	  //System.out.println(samples[i]);
        	  }
          }
        }
		play();
	}

	private AudioFormat getAudioFormat(float sample) {
	    int sampleSizeBits = 16;
	    int channels = 1;
	    boolean signed = true;
	    boolean bigEndian = false;
	    return new AudioFormat(sample, sampleSizeBits, channels, signed, bigEndian);
	}	
}
