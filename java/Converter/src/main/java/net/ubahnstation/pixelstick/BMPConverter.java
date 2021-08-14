package net.ubahnstation.pixelstick;

import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;

import javax.imageio.ImageIO;

public class BMPConverter {

	final String[] args; 
	
	public BMPConverter(String[] args) {
		this.args = args;
	}
	
	public int run() {
		
		for (String f : args) {
			File inputFile = new File(f);
			File outputFile = new File(inputFile.getParent(), inputFile.getName().substring(0, inputFile.getName().lastIndexOf('.')) + ".pix");
			try {
				convert(inputFile, outputFile);
			} catch(Exception e) {
				System.err.println(inputFile.getAbsolutePath() + ": Could not convert! " + e.getMessage());
			}
		}
		return 0;
	}
	
	public void convert(File inputFile, File outputFile) throws Exception {

		if (!inputFile.exists()) {
			throw new FileNotFoundException(inputFile.getAbsolutePath() + " not found");
		}
		
		BufferedImage image = ImageIO.read(inputFile);
		Raster raster = image.getRaster();
		int width = raster.getWidth();
		int height = raster.getHeight();

		FileOutputStream out = null;
		try {
			out = new FileOutputStream(outputFile);
			out.write("PIX".getBytes());
			out.write(intToByte(width));
			out.write(intToByte(height));
			out.write("DATA".getBytes());
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					int[] pixel = null;
					pixel = raster.getPixel(x, y, pixel);
					out.write(pixIt(pixel[0], pixel[1], pixel[2]));
				}				
			}
		} finally {
			if (out != null) {
				out.close();
				out = null;
			}
		}
	}
	
	private byte[] intToByte(int in) {
		byte[] b = new byte[4];
		b[0] = (byte) ((in >> 24) & 0xff);
		b[1] = (byte) ((in >> 16) & 0xff);
		b[2] = (byte) ((in >> 8) & 0xff);
		b[3] = (byte) ((in) & 0xff);
		return b;
	}
	private byte[] intToByte3(int in) {
		byte[] b = new byte[3];
		b[0] = (byte) ((in >> 16) & 0xff);
		b[1] = (byte) ((in >> 8) & 0xff);
		b[2] = (byte) ((in) & 0xff);
		return b;
	}
	
	private byte[] pixIt(int r, int g, int b) {
		byte[] pix = new byte[4];
		pix[0] = (byte)(r & 0xff);
		pix[1] = (byte)(g & 0xff);
		pix[2] = (byte)(b & 0xff);
		pix[3] = (byte)0;
		return pix;
	}
	
	
	public static void main(String[] args) {
		BMPConverter c = new BMPConverter(args);
		System.exit(c.run());
	}

}
