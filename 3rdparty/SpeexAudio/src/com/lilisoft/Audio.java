package com.lilisoft;

import org.xiph.speex.*;

public class Audio {

	private byte[] samples;
	private SpeexDecoder speexDecoder;
	private int packetOffset;
	private int packetSize;
	
	public void init() {
		speexDecoder = new SpeexDecoder();
		samples = new byte[256];
		packetOffset = 0;
		packetSize = 0;
	}
	
	public void play() {
		try
		{
	
			int mode = 1; // wide band
			int quality = 10; // high quality
	
			long timecode = 0;
			int numSamples = 0;
			speexDecoder.processData(samples, packetOffset, packetSize);
			byte[] decoded = new byte[speexDecoder.getProcessedDataByteSize()];
			speexDecoder.getProcessedData(decoded, 0);				
			//speexDecoder.init(mode, quality, sampleRate, channels);
			/*
			if (speexDecoder.processData(packet, 0, packet.length))
			{
				int encsize = speexDecoder.getProcessedData(packet, 0);
				play(packet);
			}*/
	
		}
		catch (Exception e)
		{
		}
	}
	
	public void setData(byte[] value) {
		samples = value;
	}
	
}
