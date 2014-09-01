/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/**
 * @file    memstreams.h
 * @brief   Memory streams structures and macros.

 * @addtogroup memory_streams
 * @{
 */

#ifndef _MEMSTREAMS_H_
#define _MEMSTREAMS_H_

#include <stdint.h>
#include <stddef.h>

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/


/**
 * @brief   @p BaseSequentialStream specific data.
 * @note    It is empty because @p BaseSequentialStream is only an interface
 *          without implementation.
 */
#define _base_sequential_stream_data

/**
 * @brief   @p RamStream specific data.
 */
#define _memory_stream_data                                                 \
  _base_sequential_stream_data                                              \
  /* Pointer to the stream buffer.*/                                        \
  uint8_t               *buffer;                                            \
  /* Size of the stream.*/                                                  \
  size_t                size;                                               \
  /* Current end of stream.*/                                               \
  size_t                eos;                                                \
  /* Current read offset.*/                                                 \
  size_t                offset;

/**
 * @brief   Message, use signed pointer equivalent.
 */
typedef int32_t         msg_t;

/**
 * @brief   BaseSequentialStream specific methods.
 */
#define _base_sequential_stream_methods                                     \
  /* Stream write buffer method.*/                                          \
  size_t (*write)(void *instance, const uint8_t *bp, size_t n);             \
  /* Stream read buffer method.*/                                           \
  size_t (*read)(void *instance, uint8_t *bp, size_t n);                    \
  /* Channel put method, blocking.*/                                        \
  msg_t (*put)(void *instance, uint8_t b);                                  \
  /* Channel get method, blocking.*/                                        \
  msg_t (*get)(void *instance);                                             \

/**
 * @brief   @p BaseSequentialStream virtual methods table.
 */
struct BaseSequentialStreamVMT {
  _base_sequential_stream_methods
};

/**
 * @brief   Base stream class.
 * @details This class represents a generic blocking unbuffered sequential
 *          data stream.
 */
typedef struct {
  /** @brief Virtual Methods Table.*/
  const struct BaseSequentialStreamVMT *vmt;
  _base_sequential_stream_data
} BaseSequentialStream;

/**
 * @brief   @p MemStream virtual methods table.
 */
struct MemStreamVMT {
  _base_sequential_stream_methods
};

/**
 * @extends BaseSequentialStream
 *
 * @brief Memory stream object.
 */
typedef struct {
  /** @brief Virtual Methods Table.*/
  const struct MemStreamVMT *vmt;
  _memory_stream_data
} MemoryStream;

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
  void msObjectInit(MemoryStream *msp, uint8_t *buffer,
                    size_t size, size_t eos);
#ifdef __cplusplus
}
#endif

#endif /* _MEMSTREAMS_H_ */

/** @} */
