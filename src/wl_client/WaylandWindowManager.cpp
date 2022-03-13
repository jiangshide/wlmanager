#define LOG_TAG "wayland"

#include "WaylandWindowManager.h"
#include <cstring>
#include <cutils/log.h>
// #include "wayland-android-client-protocol.h"
#include "android-wayland-client-protocol.h"
#include <iostream>
#include <sys/resource.h>

#define WAYLAND_THREAD

#ifdef WAYLAND_THREAD
static void *hwc_wayland_thread(void *data) {
    int ret = 0;
    wl_display *dpy = (wl_display *) (data);

    setpriority(PRIO_PROCESS, 0, -8);

    while (ret != -1)
        ret = wl_display_dispatch(dpy);

    ALOGE("*** %s: Wayland client was disconnected: %s", __PRETTY_FUNCTION__, strerror(ret));

    return NULL;
}
#endif// WAYLAND_THREAD

static void
touch_handle_down(void *data, struct wl_touch *,
                  uint32_t, uint32_t, struct wl_surface *surface,
                  int32_t id, wl_fixed_t x_w, wl_fixed_t y_w) {
    std::cout << "down\n";

    std::ignore = data;
    std::ignore = surface;
    std::ignore = id;
    std::ignore = x_w;
    std::ignore = y_w;
}

static void
touch_handle_up(void *data, struct wl_touch *,
                uint32_t, uint32_t, int32_t id) {
    std::cout << "up\n";

    std::ignore = data;
    std::ignore = id;
}

static void
touch_handle_motion(void *data, struct wl_touch *,
                    uint32_t, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w) {
    std::cout << "motion\n";

    std::ignore = data;
    std::ignore = id;
    std::ignore = x_w;
    std::ignore = y_w;
}

static void
touch_handle_frame(void *, struct wl_touch *) {
}

static void
touch_handle_cancel(void *, struct wl_touch *) {
}

static void
touch_handle_shape(void *data, struct wl_touch *, int32_t id, wl_fixed_t major, wl_fixed_t minor) {

    std::ignore = data;
    std::ignore = id;
    std::ignore = major;
    std::ignore = minor;
}

static void
touch_handle_orientation(void *, struct wl_touch *, int32_t, wl_fixed_t) {
}

static const struct wl_touch_listener touch_listener = {
    touch_handle_down,
    touch_handle_up,
    touch_handle_motion,
    touch_handle_frame,
    touch_handle_cancel,
    touch_handle_shape,
    touch_handle_orientation,
};

WaylandWindowManager::WaylandWindowManager() {
    int counter = 1000;
    while (counter-- >= 0) {
        ALOGD("try %d", counter);
        std::string path = "/wayland/wayland-1";
        dpy = wl_display_connect(path.c_str());
        if (dpy != nullptr) {
            break;
        }

        ALOGD("xxyy == error: %d   %s", errno, path.c_str());

        usleep(4 * 1000 * 1000);
    }

    if (!dpy) {
        ALOGE("failed to connect server");
        goto exit_error;
    }

    ALOGD("lijing -------xxyy----- wayland display success");

    registry = wl_display_get_registry(dpy);
    if (!registry) {
        ALOGE("failed to get registry");
        goto exit_error;
    }

    wl_registry_add_listener(registry, &registry_listener, this);

    wl_display_roundtrip(dpy);

    wl_output_add_listener(output, &output_listener, this);
    wl_seat_add_listener(seat, &w_seat_listener, this);

    wl_display_dispatch(dpy);
    wl_display_roundtrip(dpy);

#ifdef WAYLAND_THREAD
    // lijint thread
    pthread_t wayland_thread;// constant after init
    pthread_create(&wayland_thread, NULL, hwc_wayland_thread, dpy);
#endif// WAYLAND_THREAD

    return;

exit_error:
    if (dpy != NULL) {
        wl_display_disconnect(dpy);
    }
    ALOGE("failed to initialize WaylandWindowManager");
}

void WaylandWindowManager::seat_handle_capabilities(void *data, struct wl_seat *seat, uint32_t wcaps) {
    WaylandWindowManager *self = reinterpret_cast<WaylandWindowManager *>(data);
    enum wl_seat_capability caps = (enum wl_seat_capability) wcaps;

    if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !self->touch) {
        self->touch = wl_seat_get_touch(seat);
        //        d->input_fd[INPUT_TOUCH] = -1;
        //        mkfifo(INPUT_PIPE_NAME[INPUT_TOUCH], S_IRWXO | S_IRWXG | S_IRWXU);
        //        chown(INPUT_PIPE_NAME[INPUT_TOUCH], 1000, 1000);
        //        for (int i = 0; i < MAX_TOUCHPOINTS; i++)
        //            d->touch_id[i] = -1;
        wl_touch_set_user_data(self->touch, self);
        wl_touch_add_listener(self->touch, &touch_listener, self);
    }
}

void WaylandWindowManager::registry_add_object(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) {
    std::ignore = version;
    WaylandWindowManager *self = reinterpret_cast<WaylandWindowManager *>(data);

    if (strcmp(interface, "wl_compositor") == 0) {
        ALOGW("registry add object wl_compositor");
        self->compositor = (struct wl_compositor *) wl_registry_bind(registry, name, &wl_compositor_interface, version);
    } else if (strcmp(interface, "wl_shell") == 0) {
        ALOGW("registry add object wl_shell");
        self->shell = (struct wl_shell *) wl_registry_bind(registry, name, &wl_shell_interface, 1);
    } else if (strcmp(interface, "wl_output") == 0) {
        ALOGW("registry add object wl_output");
        self->output = (struct wl_output *) wl_registry_bind(registry, name, &wl_output_interface, 1);
    } else if (strcmp(interface, "wl_seat") == 0) {
        ALOGW("registry add object wl_seat");
        self->seat = (struct wl_seat *) wl_registry_bind(registry, name, &wl_seat_interface, 1);
    } else if (strcmp(interface, "android_wlegl") == 0) {
        ALOGW("registry add object android_wlegl");
        self->a_android_wlegl = static_cast<struct android_wlegl *>(wl_registry_bind(registry, name, &android_wlegl_interface, 1));
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        self->xdg_wm_base = (struct xdg_wm_base *) wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
        // xdg_wm_base_add_listener(state->xdg_wm_base,
        //                         nullptr, state);
    } else {
        ALOGE("ignored interface: %s", interface);
    }
}

void WaylandWindowManager::registry_remove_object(void *data, struct wl_registry *registry, uint32_t name) {
    ALOGW("registry remove object: %d", name);

    std::ignore = data;
    std::ignore = registry;
    std::ignore = name;
}

void WaylandWindowManager::output_handle_geometry(void *data, struct wl_output *wl_output,
                                                  int32_t x, int32_t y, int32_t physical_width, int32_t physical_height,
                                                  int32_t subpixel, const char *make, const char *model, int32_t transform) {
    ALOGW("output handle geometry");

    std::ignore = data;
    std::ignore = wl_output;
    std::ignore = x;
    std::ignore = y;
    std::ignore = physical_width;
    std::ignore = physical_height;
    std::ignore = subpixel;
    std::ignore = make;
    std::ignore = model;
    std::ignore = transform;
}

void WaylandWindowManager::output_handle_mode(void *data, struct wl_output *wl_output,
                                              uint32_t flags, int32_t width, int32_t height, int32_t refresh) {
    ALOGW("output handle mode");
    WaylandWindowManager *self = reinterpret_cast<WaylandWindowManager *>(data);

    self->screen_width = width;
    self->screen_height = height;

    std::ignore = flags;
    std::ignore = wl_output;
    std::ignore = refresh;
}

void WaylandWindowManager::output_handle_done(void *data, struct wl_output *wl_output) {
    ALOGW("output handle done");

    std::ignore = data;
    std::ignore = wl_output;
}

void WaylandWindowManager::output_handle_scale(void *data, struct wl_output *wl_output, int32_t factor) {
    ALOGW("output handle scale");

    std::ignore = data;
    std::ignore = wl_output;
    std::ignore = factor;
}

int WaylandWindowManager::finalize() {
    return 0;
}

WaylandWindowManager::~WaylandWindowManager() {
    wl_display_disconnect(dpy);
}
