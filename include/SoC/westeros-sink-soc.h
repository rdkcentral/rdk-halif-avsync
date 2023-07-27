/*
 * Copyright (C) 2016 RDK Management
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef __WESTEROS_SINK_SOC_H__
#define __WESTEROS_SINK_SOC_H__

#include <stdlib.h>
#include <stdint.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <linux/videodev2.h>
#include <drm/drm_fourcc.h>

#include "simplebuffer-client-protocol.h"

#ifndef NO_GENERIC_AVSYNC
#define USE_GENERIC_AVSYNC
#endif

#ifdef USE_AMLOGIC_MESON
#undef USE_GENERIC_AVSYNC
#endif

#define WESTEROS_SINK_CAPS \
      "video/x-h264, " \
      "parsed=(boolean) true, " \
      "alignment=(string) au, " \
      "stream-format=(string) byte-stream, " \
      "width=(int) [1,MAX], " "height=(int) [1,MAX] ; " \
      "video/mpeg, " \
      "parsed=(boolean) true, " \
      "systemstream = (boolean) false, " \
      "width=(int) [1,MAX], " "height=(int) [1,MAX]" 

typedef struct _WstVideoClientConnection
{
   GstWesterosSink *sink;
   const char *name;
   struct sockaddr_un addr;
   int socketFd;
   int serverRefreshRate;
   gint64 serverRefreshPeriod;
} WstVideoClientConnection;

typedef struct _WstPlaneInfo
{
   int fd;
   void *start;
   int capacity;
} WstPlaneInfo;

#define WST_MAX_PLANES (3)
typedef struct _WstGemBuffer
{
   uint32_t width;
   uint32_t height;
   int planeCount;
   unsigned int handle[WST_MAX_PLANES];
   unsigned int stride[WST_MAX_PLANES];
   unsigned int offset[WST_MAX_PLANES];
   uint32_t size[WST_MAX_PLANES];
   int fd[WST_MAX_PLANES];
} WstGemBuffer;

typedef struct _WstBufferInfo
{
   struct v4l2_buffer buf;
   struct v4l2_plane planes[WST_MAX_PLANES];
   WstPlaneInfo planeInfo[WST_MAX_PLANES];
   WstGemBuffer gemBuf;
   GstBuffer *gstbuf;
   int bufferId;
   bool locked;
   int lockCount;
   int planeCount;
   int fd;
   void *start;
   int capacity;
   int frameNumber;
   gint64 frameTime;
   bool drop;
   bool queued;
} WstBufferInfo;

#ifdef ENABLE_SW_DECODE
#define WST_NUM_SW_BUFFERS (4)
typedef struct _WstSWBuffer
{
   int width;
   int height;
   int fd0;
   int fd1;
   int handle0;
   int handle1;
   int size0;
   int size1;
   int offset0;
   int offset1;
   int pitch0;
   int pitch1;
} WstSWBuffer;
#endif

#ifdef USE_GENERIC_AVSYNC
typedef struct _AVSyncCtrl
{
   pthread_mutex_t mutex;
   long long sysTime;
   long long avTime;
} AVSyncCtrl;

typedef struct _AVSyncCtx
{
   int fd;
   char name[PATH_MAX];
   int ctrlSize;
   AVSyncCtrl *ctrl;
   GstElement *audioSink;
} AVSyncCtx;
#endif

typedef struct _WstPARInfo
{
   int frameNumber;
   double par;
   int frameWidth;
   int frameHeight;
} WstPARInfo;

#ifdef USE_GST_AFD
typedef struct _WstAFDInfo
{
   gint64 pts;
   int frameNumber;
   int spec;
   int field;
   int afd;
   bool haveBar;
   bool isLetterbox;
   int f;
   int d1;
   int d2;
} WstAFDInfo;
#endif

struct _GstWesterosSinkSoc
{
   struct wl_sb *sb;
   int activeBuffers;
   double frameRate;
   int frameRateFractionNum;
   int frameRateFractionDenom;
   gboolean frameRateChanged;
   double pixelAspectRatio;
   WstPARInfo *parNext;
   int parNextCount;
   int parNextCapacity;
   gboolean havePixelAspectRatio;
   gboolean pixelAspectRatioChanged;
   #ifdef USE_GST_AFD
   WstAFDInfo afdActive;
   WstAFDInfo *afdInfo;
   int afdInfoCount;
   int afdInfoCapacity;
   #endif
   gboolean showChanged;
   gboolean zoomModeGlobal;
   gboolean useImmediateOutput;
   int zoomMode;
   int zoomModeUser;
   int overscanSize;
   int frameWidth;
   int frameHeight;
   int frameWidthStream;
   int frameHeightStream;
   int frameInCount;
   int frameOutCount;
   int frameDecodeCount;
   int frameDisplayCount;
   gboolean expectNoLastFrame;
   int decoderLastFrame;
   int decoderEOS;
   uint32_t numDropped;
   uint32_t inputFormat;
   uint32_t outputFormat;
   gboolean interlaced;
   gint64 prevDecodedTimestamp;
   gint64 currentInputPTS;
   gint64 videoStartTime;

   char *devname;
   gboolean enableTextureSignal;
   gboolean enableDecodeErrorSignal;
   int v4l2Fd;
   struct v4l2_capability caps;
   uint32_t deviceCaps;
   gboolean isMultiPlane;
   gboolean preferNV12M;
   uint32_t inputMemMode;
   uint32_t outputMemMode;
   int numInputFormats;
   struct v4l2_fmtdesc *inputFormats;
   int numOutputFormats;
   struct v4l2_fmtdesc *outputFormats;
   struct v4l2_format fmtIn;
   struct v4l2_format fmtOut;
   gboolean formatsSet;
   gboolean updateSession;
   int syncType;
   int sessionId;
   #ifdef USE_AMLOGIC_MESON_MSYNC
   gboolean userSession;
   gboolean userAVSyncMode;
   #endif
   int bufferCohort;
   uint32_t minBuffersIn;
   uint32_t minBuffersOut;
   int inQueuedCount;
   int numBuffersIn;
   WstBufferInfo *inBuffers;
   int outQueuedCount;
   int numBuffersOut;
   int bufferIdOutBase;
   WstBufferInfo *outBuffers;

   int nextFrameFd;
   int prevFrame1Fd;
   int prevFrame2Fd;
   int resubFd;

   gboolean videoPlaying;
   gboolean videoPaused;
   gboolean hasEvents;
   gboolean hasEOSEvents;
   gboolean needCaptureRestart;
   gboolean emitFirstFrameSignal;
   gboolean emitUnderflowSignal;
   gboolean decodeError;
   gboolean quitVideoOutputThread;
   GThread *videoOutputThread;
   gboolean quitEOSDetectionThread;
   GThread *eosDetectionThread;
   gboolean quitDispatchThread;
   GThread *dispatchThread;

   gboolean useCaptureOnly;
   gboolean captureEnabled;
   gboolean frameAdvance;
   gboolean pauseException;
   gboolean pauseGetGfxFrame;
   gboolean useGfxSync;
   gboolean allow4kZoom;
   int pauseGfxBuffIndex;
   int hideVideoFramesDelay;
   int hideGfxFramesDelay;
   int framesBeforeHideVideo;
   int framesBeforeHideGfx;
   gint64 prevFrameTimeGfx;
   gint64 prevFramePTSGfx;
   WstVideoClientConnection *conn;
   int videoX;
   int videoY;
   int videoWidth;
   int videoHeight;

   guint8 *codecData;
   int codecDataLen;
   gboolean codecDataInjected;

   gboolean haveColorimetry;
   int hdrColorimetry[4];
   gboolean haveMasteringDisplay;
   float hdrMasteringDisplay[10];
   gboolean haveContentLightLevel;
   int hdrContentLightLevel[2];

   int dvBaseLayerPresent;
   int dvEnhancementLayerPresent;

   GstBuffer *prerollBuffer;
   gboolean frameStepOnPreroll;
   gboolean forceAspectRatio;

   gboolean lowMemoryMode;
   gboolean secureVideo;
   gboolean useDmabufOutput;
   int dwMode;
   int drmFd;

   #ifdef USE_GENERIC_AVSYNC
   AVSyncCtx *avsctx;
   #endif

   #ifdef USE_GST1
   GstPadChainFunction chainOrg;
   #endif

   #ifdef ENABLE_SW_DECODE
   GThread *firstFrameThread;
   int nextSWBuffer;
   WstSWBuffer swBuffer[WST_NUM_SW_BUFFERS];
   gboolean swPrerolled;
   #endif

   #ifdef GLIB_VERSION_2_32 
   GMutex mutex;
   #else
   GMutex *mutex;
   #endif
};


/**
 * It is a class initialization function that initializes the class structure and takes a pointer to the GstWesterosSinkClass structure as a parameter.
*/
void gst_westeros_sink_soc_class_init(GstWesterosSinkClass *klass);

/**
 * This function takes a pointer to the GstWesterosSink object as a parameter, which represents the instance of the GstWesterosSink that is being initialized. 
 * During initialization, the function sets up the hardware-specific state of the GstWesterosSink object, by setting up the underlying hardware resources needed for playback.
*/
gboolean gst_westeros_sink_soc_init( GstWesterosSink *sink );

/**
 * It is responsible for cleaning up the hardware-specific state of the GstWesterosSink object and freeing any resources that were allocated during the initialization process.
*/
void gst_westeros_sink_soc_term( GstWesterosSink *sink );

/**
 * It is a callback function that is called when a property of a GstWesterosSink object is being set. 
 * It takes four parameters: a pointer to the GObject, an unsigned integer that represents the ID of the property that is being set, a pointer to a Gvalue that represents the new value of the property and a pointer GParamSpec that represents the property specification.
*/
void gst_westeros_sink_soc_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec);

/**
 * It is a callback function that is called when a property of a GstWesterosSink object is being retrieved.
*/
void gst_westeros_sink_soc_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec);

/**
 * This function is called when the GstWesterosSink object transitions from the null state to the ready state.
*/
gboolean gst_westeros_sink_soc_null_to_ready( GstWesterosSink *sink, gboolean *passToDefault );

/**
 * This function is called when the GstWesterosSink object transitions from the ready state to the paused state.
*/
gboolean gst_westeros_sink_soc_ready_to_paused( GstWesterosSink *sink, gboolean *passToDefault );

/**
 * This function is called when the GstWesterosSink object is transitioning from the paused to the playing state.
*/
gboolean gst_westeros_sink_soc_paused_to_playing( GstWesterosSink *sink, gboolean *passToDefault );

/**
 * This function is called when the GstWesterosSink object is transitioning from the playing to the paused state.
*/
gboolean gst_westeros_sink_soc_playing_to_paused( GstWesterosSink *sink, gboolean *passToDefault );

/**
 * This function is called when the GstWesterosSink object is transitioning from the paused to the ready state.
*/
gboolean gst_westeros_sink_soc_paused_to_ready( GstWesterosSink *sink, gboolean *passToDefault );

/**
 * This function is called when the GstWesterosSink object is transitioning from the ready to the null state.
*/
gboolean gst_westeros_sink_soc_ready_to_null( GstWesterosSink *sink, gboolean *passToDefault );

/**
 * The function takes several parameters: a pointer to the GstWesterosSink instance, a pointer to the Wayland registry object, an ID for the new global object, the name of the interface, and its version. 
 * This function is a callback that is called when the Wayland registry receives a new global object.
*/
void gst_westeros_sink_soc_registryHandleGlobal( GstWesterosSink *sink, 
                                 struct wl_registry *registry, uint32_t id,
		                           const char *interface, uint32_t version);

/**
 * This function takes in a pointer to GstWesterosSink structure, a pointer to Wayland registry object, and the ID of the removed object. 
 * It is a callback that gets called when a global object is removed from the Wayland registry.
*/
void gst_westeros_sink_soc_registryHandleGlobalRemove(GstWesterosSink *sink,
                                 struct wl_registry *registry,
			                        uint32_t name);

/**
 * This function takes two parameters: a pointer to the GstWesterosSink instance and a pointer to the caps to be checked. 
 * It returns a boolean value indicating whether the caps are supported by the GstWesterosSink.
*/
gboolean gst_westeros_sink_soc_accept_caps( GstWesterosSink *sink, GstCaps *caps );

/**
 * This function sets the start presentation time stamp (PTS) for the media pipeline of the given GstWesterosSink instance.
*/
void gst_westeros_sink_soc_set_startPTS( GstWesterosSink *sink, gint64 pts );

/**
 * This function is responsible for rendering the video frames to the display and it takes a pointer to the GstWesterosSink object and a pointer to the GstBuffer containing the video frame data as input.
*/
void gst_westeros_sink_soc_render( GstWesterosSink *sink, GstBuffer *buffer );

/**
 * This function is used to reset and flush the state of the Westeros sink element. 
 * It is typically called when the pipeline is being reset or when there is a need to discard all buffered frames and start fresh.
*/
void gst_westeros_sink_soc_flush( GstWesterosSink *sink );

/**
 * This function is used to start the video playback in the Westeros sink. 
 * It returns a boolean indicating whether the start operation was successful.
*/
gboolean gst_westeros_sink_soc_start_video( GstWesterosSink *sink );

/**
 * This function handles an end-of-stream (eos) event for the Westeros sink element and takes a pointer to the GstWesterosSink object as parameter.
*/
void gst_westeros_sink_soc_eos_event( GstWesterosSink *sink );

/**
 * The function is used to set the video path for a given GstWesterosSink object. 
 * It takes a boolean argument useGFXPath which indicates whether to use the graphics path or not.
*/
void gst_westeros_sink_soc_set_video_path( GstWesterosSink *sink, bool useGfxPath );

/**
 * This function updates the position and size of the video display in the window.
*/
void gst_westeros_sink_soc_update_video_position( GstWesterosSink *sink );

/**
 * This function is a callback function that is used by GStreamer to handle queries related to the GstWesterosSink element and takes a pointer to the GstQuery.
*/
gboolean gst_westeros_sink_soc_query( GstWesterosSink *sink, GstQuery *query );

/**
 * Not Implemented currently*
 * This function takes a pointer to the GstWesterosSink and lock parameter, to set the compositor lock state.
*/
void gst_westeros_sink_soc_set_compositor_lock(GstWesterosSink *sink, bool lock);

/**
 * Not Implemented currently*
 * This function takes a pointer to the GstWesterosSink and a pointer to the GValue, to set the playback speed of a video to the specified value.
*/
void gst_westeros_sink_soc_set_speed(GstWesterosSink *sink, const GValue *value);

#endif

