#pragma once

#include "streamfile.h"

typedef enum { OPUS_SWITCH, OPUS_UE4, OPUS_EA, OPUS_X } opus_type_t;

typedef struct {
	/* config */
	opus_type_t type;
	off_t stream_offset;
	size_t stream_size;

	/* state */
	off_t logical_offset;           /* offset that corresponds to physical_offset */
	off_t physical_offset;          /* actual file offset */

	size_t block_size;              /* current block size */
	size_t page_size;               /* current OggS page size */
	uint8_t page_buffer[0x2000];    /* OggS page (observed max is ~0xc00) */
	size_t sequence;                /* OggS sequence */
	size_t samples_done;            /* OggS granule */

	uint8_t head_buffer[0x100];     /* OggS head page */
	size_t head_size;               /* OggS head page size */

	size_t logical_size;
} opus_io_data;
size_t opus_io_read(STREAMFILE *streamfile, uint8_t *dest, off_t offset, size_t length, opus_io_data* data);

size_t opus_io_size(STREAMFILE *streamfile, opus_io_data* data);

STREAMFILE* setup_opus_streamfile(STREAMFILE *streamFile, int channels, int skip, int sample_rate, off_t stream_offset, size_t stream_size, opus_type_t type);
