#ifndef IS_IT_CHRISTMAS_H
#define IS_IT_CHRISTMAS_H

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    bool is_it_christmas;
} PluginState;

#endif // IS_IT_CHRISTMAS_H