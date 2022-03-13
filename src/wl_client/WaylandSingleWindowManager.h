#ifndef WAYLAND_SINGLE_WINDOW_MANAGER__
#define WAYLAND_SINGLE_WINDOW_MANAGER__

#include <map>
#include <string>
#include <vector>

#include "WaylandWindowManager.h"

#include "android-wayland-client-protocol.h"
#include "xdg-shell-client-protocol.h"

class WaylandSingleWindowManager : public WaylandWindowManager {
 private:
    int pre_format;
    struct wl_shell_surface *w_shell_surface;
    struct wl_surface *w_surface;

    static void shell_surface_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial);
    static void shell_surface_configure(void *data,
                                        struct wl_shell_surface *shell_surface,
                                        uint32_t edges,
                                        int32_t width,
                                        int32_t height);
    static void shell_surface_popup_done(void *data, struct wl_shell_surface *shell_surface);

    static void buffer_release(void *data, struct wl_buffer *buffer);

    static void frame_callback(void *data, struct wl_callback *callback, uint32_t time);

    static void
    xdg_surface_configure(void *data,
                          struct xdg_surface *xdg_surface, uint32_t serial) {
        std::ignore = data;
        xdg_surface_ack_configure(xdg_surface, serial);
    }
#if 1
    const struct wl_buffer_listener w_buffer_listener = {
        buffer_release};

    const struct wl_callback_listener w_frame_listener = {
        frame_callback};
#endif

    struct wl_callback *frame_callback_ptr;

 protected:
    // xdg
    const struct xdg_surface_listener xdg_surface_listener = {
        .configure = xdg_surface_configure,
    };

    struct wl_shell_surface_listener
        shell_surface_listener = {&shell_surface_ping, &shell_surface_configure, &shell_surface_popup_done};

 public:
    int initialize() override;
    void pushFrame(std::vector<std::string> layer_names,
                   buffer_handle_t buffer,
                   int width,
                   int height,
                   int stride,
                   int pixel_format) override;
    int finalize() override;
    std::map<buffer_handle_t, struct wl_buffer *> buffer_map;

    WaylandSingleWindowManager() : WaylandWindowManager() {
    }

    ~WaylandSingleWindowManager() {
    }
};

#endif
