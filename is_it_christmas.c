/**
 * Code generally adapted from Simple Flashlight special for @Svaarich by @xMasterX
 * mostly plagerized really
 * 
 **/
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <gui/elements.h>
#include "is_it_christmas.h"

static void render_callback(Canvas* const canvas, void* ctx){
    const PluginState* plugin_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(plugin_state == NULL) {
        return;
    }
    
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 64, 3, AlignCenter, AlignTop, "Is it Christmas?");
    canvas_set_font(canvas, FontSecondary);
    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
    //Is it December 25th?
    if(datetime.month == 12 && datetime.day == 25){
        elements_multiline_text_aligned(canvas, 64, 3, AlignCenter, AlignBottom, "Yes it is!");
    } else {
        elements_multiline_text_aligned(canvas, 64, 3, AlignCenter, AlignBottom, "Nope! Not today.");
    }
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue){
    furi_assert(event_queue);
    PluginEvent event = {
        .type = EventTypeKey,
        .input = *input_event,
    };
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t christmas_app_main(void* p){
    UNUSED(p);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    PluginState* plugin_state = malloc(sizeof(PluginState));
    ValueMutex state_mutex;

    if(!init_mutex(&state_mutex, plugin_state, sizeof(PluginState))) {
        FURI_LOG_E("Christmas app", "Failed to init mutex");
        furi_message_queue_free(event_queue);
        free(plugin_state);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;

    for(bool processing = true; processing;){
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        PluginState* plugin_state = (PluginState*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk){
            if(event.type == EventTypeKey && event.input.type == InputTypePress){
                switch(event.input.key){
                    case InputKeyBack:
                        processing = false;
                        break;
                    default:
                        break;
                }
            }
        }
        view_port_update(view_port);
        release_mutex(&state_mutex, plugin_state);
    }
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    delete_mutex(&state_mutex);
    return 0;
}