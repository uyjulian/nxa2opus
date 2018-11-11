#include "nxa_streamfile.h"
#include "nxa_opus.h"

/* Entergram NXA Opus [Higurashi no Naku Koro ni Hou (Switch)] */
STREAMFILE *init_opus_nxa(STREAMFILE *streamFile) {
//	VGMSTREAM * vgmstream = NULL;
	off_t start_offset;
	int loop_flag = 0, channel_count, skip = 0;
	size_t data_size;

	/* checks */
	if (read_32bitBE(0x00, streamFile) != 0x4E584131) /* "NXA1" */
		goto fail;

	channel_count = read_16bitLE(0x10, streamFile);
	skip = read_16bitLE(0x16, streamFile);
	data_size = read_32bitLE(0x08, streamFile)-0x30;
	start_offset = 0x30;

	/* TODO: Determine if loop points are stored externally. No visible loop points in header */
	loop_flag = 0;


	/* build the VGMSTREAM */
//	vgmstream = allocate_vgmstream(channel_count, loop_flag);
//	if (!vgmstream) goto fail;
//
//	vgmstream->meta_type = meta_NXA;
//	vgmstream->num_samples = read_32bitLE(0x20, streamFile);
	int sample_rate = read_32bitLE(0x0C, streamFile);

//	{
//		vgmstream->codec_data = init_ffmpeg_switch_opus(streamFile, start_offset, data_size, vgmstream->channels, skip, vgmstream->sample_rate);
//		if (!vgmstream->codec_data) goto fail;
//		vgmstream->coding_type = coding_FFmpeg;
//		vgmstream->layout_type = layout_none;
//
//		if (vgmstream->num_samples == 0) {
//			vgmstream->num_samples = switch_opus_get_samples(start_offset, data_size, streamFile) - skip;
//		}
//	}

	return setup_opus_streamfile(streamFile, channel_count, skip, sample_rate, start_offset, data_size, OPUS_SWITCH);
	/* open the file for reading */
//	if (!vgmstream_open_stream(vgmstream, streamFile, start_offset))
//		goto fail;
//	return vgmstream;
fail:
//	close_vgmstream(vgmstream);
	return NULL;
}
