#include "nxa_streamfile.h"
#include "nxa_opus.h"

/* Entergram NXA Opus [Higurashi no Naku Koro ni Hou (Switch)] */
STREAMFILE *init_opus_nxa(STREAMFILE *sf) {
#if 0
//	VGMSTREAM * vgmstream = NULL;
	off_t start_offset;
	int loop_flag = 0, channel_count, skip = 0;
	size_t data_size;

	/* checks */
	if (read_32bitBE(0x00, streamFile) != 0x4E584131) /* "NXA1" */
		goto fail;

	int version = read_32bitLE(0x04, streamFile);
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

	int samples = read_32bitLE(0x18, streamFile);
	int loopstart = read_32bitLE(0x1c, streamFile);
	int loopend = read_32bitLE(0x20, streamFile);
	if (loopstart == 0 && loopend == samples) {
		loopend = 0;
	}

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
#endif
#if 0
    VGMSTREAM* vgmstream = NULL;
#endif
    off_t start_offset;
    int loop_flag = 0, channels, sample_rate;
    off_t data_offset, context_offset, multistream_offset = 0;
    size_t data_size, skip = 0;
    off_t offset = 0;
    int32_t num_samples;
    int32_t loop_start;
    int32_t loop_end;

    /* header chunk */
    if (read_u32le(offset + 0x00,sf) != 0x80000001) /* 'basic info' chunk */
        goto fail;
    /* 0x04: chunk size (should be 0x24) */

    /* 0x08: version (0) */
    channels = read_u8(offset + 0x09, sf);
    /* 0x0a: frame size if CBR, 0 if VBR */
    sample_rate = read_u32le(offset + 0x0c,sf);
    data_offset = read_u32le(offset + 0x10, sf);
    /* 0x14: 'frame data offset' (seek table? not seen) */
    context_offset = read_u32le(offset + 0x18, sf);
    skip = read_u16le(offset + 0x1c, sf); /* pre-skip sample count */
    /* 0x1e: officially padding (non-zero in Lego Movie 2 (Switch)) */
    /* (no offset to multistream chunk, maybe meant to go after seek/context chunks?) */

    /* 0x80000002: 'offset info' chunk (seek table?), not seen */

    /* 'context info' chunk, rare [Famicom Detective Club (Switch), SINce Memories (Switch)] */
    if (context_offset && read_u32le(offset + context_offset, sf) == 0x80000003) {
        /* maybe should give priority to external info? */
        context_offset += offset;
        /* 0x08: null*/
        loop_flag   = read_u8   (context_offset + 0x09, sf);
        num_samples = read_s32le(context_offset + 0x0c, sf); /* slightly smaller than manual count */
        loop_start  = read_s32le(context_offset + 0x10, sf);
        loop_end    = read_s32le(context_offset + 0x14, sf);
        /* rest (~0x38) reserved/alignment? */
        /* values seem to take encoder delay into account */
    }
    else {
        loop_flag = (loop_end > 0); /* -1 when not set */
    }


    /* 'multistream info' chunk, rare [Clannad (Switch)] */
    if (read_u32le(offset + 0x20, sf) == 0x80000005) {
        multistream_offset = offset + 0x20;
    }

    /* Opus can only do 48000 but some games store original rate [Grandia HD Collection, Lego Marvel] */
    if (sample_rate != 48000) {
        // VGM_LOG("OPUS: ignored non-standard sample rate of %i\n", sample_rate);
        sample_rate = 48000;
    }


    /* 'data info' chunk */
    data_offset += offset;
    if (read_u32le(data_offset, sf) != 0x80000004)
    {
        printf("Data info fail\n");
        goto fail;
    }

    data_size = read_u32le(data_offset + 0x04, sf);

    start_offset = data_offset + 0x08;


#if 0
    /* build the VGMSTREAM */
    vgmstream = allocate_vgmstream(channels,loop_flag);
    if (!vgmstream) goto fail;

    vgmstream->meta_type = meta_type;
    vgmstream->sample_rate = sample_rate;
    vgmstream->num_samples = num_samples;
    vgmstream->loop_start_sample = loop_start;
    vgmstream->loop_end_sample = loop_end;
    vgmstream->stream_size = data_size; /* to avoid inflated sizes from fake OggS IO */

#ifdef VGM_USE_FFMPEG
    {
        opus_config cfg = {0};

        cfg.channels = vgmstream->channels;
        cfg.skip = skip;
        cfg.sample_rate = vgmstream->sample_rate;

        if (multistream_offset && vgmstream->channels <= 8) {
            int i;
            cfg.stream_count = read_u8(multistream_offset + 0x08,sf);
            cfg.coupled_count = read_u8(multistream_offset + 0x09,sf); /* stereo streams */
            for (i = 0; i < vgmstream->channels; i++) {
                cfg.channel_mapping[i] = read_u8(multistream_offset + 0x0a + i,sf);
            }
        }

        vgmstream->codec_data = init_ffmpeg_switch_opus_config(sf, start_offset,data_size, &cfg);
        if (!vgmstream->codec_data) goto fail;
        vgmstream->coding_type = coding_FFmpeg;
        vgmstream->layout_type = layout_none;
        vgmstream->channel_layout = ffmpeg_get_channel_layout(vgmstream->codec_data);

        if (vgmstream->num_samples == 0) {
            vgmstream->num_samples = switch_opus_get_samples(start_offset, data_size, sf) - skip;
        }
    }
#else
    goto fail;
#endif
#endif

    return setup_opus_streamfile(sf, channels, skip, sample_rate, start_offset, data_size, OPUS_SWITCH, loop_start, loop_end);

	// return setup_opus_streamfile(streamFile, channel_count, skip, sample_rate, start_offset, data_size, version == 2 ? OPUS_NXAv2 : OPUS_SWITCH, loopstart, loopend);
	/* open the file for reading */
//	if (!vgmstream_open_stream(vgmstream, streamFile, start_offset))
//		goto fail;
//	return vgmstream;
fail:
//	close_vgmstream(vgmstream);
	return NULL;
}
