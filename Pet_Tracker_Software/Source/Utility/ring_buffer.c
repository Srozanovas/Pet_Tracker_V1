/**********************************************************************************************************************
 * Includes
 *********************************************************************************************************************/
#include "ring_bufer.h"

/**********************************************************************************************************************
 * Private definitions and macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private typedef
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private constants
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Private variables
 *********************************************************************************************************************/
bool RingBuffer_Init (sRingBuffer_t **rb_handle, uint16_t size) {
    *rb_handle = (sRingBuffer_t*) calloc(1, sizeof(sRingBuffer_t));
    if (rb_handle == NULL) {
        return false;
    }
    (*rb_handle)->buffer = (uint8_t*) calloc(size, sizeof(uint8_t));
    if ((*rb_handle)->buffer == NULL) {
        free(*rb_handle);
        *rb_handle = NULL;
        return false;
    }
    (*rb_handle)->max_size = size;
    (*rb_handle)->empty = true;
    (*rb_handle)->tail = size - 1;
    (*rb_handle)->head = size - 1;
    return true;
}

bool RingBuffer_Get (sRingBuffer_t *rb_handle, uint8_t *data) {
    if (rb_handle == NULL) {
    	return false;
    }
	if (rb_handle->buffer == NULL) {
        return false;
    }
    if (rb_handle->empty == true) {
        return false;
    }
    rb_handle->tail = (rb_handle->tail + 1) % rb_handle->max_size;
    *data = *(rb_handle->buffer + rb_handle->tail);
    rb_handle->full = false;
    if (rb_handle->tail == rb_handle->head) {
        rb_handle->empty = true;
        rb_handle->head = rb_handle->tail;
    }
    return true;
}

bool RingBuffer_Put (sRingBuffer_t *rb_handle, uint8_t data) {
	if (rb_handle == NULL) {
	    	return false;
	    }
	if (rb_handle->buffer == NULL) {
        return false;
    }
    rb_handle->head = (rb_handle->head + 1) % rb_handle->max_size;
    *(rb_handle->buffer + rb_handle->head) = data;
    rb_handle->empty = false;
    if (rb_handle->head == rb_handle->tail) {
        rb_handle->full = true;
        rb_handle->tail = (rb_handle->head + 1) % rb_handle->max_size;
    }
    return true;
}

/**********************************************************************************************************************
 * Exported variables and references
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Prototypes of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of private functions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Definitions of exported functions
 *********************************************************************************************************************/
