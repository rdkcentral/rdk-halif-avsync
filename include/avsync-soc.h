/*
 * If not stated otherwise in this file or this component's LICENSE file
 * the following copyright and licenses apply:
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

/**
 * @addtogroup HPK Hardware Porting Kit
 * @{
 * @par The Hardware Porting Kit
 * HPK is the next evolution of the well-defined Hardware Abstraction Layer
 * (HAL), but augmented with more comprehensive documentation and test suites
 * that OEM or SOC vendors can use to self-certify their ports before taking
 * them to RDKM for validation or to an operator for final integration and
 * deployment. The Hardware Porting Kit effectively enables an OEM and/or SOC
 * vendor to self-certify their own Video Accelerator devices, with minimal RDKM
 * assistance.
 *
 */

/**
 * @defgroup RDK_AVSYNC RDK AVSync Module
 * @{
 * @par Application API Specification
 * AVSync module provides functionalities for audio/video synchronization
 * during playback, ensuring proper alignment and timing between the streams.
 * Provides API defines the structures and functions for the RDK HAL AVSync.
 *
 */

/**
 * @defgroup RDK_AVSYNC_HAL RDK AVSync HAL
 * @{
 */

/**
 * @defgroup AVSYNC_SOC_H AvSync SOC Header
 * @{
 *
 * @par Application API Specification
 * AVSync module provides functionalities for audio/video synchronization
 * during playback, ensuring proper alignment and timing between the streams.
 * Provides API defines the structures and functions for the RDK HAL AVSync.
 *
 */

/**
 * @file avsync-soc.h
 *
 * @brief RDK HAL AVSync Public API
 *
 * This file defines the structures and functions for the RDK HAL AVSync
 *
 * @par Abbreviations
 * - HAL:       Hardware Abstraction Layer
 * - AV Sync:   Audio Video Synchronization
 * - API:       Application Programming Interface
 * - SoC:       System on Chip
 * - pts:       Presentation timestamp
 *
 */

#ifndef __AVSYNC_H__
#define __AVSYNC_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @struct AVSyncVideoFrame_
 * 
 * @brief This structure is to encapsulate information related to a video
 * frame's metadata.
 */
typedef struct AVSyncVideoFrame_
{
    void*       context;     /**< Pointer to additional context data associated
                                  with the video frame's metadata */
    void*       sync;        /**< Pointer to synchronization data or object
                                  associated with the video frame's metadata */
    void*       videoFrame;  /**< Pointer to the video frame's metadata */
    void*       syncFrame;   /**< Pointer to synchronization frame */
    uint32_t    pts;         /**< Presentation timestamp of the video frame */
    uint32_t    duration;    /**< Duration of the video frame */

} AVSyncVideoFrame;


/**
 * @brief Frame delete callback prototype
 */
typedef void (*AVSyncFrameFree)(AVSyncVideoFrame * frame);


/**
 * @brief Initializes AV Sync session
 * 
 * This function initializes AV Sync session and returns a pointer to an AVSync
 * object used for audio-video synchronization.
 * 
 * @param [in] refreshRate - The display refresh rate in hertz
 *                           Valid range: 1Hz to 1000Hz
 * @param [in] syncType    - The type of syncronization to use
 *                           Valid range: 0 to maximum number of syncType
 * @param [out] sessionId  - A pointer to an integer that will be set to the ID
 * of the avsync session.
 * @param [out] session    - A pointer to an integer that will be set to the
 * file descriptor of the avsync session
 * 
 * @return void* 
 * @retval handle for avsync module or null for failure
 *
 */
void* avsync_soc_init( int refreshRate, int syncType, int *sessionId, int* session );


/**
 * @brief Terminates AV Sync session
 * 
 * This function is used to terminate and clean up resources associated with an
 * AVSync object.
 * 
 * @param [in] sync    - A pointer to the AVSync structure that was created by
 * the avsync_soc_init() function
 * @param [in] session - The file descriptor of the avsync session
 *
 * @return void
 *
 * @pre avsync_soc_init() must be called before calling this API
 * 
 */
void avsync_soc_term( void* sync, int session );


/**
 * @brief Sets Mode for AV Sync session
 * 
 * This function is used to set the synchronization mode for an AVSync object.
 * Possible modes are Video Master, Audio Master and Program Clock Reference 
 * Master.
 * 
 * @param [in] sync - A pointer to the AVSync structure that was created by the
 * avsync_soc_init() function
 * @param [in] type - The new mode to set. Possible values are: 0, 1, 2
 * 
 * @return int - The status of the operation
 * @retval 0 if successful, -1 otherwise
 *
 * @pre avsync_soc_init() must be called before calling this API
 * 
 */
int avsync_soc_set_mode( void* sync, int type );


/**
 * @brief Registers the callback to free the allocated video frames and 
 * metadata.
 * 
 * This function is used to set the frame free callback function for an AVSync
 * object.
 * 
 * @param [in] sync   - A pointer to the AVSync structure that was created by
 * the avsync_soc_init() function
 * @param [in] freeCB - A pointer to the AVSyncFrameFree structure
 * 
 * @return void
 * 
 * @pre avsync_soc_init() must be called before calling this API
 * 
 * @see avsync_soc_init()
 * 
 */
void avsync_soc_free_frame_callback( void* sync, AVSyncFrameFree *freeCB );


/**
 * @brief Push a video frame to the AVSync module.
 * 
 * This function is used to push a video frame to the AVSync module for 
 * synchronization.
 *
 * @param [in] sync - A pointer to the AVSync structure that was created by the
 * avsync_soc_init() function
 * @param [in] f    - A pointer to the AVSyncVideoFrame structure that contains
 * the frame metadata. Valid value: any non-zero value
 * 
 * @return bool - The status of the operation
 * @retval true if successful, false otherwise
 * 
 * @see AVSyncVideoFrame_
 * 
 * @pre avsync_soc_init() must be called before calling this API
 *
 */
bool avsync_soc_push_frame( void* sync, AVSyncVideoFrame *f );


/**
 * @brief Pop a video frame from the AVSync module.
 * 
 * This function is used to pop/retrieve a video frame from the AVSync module's
 * frame queue.
 *  
 * @param [in] sync - A pointer to the AVSync structure that was created by the
 * avsync_soc_init() function
 * 
 * @return - AVSyncVideoFrame*
 * @retval A pointer to the AVSyncVideoFrame structure that contains
 * the frame metadata. Valid value: any non-zero value
 * 
 * @see AVSyncVideoFrame_, avsync_soc_set_interval(), avsync_soc_push_frame()
 * 
 * @pre avsync_soc_init() must be called before calling this API
 * @pre avsync_soc_push_frame() must be called before calling this API
 * 
 */
AVSyncVideoFrame* avsync_soc_pop_frame(void* sync );


/**
 * @brief Sets playback rate for AV Sync session
 * 
 * This function is used to set the playback rate/speed of an AVSync object
 * 
 * @param [in] sync - A pointer to the AVSync structure that was created by the
 * avsync_soc_init() function
 * @param [in] rate - is the playback rate. Valid range: 0.0 to 256.0
 * 
 * @return void
 * 
 * @pre avsync_soc_init() must be called before calling this API
 *
 */
void avsync_soc_set_rate( void* sync, float rate );


/**
 * @brief Pauses or resume AV Sync session
 * 
 * This function is used to pause or resume playback of an AVSync object.
 * 
 * @param [in] sync  - A pointer to the AVSync structure that was created by
 * the avsync_soc_init() function
 * @param [in] pause - a boolean value that indicates whether to pause (true)
 * or resume (false) playback
 * 
 * @return void
 *
 * @pre avsync_soc_init() must be called before calling this API
 * @pre avsync_soc_set_rate() must be called before calling this API
 *
 * @see avsync_soc_set_rate()
 * 
 */
void avsync_soc_pause( void* sync, bool pause );


/**
 * @brief Sets vsync interval for AV Sync session
 * 
 * This function is used to set the interval between vertical synchronization
 * (vsync interval) events for the AVSync module.
 * 
 * @param [in] sync     - A pointer to the AVSync structure that was created by
 * the avsync_soc_init() function
 * @param [in] interval - the desired interval between vsync events. Valid range: 1-1000 milliseconds
 * 
 * @return void
 * 
 * @pre avsync_soc_init() must be called before calling this API
 *
 */
void avsync_soc_set_interval( void* sync, uint32_t interval );


/**
 * @brief Signals EOS for AV Sync session
 * 
 * This function is used to signal the end of stream(EOS) to the AVSync module.
 * 
 * @param [in] sync - A pointer to the AVSync structure that was created by the
 * avsync_soc_init() function
 * 
 * @return void
 * 
 * @pre avsync_soc_init() must be called before calling this API
 *
 */
void avsync_soc_eos( void* sync );


#endif // End of __AVSYNC_H__

/** @} */ // End of AVSYNC_SOC_H
/** @} */ // End of RDK_AVSYNC_HAL
/** @} */ // End of RDK_AVSYNC
/** @} */ // End of HPK
