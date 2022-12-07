#ifndef IS_IT_CHRISTMAS_H
#define IS_IT_CHRISTMAS_H

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <gui/elements.h>

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    bool is_on;
} PluginState;

#endif