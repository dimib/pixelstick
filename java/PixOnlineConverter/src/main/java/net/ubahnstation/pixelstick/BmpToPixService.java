package net.ubahnstation.pixelstick;

import org.springframework.core.io.Resource;
import org.springframework.web.multipart.MultipartFile;


public interface BmpToPixService {

	Resource convert(MultipartFile file) throws Exception;

}
