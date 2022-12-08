/***
 * is_it_christmas.c
 * plagerized mostly from the hello_world example plugin https://github.com/mfulz/Flipper-Plugin-Tutorial
*/

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include "is_it_christmas.h"

static void render_callback(Canvas* const canvas, void* ctx) {
    const PluginState* plugin_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(plugin_state == NULL) {
        return;
    }
    // border around the edge of the screen
    canvas_draw_frame(canvas, 0, 0, 128, 64);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 5, 5, AlignLeft, AlignTop, "Is It Christmas?");
    //Is today Deccember 25th?
    if (plugin_state->is_it_christmas) {
        canvas_draw_str_aligned(
            canvas, 5, 20, AlignLeft, AlignTop, "Yes");
    } else {
        canvas_draw_str_aligned(
            canvas, 5, 20, AlignLeft, AlignTop, "No");
    }

    release_mutex((ValueMutex*)ctx, plugin_state);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void is_it_christmas_state_init(PluginState* const plugin_state) {
    plugin_state->is_it_christmas = false;
        FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
    //Is it December 25th?
    if(datetime.month == 12 && datetime.day == 25){
        plugin_state->is_it_christmas = true;
    }
}

int32_t is_it_christmas_app() {
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    PluginState* plugin_state = malloc(sizeof(PluginState));

    is_it_christmas_state_init(plugin_state);

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, plugin_state, sizeof(PluginState))) {
        FURI_LOG_E("is_it_christmas", "cannot create mutex\r\n");
        free(plugin_state);
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        PluginState* plugin_state = (PluginState*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
                        processing = false;
                }
            }

        } else {
            FURI_LOG_D("is_it_christmas", "FuriMessageQueue: event timeout");
            // event timeout
        }

        view_port_update(view_port);
        release_mutex(&state_mutex, plugin_state);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    delete_mutex(&state_mutex);
    free(plugin_state);

    return 0;
}
