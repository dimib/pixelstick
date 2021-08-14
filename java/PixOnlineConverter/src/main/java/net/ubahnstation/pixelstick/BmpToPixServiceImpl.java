package net.ubahnstation.pixelstick;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.io.FileSystemResource;
import org.springframework.core.io.Resource;
import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;

@Service
public class BmpToPixServiceImpl implements BmpToPixService {

	@Autowired
	BMPConverter bmpConverter;
	
	public Resource convert(MultipartFile file) throws Exception {
		
		InputStream in = file.getInputStream();
		File tmpfile = File.createTempFile("tmp", ".pix");
		tmpfile.deleteOnExit();
		FileOutputStream out = new FileOutputStream(tmpfile);
		try {
			bmpConverter.convert(in, out);
		} catch (Exception e) {
			throw e;
		} finally {
			if (out !=  null) {
				out.close();
				out = null;
			}
			if (in != null) {
				in.close();
			}
		}
		
		Resource r = new FileSystemResource(tmpfile);
		
		return r;
	}

}
