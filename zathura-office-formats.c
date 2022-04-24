#include <openssl/md5.h>
#include <poppler.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <zathura/document.h>
#include <zathura/plugin-api.h>

#include "plugin.h"

#define MD5_LEN (MD5_DIGEST_LENGTH*2)

zathura_error_t officeDocOpen(zathura_document_t*);

ZATHURA_PLUGIN_REGISTER_WITH_FUNCTIONS(
	PLUGIN_NAME,
	VERSION_MAJ,
	VERSION_MIN,
	VERSION_REV,
	ZATHURA_PLUGIN_FUNCTIONS({
		.document_open            = officeDocOpen,
		.document_free            = pdf_document_free,
		.document_index_generate  = pdf_document_index_generate,
		.document_save_as         = pdf_document_save_as,
		.document_attachments_get = pdf_document_attachments_get,
		.document_attachment_save = pdf_document_attachment_save,
		.document_get_information = pdf_document_get_information,
		.page_init                = pdf_page_init,
		.page_clear               = pdf_page_clear,
		.page_search_text         = pdf_page_search_text,
		.page_links_get           = pdf_page_links_get,
		.page_form_fields_get     = pdf_page_form_fields_get,
		.page_images_get          = pdf_page_images_get,
		.page_get_text            = pdf_page_get_text,
		.page_render_cairo        = pdf_page_render_cairo,
		.page_image_get_cairo     = pdf_page_image_get_cairo,
		.page_get_label           = pdf_page_get_label
	}),
	ZATHURA_PLUGIN_MIMETYPES({
		"application/vnd.openxmlformats-officedocument.wordprocessingml.document",
		"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
		"application/vnd.openxmlformats-officedocument.presentationml.presentation",
		"application/msword",
		"application/vnd.ms-excel",
		"application/vnd.ms-powerpoint",
		"application/vnd.oasis.opendocument.text",
		"application/vnd.oasis.opendocument.spreadsheet",
		"application/vnd.oasis.opendocument.presentation"
	})
)

void md5sum(const char *input, unsigned char output[MD5_LEN]) {
	unsigned char temp[MD5_DIGEST_LENGTH];
	MD5_CTX context;
	MD5_Init(&context);
	MD5_Update(&context, input, strlen(input));
	MD5_Final(temp, &context);
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(output+2*i, "%02x", temp[i]);
	}
	return;
}

zathura_error_t officeDocOpen(zathura_document_t *doc) {
	if (doc == NULL) return ZATHURA_ERROR_INVALID_ARGUMENTS;

	const char *inputPath = zathura_document_get_path(doc);

	unsigned char md5[MD5_LEN];
	md5sum(inputPath, md5);

	char *home = getenv("HOME");
	char pdfDir[strlen(home)+strlen(PDF_DIR)+1];
	char tmpDir[strlen(home)+strlen(TMP_DIR)+1];
	if (PDF_DIR[0] == '~' && PDF_DIR[1] == '/') sprintf(pdfDir, "%s%s", home, PDF_DIR+1);
	else sprintf(pdfDir, "%s", PDF_DIR);
	if (TMP_DIR[0] == '~' && TMP_DIR[1] == '/') sprintf(tmpDir, "%s%s", home, TMP_DIR+1);
	else sprintf(tmpDir, "%s", TMP_DIR);

	char *opFormat = "%s/%s.pdf";
	char outputPath[strlen(opFormat)-2*2+strlen(pdfDir)+MD5_LEN+1];
	sprintf(outputPath, opFormat, pdfDir, md5);

	struct stat statbuf;
	if (stat(outputPath, &statbuf) == 0) {
		printf("using cached pdf file for %s\n", inputPath);
	} else {
		const char *cmdFormat = "libreoffice --convert-to pdf \"%s\" --outdir \"%s\" && mv \"%s/$(ls -t \"%s\" | head -1)\" \"%s\"";
		char cmd[strlen(cmdFormat)-2*4+strlen(inputPath)+strlen(tmpDir)*3+strlen(outputPath)+1];
		sprintf(cmd, cmdFormat, inputPath, tmpDir, tmpDir, tmpDir, outputPath);
		int ret = system(cmd);
		if (ret != 0) return ZATHURA_ERROR_UNKNOWN;
	}

	char *uri = g_filename_to_uri(outputPath, NULL, NULL);
	if (uri == NULL) return ZATHURA_ERROR_UNKNOWN;

	PopplerDocument *popDoc = poppler_document_new_from_file(uri, NULL, NULL);

	g_free(uri);

	if (popDoc == NULL) return ZATHURA_ERROR_UNKNOWN;

	zathura_document_set_data(doc, popDoc);
	zathura_document_set_number_of_pages(doc, poppler_document_get_n_pages(popDoc));

	return ZATHURA_ERROR_OK;
}
