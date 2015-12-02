/*
 *			GPAC - Multimedia Framework C SDK
 *
 *			Authors: Jean Le Feuvre
 *			Copyright (c) Telecom ParisTech 2000-2012
 *					All rights reserved
 *
 *  This file is part of GPAC / Authoring Tools sub-project
 *
 *  GPAC is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  GPAC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _GF_MEDIA_H_
#define _GF_MEDIA_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <gpac/isomedia.h>
#include <gpac/avparse.h>
#include <gpac/config_file.h>

/*computes file hash. If file is ISO-based, computre hash according to OMA (P)DCF (without MutableDRMInformation box)*/
GF_Err gf_media_get_file_hash(const char *file, u8 hash[20]);

#ifndef GPAC_DISABLE_ISOM
/*creates (if needed) a GF_ESD for the given track - THIS IS RESERVED for local playback
only, since the OTI used when emulated is not standard...*/
GF_ESD *gf_media_map_esd(GF_ISOFile *mp4, u32 track);

/*!
 * \brief Get RFC 6381 description for @track from @movie.
 * \parama szCodec a pointer to an already allocated string.
 */
GF_Err gf_media_get_rfc_6381_codec_name(GF_ISOFile *movie, u32 track, char *szCodec, Bool force_inband_xps, Bool force_sbr);
#endif

#ifndef GPAC_DISABLE_ISOM_WRITE
/*changes pixel aspect ratio for visual tracks if supported. Negative values remove any PAR info*/
GF_Err gf_media_change_par(GF_ISOFile *file, u32 track, s32 ar_num, s32 ar_den);
GF_Err gf_media_remove_non_rap(GF_ISOFile *file, u32 track);
#endif


#define GF_IMPORT_DEFAULT_FPS	25.0

#ifndef GPAC_DISABLE_MEDIA_IMPORT

/*
			track importers

	All these can import a file into a dedicated track. If esd is NULL the track is blindly added
	otherwise it is added with the requested ESID if non-0, otherwise the new trackID is stored in ESID
	if use_data_ref is set, data is only referenced in the file
	if duration is not 0, only the first duration seconds are imported
	NOTE: if an ESD is specified, its decoderSpecificInfo is also updated
*/
/*track importer flags*/
enum
{
	/*references data rather than copy, whenever possible*/
	GF_IMPORT_USE_DATAREF = 1,
	/*for AVI video: imports at constant FPS (eg imports N-Vops due to encoder drops)*/
	GF_IMPORT_NO_FRAME_DROP = 1<<1,
	/*for CMP ASP only: forces treating the input as packed bitsream and discards all n-vops*/
	GF_IMPORT_FORCE_PACKED = 1<<2,
	/*for AAC audio: forces SBR mode with implicit signaling (backward compatible)*/
	GF_IMPORT_SBR_IMPLICIT = 1<<3,
	/*for AAC audio: forces SBR mode with explicit signaling (non-backward compatible).
	Will override GF_IMPORT_SBR_IMPLICIT flag when set*/
	GF_IMPORT_SBR_EXPLICIT = 1<<4,
	/*forces MPEG-4 import - some 3GP2 streams have both native IsoMedia sample description and MPEG-4 one possible*/
	GF_IMPORT_FORCE_MPEG4 = 1<<5,
	/*special flag for text import at run time (never set on probe), indicates to leave the text box empty
	so that we dynamically adapt to display size*/
	GF_IMPORT_SKIP_TXT_BOX = 1<<6,
	/*indicates to keep unknown tracks from .MOV/.IsoMedia files*/
	GF_IMPORT_KEEP_ALL_TRACKS = 1<<7,
	/*uses compact size in .MOV/.IsoMedia files*/
	GF_IMPORT_USE_COMPACT_SIZE = 1<<8,
	/*don't add a final empty sample when importing text tracks from srt*/
	GF_IMPORT_NO_TEXT_FLUSH = 1<<9,
	/*for SVC or SHVC video: forces explicit SVC / SHVC signaling */
	GF_IMPORT_SVC_EXPLICIT = 1<<10,
	/*for SVC / SHVC video: removes all SVC / SHVC extensions*/
	GF_IMPORT_SVC_NONE = 1<<11,

	/*for AAC audio: forces PS mode with implicit signaling (backward compatible)*/
	GF_IMPORT_PS_IMPLICIT = 1<<12,
	/*for AAC audio: forces PS mode with explicit signaling (non-backward compatible).
	Will override GF_IMPORT_PS_IMPLICIT flag when set*/
	GF_IMPORT_PS_EXPLICIT = 1<<13,

	/* oversampled SBR */
	GF_IMPORT_OVSBR = 1<<14,

	/* set subsample information with SVC*/
	GF_IMPORT_SET_SUBSAMPLES = 1<<15,

	/* force to mark non-IDR frmaes with sync data (I slices,) to be marked as sync points points
	THE RESULTING FILE IS NOT COMPLIANT*/
	GF_IMPORT_FORCE_SYNC = 1<<16,

	/*keep trailing 0 bytes in AU payloads when any*/
	GF_IMPORT_KEEP_TRAILING = 1<<17,

	/*forces inband parameter sets*/
	GF_IMPORT_FORCE_XPS_INBAND = 1<<18,

	/*do not compute edit list for B-frames video tracks*/
	GF_IMPORT_NO_EDIT_LIST = 1<<19,

	/*when set, only updates tracks info and return*/
	GF_IMPORT_PROBE_ONLY	= 1<<20,
	/*only set when probing, signals several frames per sample possible*/
	GF_IMPORT_3GPP_AGGREGATION = 1<<21,
	/*only set when probing, signals video FPS overridable*/
	GF_IMPORT_OVERRIDE_FPS = 1<<22,
	/*only set when probing, signals duration not usable*/
	GF_IMPORT_NO_DURATION = 1<<23,
	/*when set IP packets found in MPE sections will be sent to the local network */
	GF_IMPORT_MPE_DEMUX = 1<<24,


	/*when set by user during import, will abort*/
	GF_IMPORT_DO_ABORT = 1<<31
};



#define GF_IMPORT_AUTO_FPS		10000.0

#define GF_IMPORT_MAX_TRACKS	100
struct __track_video_info
{
	u32 width, height, par;
	Double FPS;
};
struct __track_audio_info
{
	u32 sample_rate, nb_channels;
};

struct __track_import_info
{
	u32 track_num;
	/*track type (GF_ISOM_MEDIA_****)*/
	u32 type;
	/*media type ('MPG1', 'MPG2', ISO 4CC, AVI 4CC)*/
	u32 media_type;
	/*possible import flags*/
	u32 flags;
	/*media format info*/
	struct __track_video_info video_info;
	struct __track_audio_info audio_info;

	char szCodecProfile[20];

	u32 lang;
	/*for MPEG4 on MPEG-2 TS: mpeg4 ES-ID*/
	u32 mpeg4_es_id;
	/*for MPEG2 TS: program number*/
	u16 prog_num;
};

struct __program_import_info
{
	u32 number;
	char name[40];
};

/*track dumper*/
typedef struct __track_import
{
	GF_ISOFile *dest;
	/*media to import:
		MP4/ISO media: trackID
		AVI files:
			0: first video and first audio,
			1: video track
			2->any: audio track(s)
		MPEG-PS files with N video streams:
			0: first video and first audio
			1->N: video track
			N+1->any: audio track
	TrackNums can be obtain with probing
	*/
	u32 trackID;
	/*media source - selects importer type based on extension*/
	char *in_name;
	/*import duration if any*/
	u32 duration;
	/*importer flags*/
	u32 flags;
	/*importer swf flags*/
	u32 swf_flags;
	Float swf_flatten_angle;
	/*if non 0, force video FPS (CMP, AVI, OGG, H264) - also used by SUB import*/
	Double video_fps;
	/*optional ESD*/
	GF_ESD *esd;
	/*optional format indication for media source (used in IM1)*/
	char *streamFormat;
	/*frame per sample cumulation (3GP media only) - MAX 15, ignored in data ref*/
	u32 frames_per_sample;
	/*track ID of imported media in destination file*/
	u32 final_trackID;
	/*optional format indication for media source (used in IM1)*/
	char *force_ext;

	/*for MP4 import only*/
	GF_ISOFile *orig;

	/*for text import*/
	u32 fontSize;
	char *fontName;
	u32 text_track_width, text_track_height, text_width, text_height;
	s32 text_x, text_y;

	/*Initial offset of the first AU to import*/
	Double initial_time_offset;

	/*number of tracks after probing - may be set to 0, in which case no track
	selection can be performed. It may also be inaccurate if probing doesn't
	detect all available tracks (cf ogg import)*/
	u32 nb_tracks;
	/*track info after probing (GF_IMPORT_PROBE_ONLY set).*/
	struct __track_import_info tk_info[GF_IMPORT_MAX_TRACKS];
	u64 probe_duration;

	/*for MPEG-TS and similar: program names*/
	u32 nb_progs;
	struct __program_import_info pg_info[GF_IMPORT_MAX_TRACKS];

	GF_Err last_error;
} GF_MediaImporter;

GF_Err gf_media_import(GF_MediaImporter *importer);


/*adds chapter info contained in file - import_fps is optional (most formats don't use it), defaults to 25*/
GF_Err gf_media_import_chapters(GF_ISOFile *file, char *chap_file, Double import_fps);

/*make the file ISMA compliant: creates ISMA BIFS / OD tracks if needed, and update audio/video IDs
the file should not contain more than one audio and one video track
@keepImage: if set, generates system info if image is found - only used for image imports
*/
GF_Err gf_media_make_isma(GF_ISOFile *mp4file, Bool keepESIDs, Bool keepImage, Bool no_ocr);

/*make the file 3GP compliant && sets profile
*/
GF_Err gf_media_make_3gpp(GF_ISOFile *mp4file);

/*make the file playable on a PSP
*/
GF_Err gf_media_make_psp(GF_ISOFile *mp4file);

/*changes the profile (if not 0) and level (if not 0) indication of the media - only AVC/H264 supported for now*/
GF_Err gf_media_change_pl(GF_ISOFile *file, u32 track, u32 profile, u32 level);

/*rewrite AVC samples if nalu size_length has to be changed*/
GF_Err gf_media_avc_rewrite_samples(GF_ISOFile *file, u32 track, u32 prev_size_in_bits, u32 new_size_in_bits);

/* Split SVC layers */
GF_Err gf_media_split_svc(GF_ISOFile *file, u32 track, Bool splitAll);

/* Merge SVC layers*/
GF_Err gf_media_merge_svc(GF_ISOFile *file, u32 track, Bool mergeAll);

/* Split SHVC layers */
GF_Err gf_media_split_shvc(GF_ISOFile *file, u32 track, Bool splitAll, Bool use_extractors);

GF_Err gf_media_split_hevc_tiles(GF_ISOFile *file, Bool use_extractors);

#endif /*GPAC_DISABLE_MEDIA_IMPORT*/


	
typedef struct
{
	char *file_name;
	char *representationID;
	char *periodID;
	Double media_duration;
	u32 nb_baseURL;
	char **baseURL;
	char *xlink;
	char *role;
	u32 nb_rep_descs;
	char **rep_descs;
	u32 nb_p_descs;
	char **p_descs;
	u32 nb_as_descs;
	char **as_descs;
	u32 nb_as_c_descs;
	char **as_c_descs;
	u32 bandwidth;
	Double period_duration;
} GF_DashSegmenterInput;

typedef enum
{
	// DASH profiles (ISO/IEC 23009-1).
	GF_DASH_PROFILE_FULL = 0,
	GF_DASH_PROFILE_LIVE, /*live for ISOFF, SIMPLE for M2TS*/
	GF_DASH_PROFILE_ONDEMAND,
	GF_DASH_PROFILE_MAIN,

	// DASH-IF AVC/264 2.0 IOP profiles.
	GF_DASH_PROFILE_DASHIF_AVC264_2_0_LIVE,     //now called GF_DASH_PROFILE_DASHIF264_HD
	GF_DASH_PROFILE_DASHIF_AVC264_2_0_ONDEMAND, //now called GF_DASH_PROFILE_DASHIF264_HD

	// DASH-IF IOP 3.0.
	GF_DASH_PROFILE_DASHIF264_MAIN,
	GF_DASH_PROFILE_DASHIF264_HIGH,
	GF_DASH_PROFILE_DASHIF_SIMPLE,
	GF_DASH_PROFILE_DASHIF_MAIN,

	// HbbTV.
	GF_DASH_PROFILE_HBBTV_1_5_ISOBMF_LIVE,

	/*internal use only*/
	GF_DASH_PROFILE_UNKNOWN
} GF_DashProfile;


typedef enum
{
	GF_DASH_BSMODE_DEFAULT, //=inband for live profile and none for onDemand
	GF_DASH_BSMODE_NONE,
	GF_DASH_BSMODE_INBAND,
	GF_DASH_BSMODE_MERGED,
	GF_DASH_BSMODE_MULTIPLE_ENTRIES,
	GF_DASH_BSMODE_SINGLE
} GF_DashSwitchingMode;


typedef enum
{
	GF_DASH_STATIC = 0,
	GF_DASH_DYNAMIC,
	//can only be used when DASH segmenter context is used, will close the period
	GF_DASH_DYNAMIC_LAST,
	GF_DASH_DYNAMIC_DEBUG,
} GF_DashDynamicMode;

typedef enum
{
	GF_DASH_CPMODE_ADAPTATION_SET=0,
	GF_DASH_CPMODE_REPRESENTATION,
	GF_DASH_CPMODE_BOTH,
} GF_DASH_ContentLocationMode;
	
typedef struct __gf_dash_segmenter GF_DASHSegmenter;

/*Create a new DASH segmenter
 *	\param mpdName target MPD file name, cannot be changed
 *	\param profile target DASH profile, cannot be changed
 *	\param tmp_dir temp dir for file generation, OS NULL for default
 *	\param timescale timescale used to specif most of the dash timings. If 0, 1000 is used
 *	\param dasher_context_file config file used to store the context of the DASH segmenter. This allows destroying the segmenter and restarting it later on with the right DASH segquence numbers, MPD and and timing info
 *	\return the DASH segmenter object
*/
GF_DASHSegmenter *gf_dasher_new(const char *mpdName, GF_DashProfile profile, const char *tmp_dir, u32 timescale, GF_Config *dasher_context_file);
/*Deletes a DASH segmenter
 *	\param dasher the DASH segmenter object
*/
void gf_dasher_del(GF_DASHSegmenter *dasher);
/*Removes the DASH inputs. Re-add new ones with gf_dasher_add_input()
 *	\param dasher the DASH segmenter object
*/
void gf_dasher_clean_inputs(GF_DASHSegmenter *dasher);
/*Sets MPD info
 *	\param dasher the DASH segmenter object
 *	\param title MPD title
 *	\param copyright MPD copyright
 *	\param moreInfoURL MPD "more info" URL
 *	\param sourceInfo MPD source info
 *	\return error code if any
*/
GF_Err gf_dasher_set_info(GF_DASHSegmenter *dasher, const char *title, const char *copyright, const char *moreInfoURL, const char *sourceInfo);
/*Sets MPD Location. This is useful to distrubute a dynamic MPD by mail or any non-HTTP mean
 *	\param dasher the DASH segmenter object
 *	\param location the URL where this MPD can be found
 *	\return error code if any
*/
GF_Err gf_dasher_set_location(GF_DASHSegmenter *dasher, const char *location);
/*Adds a base URL to the MPD 
 *	\param dasher the DASH segmenter object
 *	\param base_url base url to add
 *	\return error code if any
*/
GF_Err gf_dasher_add_base_url(GF_DASHSegmenter *dasher, const char *base_url);
/*Enable URL template -  - may be overriden by the current profile
 *	\param dasher the DASH segmenter object
 *	\param enable enable usage of URL template
 *	\param default_template template for the segment name
 *	\param default_extension extension for the segment name
 *	\return error code if any
*/
GF_Err gf_dasher_enable_url_template(GF_DASHSegmenter *dasher, Bool enable, const char *default_template, const char *default_extension);
/*Enable Segment Timeline template - may be overriden by the current profile
 *	\param dasher the DASH segmenter object
 *	\param enable enable or disable
 *	\return error code if any
*/
GF_Err gf_dasher_enable_segment_timeline(GF_DASHSegmenter *dasher, Bool enable);
/*Enables single segment - may be overriden by the current profile
 *	\param dasher the DASH segmenter object
 *	\param enable enable or disable
 *	\return error code if any
*/
GF_Err gf_dasher_enable_single_segment(GF_DASHSegmenter *dasher, Bool enable);
/*Enable single file (with multiple segments) - may be overriden by the current profile
 *	\param dasher the DASH segmenter object
 *	\param enable enable or disable
 *	\return error code if any
*/
GF_Err gf_dasher_enable_single_file(GF_DASHSegmenter *dasher, Bool enable);
/*Sets bitstream switching mode - may be overriden by the current profile
 *	\param dasher the DASH segmenter object
 *	\param bitstream_switching mode to use for bitstream switching
 *	\return error code if any
*/
GF_Err gf_dasher_set_switch_mode(GF_DASHSegmenter *dasher, GF_DashSwitchingMode bitstream_switching);
/*Sets segment and fragment durations. 
 *	\param dasher the DASH segmenter object
 *	\param default_segment_duration the duration of a dash segment
 *	\param segment_duration_strict indicated is the duration is strict (otherwise we consider the mean duration)
 *	\param default_fragment_duration the duration of a dash fragment - if 0, same as default_segment_duration
 *	\return error code if any
*/
GF_Err gf_dasher_set_durations(GF_DASHSegmenter *dasher, Double default_segment_duration, Bool segment_duration_strict, Double default_fragment_duration);
/*Enables spliting at RAP boundaries
 *	\param dasher the DASH segmenter object
 *	\param segments_start_with_rap segments will be split at RAP boundaries
 *	\param fragments_start_with_rap fragments will be split at RAP boundaries
 *	\return error code if any
*/
GF_Err gf_dasher_enable_rap_splitting(GF_DASHSegmenter *dasher, Bool segments_start_with_rap, Bool fragments_start_with_rap);
/*Enables segment marker
 *	\param dasher the DASH segmenter object
 *	\param dasher the DASH segmenter object
 *	\return error code if any
*/
GF_Err gf_dasher_set_segment_marker(GF_DASHSegmenter *dasher, u32 segment_marker_4cc);
/*Enables segment indexes 
 *	\param dasher the DASH segmenter object
 *	\param enable_sidx enable or disable
 *	\param subsegs_per_sidx number of subsegments per segment
 *	\param daisy_chain_sidx enable daisy chaining of sidx
 *	\return error code if any
*/
GF_Err gf_dasher_enable_sidx(GF_DASHSegmenter *dasher, Bool enable_sidx, u32 subsegs_per_sidx, Bool daisy_chain_sidx);
/*Sets mode for the dash segmenter.
 *	\param dasher the DASH segmenter object
 *	\param dash_mode the mode to use. Currently switching from static mode to dynamic mode is not well supported and may produce non-compliant MPDs
 *	\param mpd_update_time time between MPD refresh, in seconds. Used for dynamic mode, may be 0 if @mpd_live_duration is set
 *	\param time_shift_depth the depth of the time shift buffer in seconds, -1 for infinite time shift.
 *	\param mpd_live_duration total duration of the DASH session in dynamic mode, in seconds. May be set to 0 if @mpd_update_time is set
 *	\return error code if any
*/
GF_Err gf_dasher_set_dynamic_mode(GF_DASHSegmenter *dasher, GF_DashDynamicMode dash_mode, Double mpd_update_time, s32 time_shift_depth, Double mpd_live_duration);
/*Sets the minimal buffer desired.
 *	\param dasher the DASH segmenter object
 *	\param min_buffer min buffer time in seconds for the DASH session. Currently the minimal buffer is NOT computed from the source material and must be set to an appropriate value.
 *	\return error code if any
*/
GF_Err gf_dasher_set_min_buffer(GF_DASHSegmenter *dasher, Double min_buffer);
/*Sets the availability start time offset.
 *	\param dasher the DASH segmenter object
 *	\param ast_offset ast offset in milliseconds. If >0, the DASH session availabilityStartTime will be earlier than UTC by the amount of seconds specified. If <0, the media representation will have an availabilityTimeOffset of the amount of seconds specified, instructing the client that segments may be accessed earlier.
 *	\return error code if any
*/
GF_Err gf_dasher_set_ast_offset(GF_DASHSegmenter *dasher, s32 ast_offset);
/*Enables memory fragmenting: fragments will be written to disk only once completed
 *	\param dasher the DASH segmenter object
 *	\param enable Enables or disables. Default is disabled.
 *	\return error code if any
*/
GF_Err gf_dasher_enable_memory_fragmenting(GF_DASHSegmenter *dasher, Bool enable);
/*Sets initial values for ISOBMFF sequence number and TFDT in movie fragments.
 *	\param dasher the DASH segmenter object
 *	\param initial_moof_sn sequence number of the first moof to be generated. Default value is 1.
 *	\param initial_tfdt initial tfdt of the first traf to be generated, in DASH segmenter timescale units. Default value is 0.
 *	\return error code if any
*/
GF_Err gf_dasher_set_initial_isobmf(GF_DASHSegmenter *dasher, u32 initial_moof_sn, u64 initial_tfdt);
/*Configure how default values for ISOBMFF are stored
 *	\param dasher the DASH segmenter object
 *	\param no_fragments_defaults if set, fragments default values are repeated in each traf and not set in trex. Default value is GF_FALSE
 *	\param pssh_moof if set, PSSH is stored in each moof, and not set in init segment. Default value is GF_FALSE
 *	\param samplegroups_in_traf if set, all sample group definitions are stored in each traf and not set in init segment. Default value is GF_FALSE
 *	\param single_traf_per_moof if set, each moof will contain a single traf, even if source media is multiplexed. Default value is GF_FALSE
 *	\return error code if any
*/
GF_Err gf_dasher_configure_isobmf_default(GF_DASHSegmenter *dasher, Bool no_fragments_defaults, Bool pssh_moof, Bool samplegroups_in_traf, Bool single_traf_per_moof);
/*Enables insertion of UTC reference in the begining of segments
 *	\param dasher the DASH segmenter object
 *	\param insert_utc if set, UTC will be inserted. Default value is disabled.
 *	\return error code if any
*/
GF_Err gf_dasher_enable_utc_ref(GF_DASHSegmenter *dasher, Bool insert_utc);
/*Enables real-time generation of media segments.
 *	\param dasher the DASH segmenter object
 *	\param dasher real_time if set, segemnts are generated in real time. Only supported for single representation (potentially multiplexed) DASH session. Default is disabled.
 *	\return error code if any
*/
GF_Err gf_dasher_enable_real_time(GF_DASHSegmenter *dasher, Bool real_time);
/*Sets where the  ContentProtection element is inserted in an adaptation set.
*	\param dasher the DASH segmenter object
*	\param  ContentProtection element location mode.
*	\return error code if any
*/
GF_Err gf_dasher_set_content_protection_location_mode(GF_DASHSegmenter *dasher, GF_DASH_ContentLocationMode mode);
/*Sets profile extension as used by DASH-IF and DVB.
 *	\param dasher the DASH segmenter object
 *	\param dash_profile_extension specifies a string of profile extensions, as used by DASH-IF and DVB.
 *	\return error code if any
*/
GF_Err gf_dasher_set_profile_extension(GF_DASHSegmenter *dasher, const char *dash_profile_extension);
/*Adds a media input to the DASHer
 *	\param dasher the DASH segmenter object
 *	\param input media source to add
 *	\return error code if any
*/
GF_Err gf_dasher_add_input(GF_DASHSegmenter *dasher, GF_DashSegmenterInput *input);
/*Process the media source and generate segments
 *	\param dasher the DASH segmenter object
 *	\param sub_duration the duration in seconds of media to DASH. If 0, the whole sources will be processed.
 *	\return error code if any
*/
GF_Err gf_dasher_process(GF_DASHSegmenter *dasher, Double sub_duration);

/*returns time to wait until end of currently generated segments
 *	\param dasher the DASH segmenter object
 *	\return time to wait in milliseconds
*/
u32 gf_dasher_next_update_time(GF_DASHSegmenter *dasher);


    
    
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
/*save file as fragmented movie*/
GF_Err gf_media_fragment_file(GF_ISOFile *input, const char *output_file, Double max_duration_sec);
#endif

#ifndef GPAC_DISABLE_MEDIA_EXPORT

enum
{
	/*track dumper types are formatted as flags for conveniency for
	authoring tools, but never used as a OR'ed set*/
	/*native format (JPG, PNG, MP3, etc) if supported*/
	GF_EXPORT_NATIVE = GF_TRUE,
	/*raw samples (including hint tracks for rtp)*/
	GF_EXPORT_RAW_SAMPLES = (1<<1),
	/*NHNT format (any MPEG-4 media)*/
	GF_EXPORT_NHNT = (1<<2),
	/*AVI (MPEG4 video and AVC tracks only)*/
	GF_EXPORT_AVI = (1<<3),
	/*MP4 (all except OD)*/
	GF_EXPORT_MP4 = (1<<4),
	/*AVI->RAW to dump video (trackID=1) or audio (trackID>=2)*/
	GF_EXPORT_AVI_NATIVE = (1<<5),
	/*NHML format (any media)*/
	GF_EXPORT_NHML = (1<<6),
	/*SAF format*/
	GF_EXPORT_SAF = (1<<7),
	/*WebVTT metadata format (any media)*/
	GF_EXPORT_WEBVTT_META = (1<<8),
	/*WebVTT metadata format: media data will be embedded in webvtt*/
	GF_EXPORT_WEBVTT_META_EMBEDDED = (1<<9),

	/*following ones are real flags*/
	/*
	for MP4 extraction, indicates track should be added to dest file if any
	for raw extraction, indicates data shall be appended at the end of output file if present
	*/
	GF_EXPORT_MERGE = (1<<10),
	/*indicates QCP file format possible as well as native (EVRC and SMV audio only)*/
	GF_EXPORT_USE_QCP = (1<<11),
	/*indicates full NHML dump*/
	GF_EXPORT_NHML_FULL = (1<<11),
	/**/
	GF_EXPORT_SVC_LAYER = (1<<12),
	/* Don't merge identical cues in consecutive samples */
	GF_EXPORT_WEBVTT_NOMERGE = (1<<13),

	/* Experimental Streaming Instructions */
	GF_EXPORT_SIX = (1<<14),

	/* Experimental Streaming Instructions */
	GF_EXPORT_FORCE_EXT = (1<<15),

	/*ony probes extraction format*/
	GF_EXPORT_PROBE_ONLY = (1<<30),
	/*when set by user during export, will abort*/
	GF_EXPORT_DO_ABORT = (1<<31),
};

/*track dumper*/
typedef struct __track_exporter
{
	GF_ISOFile *file;
	u32 trackID;
	/*sample number to export for GF_EXPORT_RAW_SAMPLES only*/
	u32 sample_num;
	/*out name WITHOUT extension*/
	char *out_name;
	/*dump type*/
	u32 flags;
	/*non-IsoMedia file (AVI)*/
	char *in_name;
} GF_MediaExporter;

/*if error returns same value as error signaled in message*/
GF_Err gf_media_export(GF_MediaExporter *dump);

#endif /*GPAC_DISABLE_MEDIA_EXPORT*/


#ifndef GPAC_DISABLE_ISOM_HINTING
/*
	RTP IsoMedia file hinting
*/
typedef struct __tag_isom_hinter GF_RTPHinter;

GF_RTPHinter *gf_hinter_track_new(GF_ISOFile *file, u32 TrackNum,
                                  u32 Path_MTU, u32 max_ptime, u32 default_rtp_rate, u32 hint_flags, u8 PayloadID,
                                  Bool copy_media, u32 InterleaveGroupID, u8 InterleaveGroupPriority, GF_Err *e);
/*delete the track hinter*/
void gf_hinter_track_del(GF_RTPHinter *tkHinter);
/*hints all samples in the media track*/
GF_Err gf_hinter_track_process(GF_RTPHinter *tkHint);
/*returns media bandwidth in kbps*/
u32 gf_hinter_track_get_bandwidth(GF_RTPHinter *tkHinter);
/*retrieves hinter flags*/
u32 gf_hinter_track_get_flags(GF_RTPHinter *tkHinter);
/*retrieves rtp payload name
	@payloadName: static buffer for retrieval, minimum 30 bytes
*/
void gf_hinter_track_get_payload_name(GF_RTPHinter *tkHint, char *payloadName);

/*finalizes hinting process for the track (setup flags, write SDP for RTP, ...)
	@AddSystemInfo: if non-0, systems info are duplicated in the SDP (decoder cfg, PL IDs ..)
*/
GF_Err gf_hinter_track_finalize(GF_RTPHinter *tkHint, Bool AddSystemInfo);

/*SDP IOD flag*/
enum
{
	/*no IOD included*/
	GF_SDP_IOD_NONE = 0,
	/*base64 encoding of the regular MPEG-4 IOD*/
	GF_SDP_IOD_REGULAR,
	/*base64 encoding of IOD containing BIFS and OD tracks (one AU only) - this is used for ISMA 1.0 profiles
	note that the "hinted" file will loose all systems info*/
	GF_SDP_IOD_ISMA,
	/*same as ISMA but removes all clock references from IOD*/
	GF_SDP_IOD_ISMA_STRICT,
};

/*finalizes hinting process for the file (setup flags, write SDP for RTP, ...)
	@IOD_Profile: see above
	@bandwidth: total bandwidth in kbps of all hinted tracks, 0 means no bandwidth info at session level
*/
GF_Err gf_hinter_finalize(GF_ISOFile *file, u32 IOD_Profile, u32 bandwidth);


/*returns TRUE if the encoded data fits in an ESD url - streamType is the systems stream type needed to
signal data mime-type (OD, BIFS or any) */
Bool gf_hinter_can_embbed_data(char *data, u32 data_size, u32 streamType);


#endif /*GPAC_DISABLE_ISOM_HINTING*/


/*SAF Multiplexer object. The multiplexer supports concurencial (multi-threaded) access*/
typedef struct __saf_muxer GF_SAFMuxer;
/*SAF Multiplexer constructor*/
GF_SAFMuxer *gf_saf_mux_new();
/*SAF Multiplexer destructor*/
void gf_saf_mux_del(GF_SAFMuxer *mux);
/*adds a new stream in the SAF multiplex*/
GF_Err gf_saf_mux_stream_add(GF_SAFMuxer *mux, u32 stream_id, u32 ts_res, u32 buffersize_db, u8 stream_type, u8 object_type, char *mime_type, char *dsi, u32 dsi_len, char *remote_url);
/*removes a stream from the SAF multiplex*/
GF_Err gf_saf_mux_stream_rem(GF_SAFMuxer *mux, u32 stream_id);
/*adds an AU to the given stream. !!AU data will be freed by the multiplexer!!
AUs are not reinterleaved based on their CTS, in order to enable audio interleaving
*/
GF_Err gf_saf_mux_add_au(GF_SAFMuxer *mux, u32 stream_id, u32 CTS, char *data, u32 data_len, Bool is_rap);
/*gets the content of the multiplexer for the given time.
if force_end_of_session is set, this flushes the SAF Session - no more operations will be allowed on the muxer*/
GF_Err gf_saf_mux_for_time(GF_SAFMuxer *mux, u32 time_ms, Bool force_end_of_session, char **out_data, u32 *out_size);

/*reduces input width/height to common aspect ration num/denum values*/
void gf_media_reduce_aspect_ratio(u32 *width, u32 *height);
/*reduces input FPS to a more compact value (eg 25000/1000 -> 25/1)*/
void gf_media_get_reduced_frame_rate(u32 *timescale, u32 *sample_dur);

#ifdef __cplusplus
}
#endif


#endif	/*_GF_MEDIA_H_*/

