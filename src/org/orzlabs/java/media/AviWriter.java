package org.orzlabs.java.media;

import android.util.Log;


public class AviWriter {
	static {
		System.loadLibrary("jpegtoavi-jni");
	}
	private String[] args;
	private int argsLength;
	public AviWriter(String[] arguments) {
		// TODO Auto-generated constructor stub
		args = arguments;
		argsLength = arguments.length;
	}
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		AviWriter j2at = new AviWriter(args);
		j2at.execute();
	}
	public void execute() {
		Log.d("AviWriter", "retcode:" + jpegToAviJNI()); 
//		// TODO Auto-generated method stub
//		File outFile = new File(args[args.length - 1]);
//		try {
//			AviWriter aviWriter = new AviWriter(outFile, 2, true);
//			
//			for (int i = 0; i < args.length - 1; i++) {
//				File inFile = new File(args[i]);
//				FileInputStream fis = new FileInputStream(inFile);
//				long size = inFile.length();
//				System.out.println("size:" + size);
//				ByteArrayOutputStream baos = new ByteArrayOutputStream();
//				byte[] buffer = new byte[2048];
//				byte[] trash = new byte[10];
//				
////				fis.read(buffer, 0, 6);
////				baos.write(buffer, 5, 1);
////				for (int j = 0; j < buffer.length; j++) {
////					System.out.print(buffer[j]);
////				}
////				//fis.read(trash, 0, 5);
////				fis.read(trash, 0, 4);
////				byte[] avi1 = {'A', 'V', 'I', '1'};
////				baos.write(avi1);
//				int readSize;
//				while ((readSize = fis.read(buffer, 0, 2048)) != -1) {
//					baos.write(buffer, 0, readSize);
//				}
//				aviWriter.writeFrame(baos.toByteArray());
//				fis.close();
//			}
//			aviWriter.setFramesPerSecond(1, 1);
//			aviWriter.setSamplesPerSecond(1);
//			aviWriter.setDimensions(1, 1);
//			aviWriter.close();
//		} catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
	}
	
	public native int jpegToAviJNI();


}
