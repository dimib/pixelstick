package net.ubahnstation.pixelstick;

import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

import javax.imageio.ImageIO;

import org.springframework.boot.autoconfigure.AutoConfigureBefore;
import org.springframework.stereotype.Component;

@Component
public class BMPConverter {

	
	public void convert(InputStream in, OutputStream out) throws Exception {
		BufferedImage image = ImageIO.read(in);
		Raster raster = image.getRaster();
		int width = raster.getWidth();
		int height = raster.getHeight();

		try {
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
	
	public void convert(File inputFile, File outputFile) throws Exception {

		if (!inputFile.exists()) {
			throw new FileNotFoundException(inputFile.getAbsolutePath() + " not found");
		}
		
		FileInputStream in = new FileInputStream(inputFile);
		FileOutputStream out = new FileOutputStream(outputFile);
		convert(in, out);
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
}
