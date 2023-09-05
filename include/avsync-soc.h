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
#define __WESTEROS_GL_AVSYNC_SOC_H__

#include <stdint.h>
#include <stdbool.h>

#define WESTEROS_GL_AVSYNC

#define USE_AVSYNC_SOC

/**
 * @struct AVSyncVideoFrame_
 * 
 * @brief This structure is to encapsulate information related to a video frame's metadata.
*/
typedef struct AVSyncVideoFrame_
{
    void*       context;
    void*       sync;
    void*       videoFrame;
    void*       syncFrame;
    uint32_t    pts;
    uint32_t    duration;

} AVSyncVideoFrame;


/**
 * @brief Frame delete callback prototype
 */
typedef void (*AVSyncFrameFree)(AVSyncVideoFrame * frame);


/**
 * @brief Initializes AV Sync session.
 * 
 * This function initializes AV Sync session and returns a pointer to an AVSync object used for audio-video synchronization.
 * The function takes four arguments.
 * 
 * @param [in] refreshRate - The display refresh rate in hertz.
 * @param [in] syncType    - The type of syncronization to use.
 * 
 * @param [out] sessionId  - A pointer to an integer that will be set to the ID of the avsync session.
 * @param [out] session    - A pointer to an integer that will be set to the file descriptor of the avsync session. 
 * 
 * @return void* 
 * @retval handle for avsync module or null for failure.
 *
*/
void* avsync_soc_init( int refreshRate, int syncType, int *sessionId, int* session );


/**
 * @brief Terminates AV Sync session.
 * 
 * This function is used to terminate and clean up resources associated with an AVSync object.
 * The function takes two arguments.
 * 
 * @param [in] sync    - A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * @param [in] session - The file descriptor of the avsync session.
 * 
 * @return void 
*/
void avsync_soc_term( void* sync, int session );


/**
 * @brief Set Mode for AV Sync session.
 * 
 * This function is used to set the synchronization mode for an AVSync object.
 * Possible modes are VMASTER, AMASTER, PCR_MASTER.
 * The function takes two arguments. 
 * 
 * @param [in] sync - A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * @param [in] type - The new mode to set. 
 * 
 * @return int - The status of the operation.
 * @retval 0 if successful, appropiate error code otherwise.
 * 
*/
int avsync_soc_set_mode( void* sync, int type );


/**
 * @brief Set the callback to free allocated video frames.
 * 
 * This function is used to set the frame free callback function for an AVSync object.
 * The function takes two arguments.
 * 
 * @param [in] sync   - A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * @param [in] freeCB - A pointer to the AVSyncFrameFree structure.
 * 
 * @return void
 * 
 * @pre avsync_soc_init() must be called before calling this API
 * 
 * @see avsync_soc_init()
 * 
 * @todo AVSyncFrameFree
 * 
*/
void avsync_soc_free_frame_callback( void* sync, AVSyncFrameFree *freeCB );


/**
 * @brief Push a video frame to the AVSync module.
 * 
 * This function is used to push a video frame to the AVSync module for synchronization.
 * The function takes two arguments.
 *
 * @param [in] sync - A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * @param [in] f    - A pointer to the AVSyncVideoFrame structure that contains the frame data.
 * 
 * @return bool - The status of the operation.
 * @retval true if successful, appropiate error code otherwise.
 * 
 * @see AVSyncVideoFrame_
 * 
*/
bool avsync_soc_push_frame( void* sync, AVSyncVideoFrame *f );


/**
 * @brief Pop a video frame from the AVSync module.
 * 
 * This function is used to pop/retrieve a video frame from the AVSync module's frame queue.
 * The function takes one argument.
 *  
 * @param [in] sync - A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * 
 * @return - AVSyncVideoFrame*
 * @retval AVSyncVideoFrame f
 * 
 * @see AVSyncVideoFrame_, avsync_soc_set_interval(), avsync_soc_push_frame()
 * 
*/
AVSyncVideoFrame* avsync_soc_pop_frame(void* sync );


/**
 * @brief Set playback rate for AV Sync session.
 * 
 * This function is used to set the playback rate/speed of an AVSync object.
 * The function takes two arguments.
 * 
 * @param [in] sync - A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * @param [in] rate - is the playback rate.
 * 
 * @return void
 * @retval 0 if successful, appropiate error code otherwise.
 * 
*/
void avsync_soc_set_rate( void* sync, float rate );


/**
 * @brief Pause or resume AV Sync session.
 * 
 * This function is used to pause or resume playback of an AVSync object.
 * The function takes two arguments.
 * 
 * @param [in] sync  - A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * @param [in] pause - a boolean value that indicates whether to pause (true) or resume (false) playback.
 * 
 * @return void
 * @retval 0 if successful, appropiate error code otherwise.
 * 
 * @see avsync_soc_set_rate()
 * 
*/
void avsync_soc_pause( void* sync, bool pause );


/**
 * @brief Set vsync interval for AV Sync session.
 * 
 * This function is used to set the interval between vertical synchronization (vsync interval) events for the AVSync module.
 * The function takes two arguments.
 * 
 * @param [in] sync     - A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * @param [in] interval - interval of type uint32_t, representing the desired interval between vsync events.
 * 
 * @return void
 * @retval 0 if successful, appropiate error code otherwise.
 * 
*/
void avsync_soc_set_interval( void* sync, uint32_t interval );


/**
 * @brief Signal EOS for AV Sync session.
 * 
 * This function is used to signal the end of stream (EOS) to the AVSync module.
 * The function takes one argument.
 * 
 * @param [in] sync - A pointer to the AVSync structure that was created by the avsync_soc_init() function.
 * 
 * @return void
 * @retval 0 if successful, appropiate error code otherwise.
 * 
*/
void avsync_soc_eos( void* sync );


#endif
