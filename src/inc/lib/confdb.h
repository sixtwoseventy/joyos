/*
 * The MIT License
 *
 * Copyright (c) 2007 MIT 6.270 Robotics Competition
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef SIMULATE

#ifndef _CONFDB_H_
#define _CONFDB_H_

#include <kern/global.h>

/**
 * DISABLED UNTIL FURTHER TESTING
 *
 * \XXfile confdb.h
 * \XXbrief Simple non-volatile configuration database
 *
 * Confdb provides a simple way to store configuration parameters in the
 * Happyboards onboard flash memory. The Confdb stores ConfdbItems which can
 * represent either integers or floats. ConfdbItems are stored in flash and are
 * identified by a unique ID number.
 */

// Confdb item types
#define CONFDB_ITEM_INTEGER 0 ///< Integer Confdb item
#define CONFDB_ITEM_FLOAT   1 ///< Float Confdb item

// Confdb return values
#define CONFDB_OK           0 ///< Command OK
#define CONFDB_INVALID_TYPE 1 ///< Item to be loaded/saved has invalid type

/**
 * A Confdb Item
 */
typedef struct {
    uint16_t id  : 14;     ///< Unique ID
    uint16_t type : 2;     ///< Item type
    union {
        uint32_t i_value;  ///< integer value type
        float    f_value;  ///< float value type
    };
} ConfdbItem;


/**
 * Initialise Confdb
 */
void confdb_init();

/**
 * Load a Confdb item from flash
 *
 * @param id    ID of the item to load
 * @param item  allocated structure to be filled with loaded item
 * @return      CONFDB_OK if item loaded ok, otherwise error
 */
uint8_t confdb_load_item(uint16_t id, ConfdbItem *item);

/**
 * Check if a confdb item exists
 *
 * @param id    ID to check
 * @return      true if item exists false otherwise
 */
uint8_t confdb_item_exists(uint16_t id);

/**
 * Save a Confdb item to flash
 *
 * @param item  item to be saved
 * @return      CONFDB_OK if item saved ok, otherwise error
 */
uint8_t confdb_save_item(ConfdbItem *item);

/**
 * Load an integer Confdb item from flash
 *
 * @param id    ID of the item to load
 * @param value integer to load value into
 * @return      CONFDB_OK if item loaded ok, otherwise error
 */
uint8_t confdb_load_integer(uint16_t id, uint16_t *value);

/**
 * Save an integer Confdb item to flash
 *
 * @param id    item id to save to
 * @param value integer to be saved
 * @return      CONFDB_OK if item saved ok, otherwise error
 */
uint8_t confdb_save_integer(uint16_t id, uint16_t value);

#endif

#endif

