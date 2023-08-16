/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __WESTEROS_GL_AVSYNC_H__
#define __WESTEROS_GL_AVSYNC_H__

#define WESTEROS_GL_AVSYNC

#define VFM_QUEUE_CAPACITY (16)
typedef struct _VideoFrameManager
{
   VideoServerConnection *conn;
   int queueSize;
   int queueCapacity;
   VideoFrame *queue;
   pthread_mutex_t mutex;
   bool paused;
   bool resetBaseTime;
   bool frameAdvance;
   float rate;
   long long vblankTime;
   long long vblankInterval;
   long long vblankIntervalPrev;
   long long flipTimeBase;
   long long frameTimeBase;
   long long flipTimeCurrent;
   long long frameTimeCurrent;
   long long adjust;
   long long displayedFrameTime;
   long long expireLimit;
   int dropFrameCount;
   int dropFrameCountReported;
   bool underflowDetected;
   bool underflowReported;
   int bufferIdCurrent;
   bool syncInit;
   void *sync;
   int syncSession;
   #ifdef USE_GENERIC_AVSYNC
   int avscFd;
   int avscSize;
   AVSyncCtrl *avscCtrl;
   long long avscAVTime;
   bool avscAdjust;
   #endif
} VideoFrameManager;

typedef struct _VideoFrame
{
   WstOverlayPlane *plane;
   bool hide;
   bool hidden;
   bool canExpire;
   bool dropped;
   bool advanced;
   uint32_t fbId;
   uint32_t handle0;
   uint32_t handle1;
   int fd0;
   int fd1;
   int fd2;
   uint32_t frameWidth;
   uint32_t frameHeight;
   uint32_t frameFormat;
   uint32_t frameWidthVisible;
   uint32_t frameHeightVisible;
   int rectX;
   int rectY;
   int rectW;
   int rectH;
   int frameNumber;
   int bufferId;
   long long frameTime;
   void *vf;
} VideoFrame;


/**
 * This api initializes the AV sync module used for playing video frames in sync with the audio and handles the video playback on a device.
 * The function takes two parameters: a pointer to a VideoFrameManager object and a session ID. 
 * The session ID is used to identify a particular AV sync session, which may be shared across multiple video playback instances. 
 * //TO-DO: This function is static by default currently; change the type to global.
*/
static void wstAVSyncInit( VideoFrameManager *vfm, int sessionId );

/**
 * This api is used to clean up and terminate the AV sync module used for video playback that was previously initialized by wstAVSyncInit. 
 * It destroys the AV sync module and closes the AV sync session (if applicable), and sets the necessary fields in the VideoFrameManager object to indicate that the AV sync module is no longer active or initialized.
 * The function takes a single parameter, a pointer to a VideoFrameManager object. 
 * The VideoFrameManager object contains information about the video playback session, including the AV sync module.
 * //TO-DO: This function is static by default currently; change the type to global.
*/
static void wstAVSyncTerm( VideoFrameManager *vfm );

/**
 * This api is used to set the synchronization type used by the AV sync module. 
 * It checks if the AV sync module is active, sets the synchronization type, and outputs a message indicating the new mode.
 * The function takes two parameters: a pointer to a VideoFrameManager object and an integer value representing the synchronization type. 
 * The synchronization type determines how the AV sync module synchronizes the video frames with the audio.
 * //TO-DO: This function is static by default currently; change the type to global.
*/
static void wstAVSyncSetSyncType( VideoFrameManager *vfm, int type );

/**
 * This api is used to push a video frame to the AV sync module for synchronization with the audio.
 * The function takes two parameters: a pointer to a VideoFrameManager object and a pointer to a VideoFrame object representing the video frame to be pushed.
 * //TO-DO: This function is static by default currently; change the type to global.
*/
static void wstAVSyncPush( VideoFrameManager *vfm, VideoFrame *f );

/**
 * This api is used to pop a video frame from the AV sync module after it has been synchronized with the audio.
 * The function takes a single parameter, a pointer to a VideoFrameManager object.
 * //TO-DO: This function is static by default currently; change the type to global.
*/
static VideoFrame *wstAVSyncPop( VideoFrameManager *vfm );

/**
 * This api is used to pause or resume the AV sync module.
 * The function takes two parameters: a pointer to a VideoFrameManager object and a boolean pause value indicating whether to pause or resume the AV sync module.
 * //TO-DO: This function is static by default currently; change the type to global.
*/
static void wstAVSyncPause( VideoFrameManager *vfm, bool pause );

/**
 * This api is used to signal the AV sync module that the end of the video stream has been reached.
 * The function takes a single parameter, a pointer to a VideoFrameManager object.
 * //TO-DO: This function is static by default currently; change the type to global.
*/
static void wstAVSyncEos( VideoFrameManager *vfm );

#endif
