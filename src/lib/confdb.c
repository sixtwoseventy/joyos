#ifndef SIMULATE

#include "lib/confdb.h"
#include "at45db011.h"

#define ITEMS_PER_PAGE 42

typedef struct {
    uint32_t num_entries;
} ConfdbHeader;

ConfdbHeader confdb_header;

void confdb_init() {
}

uint8_t confdb_item_exists(uint16_t id) {
    return false;
}

uint8_t confdb_load_item(uint16_t id, ConfdbItem *item) {
    uint16_t page = id/ITEMS_PER_PAGE;
    uint16_t pageloc = id%ITEMS_PER_PAGE;
    at45db_start_continuous_read((page<<8) + pageloc);
    at45db_continuous_read_block(sizeof(ConfdbItem),(uint8_t*)item);
    at45db_end_continuous_read();
    return CONFDB_OK;
}

uint8_t confdb_save_item(ConfdbItem *item) {
    uint16_t page = item->id/ITEMS_PER_PAGE;
    uint16_t pageloc = item->id%ITEMS_PER_PAGE;
    at45db_fill_buffer_from_flash(page<<8);
    at45db_fill_buffer(pageloc, (uint8_t*)item, sizeof(ConfdbItem));
    at45db_store_buffer (page<<8);
    return CONFDB_OK;
}

uint8_t confdb_load_integer(uint16_t id, uint16_t *value) {
    ConfdbItem item;
    confdb_load_item(id, &item);
    if (item.type!=CONFDB_ITEM_INTEGER)
        return CONFDB_INVALID_TYPE;
    *value = item.i_value;
    return CONFDB_OK;
}

uint8_t confdb_save_integer(uint16_t id, uint16_t value) {
    ConfdbItem item;
    confdb_load_item(id, &item);
    if (item.type!=CONFDB_ITEM_INTEGER)
        return CONFDB_INVALID_TYPE;
    item.i_value = value;
    confdb_save_item(&item);
    return CONFDB_OK;
}

#endif

