
#define ATRACE_TAG ATRACE_TAG_GRAPHICS
#define LOG_TAG "wayland"

#include "WaylandSingleWindowManager.h"
#include <cutils/log.h>
#include <hardware/gralloc.h>
#include <utils/Trace.h>
#define QT_SURFACE_EXTENSION_GET_EXTENDED_SURFACE 0

int WaylandSingleWindowManager::initialize() {
    ALOGW("initializing WaylandSingleWindowManager");

    ALOGW("creating surface");
    w_surface = wl_compositor_create_surface(WaylandWindowManager::compositor);

    if (w_surface == nullptr) {
        ALOGW("w_surface is null");
        return -1;
    }

    ALOGW("getting shell surface");
    WaylandWindowManager::xdg_surface = xdg_wm_base_get_xdg_surface(WaylandWindowManager::xdg_wm_base, w_surface);
    // w_shell_surface = wl_shell_get_shell_surface(shell, w_surface);

    ALOGW("adding listeners");
    xdg_surface_add_listener(WaylandWindowManager::xdg_surface, &xdg_surface_listener, nullptr);
    // wl_shell_surface_add_listener(w_shell_surface, &shell_surface_listener, NULL);

    // wl_shell_surface_set_toplevel(w_shell_surface);
    WaylandWindowManager::xdg_toplevel = xdg_surface_get_toplevel(WaylandWindowManager::xdg_surface);

    // xdg_toplevel_set_title(WaylandWindowManager::xdg_toplevel, "android-app");
    // xdg_toplevel_set_app_id(WaylandWindowManager::xdg_toplevel, "android-app");

    //    ALOGW("creating wl region");
    //    struct wl_region *region;
    //    region = wl_compositor_create_region(WaylandWindowManager::compositor);
    //    wl_region_add(region, 0, 0,
    //                  screen_width,
    //                  screen_height);
    //    wl_surface_set_opaque_region(w_surface, region);
    //    wl_region_destroy(region);

    xdg_toplevel_set_title(WaylandWindowManager::xdg_toplevel, "android-app");
    xdg_toplevel_set_app_id(WaylandWindowManager::xdg_toplevel, "android-app");
    xdg_toplevel_set_fullscreen(WaylandWindowManager::xdg_toplevel, nullptr);
    wl_surface_commit(w_surface);

    /* Here we retrieve objects if executed without immed, or error */
    wl_display_roundtrip(dpy);
    wl_surface_commit(w_surface);

    frame_callback_ptr = 0;

    // lijing
    pre_format = -1;

    return 0;
}

void WaylandSingleWindowManager::pushFrame(std::vector<std::string> layer_names, buffer_handle_t buffer,
                                           int width, int height, int stride, int pixel_format) {
    std::ignore = layer_names;
    ATRACE_CALL();

    // if (access("/data/exist", F_OK) != 0) {
    //     return;
    // }

    // if (pre_format != pixel_format) {
    //     pre_format = pixel_format;

    //     // finalize();
    //for(std::map<buffer_handle_t, struct wl_buffer*>::iterator it = buffer_map.begin();it != buffer_map.end();it++) {
    //    wl_buffer_destroy(it->second);
    //}
    //     buffer_map.clear();
    // }

    if (buffer_map.find(buffer) == buffer_map.end()) {
        struct wl_buffer *w_buffer;
        struct wl_array ints;
        int *the_ints;
        struct android_wlegl_handle *wlegl_handle;

        wl_array_init(&ints);
        the_ints = (int *) wl_array_add(&ints, buffer->numInts * sizeof(int));
        memcpy(the_ints, buffer->data + buffer->numFds, buffer->numInts * sizeof(int));
        wlegl_handle = android_wlegl_create_handle(a_android_wlegl, buffer->numFds, &ints);
        wl_array_release(&ints);

        for (int i = 0; i < buffer->numFds; i++) {
            android_wlegl_handle_add_fd(wlegl_handle, buffer->data[i]);
        }

        w_buffer = android_wlegl_create_buffer(a_android_wlegl, width, height, stride, pixel_format, GRALLOC_USAGE_HW_RENDER, wlegl_handle);
        android_wlegl_handle_destroy(wlegl_handle);

        wl_buffer_add_listener(w_buffer, &w_buffer_listener, this);

        buffer_map[buffer] = w_buffer;
    }

//    int ret = 0;
//    while (frame_callback_ptr && ret != -1) {
//        ret = wl_display_dispatch(dpy);
//    }

    frame_callback_ptr = wl_surface_frame(w_surface);
    wl_callback_add_listener(frame_callback_ptr, &w_frame_listener, this);

    /////
    wl_surface_attach(w_surface, buffer_map[buffer], 0, 0);
    wl_surface_damage(w_surface, 0, 0, width, height);
    wl_surface_commit(w_surface);
    wl_display_flush(dpy);
}

void WaylandSingleWindowManager::buffer_release(void *data, struct wl_buffer *buffer) {
    // we're cleaning in finalize()

    std::ignore = data;
    std::ignore = buffer;
}

void WaylandSingleWindowManager::shell_surface_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial) {
    ALOGW("shell surface ping");
    wl_shell_surface_pong(shell_surface, serial);

    std::ignore = data;
}

void WaylandSingleWindowManager::shell_surface_configure(void *data, struct wl_shell_surface *shell_surface,
                                                         uint32_t edges, int32_t width, int32_t height) {
    ALOGW("shell surface configure");

    std::ignore = data;
    std::ignore = shell_surface;
    std::ignore = edges;
    std::ignore = width;
    std::ignore = height;
}

void WaylandSingleWindowManager::shell_surface_popup_done(void *data, struct wl_shell_surface *shell_surface) {
    ALOGW("shell surface popup done");

    std::ignore = data;
    std::ignore = shell_surface;
}

void WaylandSingleWindowManager::frame_callback(void *data, struct wl_callback *callback, uint32_t time) {
    WaylandSingleWindowManager *self = (WaylandSingleWindowManager *) data;
    self->frame_callback_ptr = 0;
    wl_callback_destroy(callback);

    std::ignore = time;
}

int WaylandSingleWindowManager::finalize() {
    for (std::map<buffer_handle_t, struct wl_buffer *>::iterator it = buffer_map.begin(); it != buffer_map.end(); it++) {
        wl_buffer_destroy(it->second);
    }
    buffer_map.clear();

    wl_shell_surface_destroy(w_shell_surface);
    wl_surface_destroy(w_surface);
    return 0;
}
