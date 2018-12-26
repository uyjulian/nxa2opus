#include <stdio.h>
#include "streamfile.h"
#include "nxa_streamfile.h"

#define BUFFERSIZE 0x8000
int main(int argc, const char * argv[]) {
	int retval = EXIT_FAILURE;
	if (argc <= 2) {
		printf("Usage: %s infile.nxa outfile.opus\n", argv[0]);
		goto failNothingOpen;
	}
	STREAMFILE *streamfile = open_stdio_streamfile(argv[1]);
	if (!streamfile) {
		printf("Failed to open file %s\n", argv[1]);
		goto failNothingOpen;
	}
	STREAMFILE *opusStreamfile = init_opus_nxa(streamfile);
	if (!opusStreamfile) {
		printf("%s wasn't a valid NXA file\n", argv[1]);
		goto failStreamfile;
	}
	FILE *outFile = fopen(argv[2], "w");
	if (!outFile) {
		printf("Couldn't open %s for writing\n", argv[2]);
		goto failOpusStreamfile;
	}
	size_t size = opusStreamfile->get_size(opusStreamfile);
	size_t pos = 0;
	uint8_t buffer[BUFFERSIZE];
	while (pos < size) {
		size_t amount = BUFFERSIZE < (size - pos) ? BUFFERSIZE : (size - pos);
		size_t read = opusStreamfile->read(opusStreamfile, buffer, pos, amount);
		pos += read;
		fwrite(buffer, 1, read, outFile);
	}
	
	retval = EXIT_SUCCESS;

	fclose(outFile);
failOpusStreamfile:
	close_streamfile(opusStreamfile);
failStreamfile:
	close_streamfile(streamfile);
failNothingOpen:
	return retval;
}
