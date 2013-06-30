package com.lilisoft;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.SourceDataLine;

import org.xiph.speex.*;

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
	    boolean state = speexDecoder.init(0,8000,1,false);
	    if (state) {
	    	// Init Bits struct
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
		}
	}
	
	public void setData(byte[] value) {
		samples = value;
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
