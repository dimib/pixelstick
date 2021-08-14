package net.ubahnstation.pixelstick;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.autoconfigure.EnableAutoConfiguration;
import org.springframework.core.io.Resource;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.multipart.MultipartFile;

@Controller
@EnableAutoConfiguration
public class BmpToPixController {

	private final BmpToPixService bmpToPixService;
	
	@Autowired
	public BmpToPixController(BmpToPixService bmpToPixService) {
		// TODO Auto-generated constructor stub
		this.bmpToPixService = bmpToPixService;
	}
	
	// @PostMapping(path="/convert", produces = "binary/octet-stream")
	@PostMapping("/convert")
	@ResponseBody()
	public ResponseEntity<Resource> convert(@RequestParam("file") MultipartFile file) throws Exception {

		Resource pixFile = bmpToPixService.convert(file);
		return ResponseEntity.ok().header(HttpHeaders.CONTENT_DISPOSITION,
	            "attachment; filename=\"convert.pix\"").body(pixFile);
	}
	
	
	
	String body = 
			"<html><body>" 
			+ "<form method=\"POST\" enctype=\"multipart/form-data\" action=\"/convert\">"
			+ "File to upload: <input type=\"file\" name=\"file\"/><br/>"
			+ "<input type=\"submit\" value=\"Upload\"/>"
			+ "</form>"
			+ "</body></html>";
	
	@RequestMapping("/convert")
	@ResponseBody
	public String home() {
		return (body);
	}
}
