/* Generated by wayland-scanner 1.18.0 */

#ifndef ANDROID_CLIENT_PROTOCOL_H
#define ANDROID_CLIENT_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include "wayland-client.h"

#ifdef  __cplusplus
extern "C" {
#endif

/**
 * @page page_android The android protocol
 * @section page_ifaces_android Interfaces
 * - @subpage page_iface_android_wlegl - Android EGL graphics buffer support
 * - @subpage page_iface_android_wlegl_handle - An Android native_handle_t object
 * - @subpage page_iface_android_wlegl_server_buffer_handle - a server allocated buffer
 * @section page_copyright_android Copyright
 * <pre>
 *
 * Copyright © 2012 Collabora, Ltd.
 *
 * Permission to use, copy, modify, distribute, and sell this
 * software and its documentation for any purpose is hereby granted
 * without fee, provided that the above copyright notice appear in
 * all copies and that both that copyright notice and this permission
 * notice appear in supporting documentation, and that the name of
 * the copyright holders not be used in advertising or publicity
 * pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 * </pre>
 */
struct android_wlegl;
struct android_wlegl_handle;
struct android_wlegl_server_buffer_handle;
struct wl_buffer;

/**
 * @page page_iface_android_wlegl android_wlegl
 * @section page_iface_android_wlegl_desc Description
 *
 * Interface used in the Android wrapper libEGL to share
 * graphics buffers between the server and the client.
 * @section page_iface_android_wlegl_api API
 * See @ref iface_android_wlegl.
 */
/**
 * @defgroup iface_android_wlegl The android_wlegl interface
 *
 * Interface used in the Android wrapper libEGL to share
 * graphics buffers between the server and the client.
 */
extern const struct wl_interface android_wlegl_interface;
/**
 * @page page_iface_android_wlegl_handle android_wlegl_handle
 * @section page_iface_android_wlegl_handle_desc Description
 *
 * The Android native_handle_t is a semi-opaque object, that
 * contains an EGL implementation specific number of int32
 * values and file descriptors.
 *
 * We cannot send a variable size array of file descriptors
 * over the Wayland protocol, so we send them one by one.
 * @section page_iface_android_wlegl_handle_api API
 * See @ref iface_android_wlegl_handle.
 */
/**
 * @defgroup iface_android_wlegl_handle The android_wlegl_handle interface
 *
 * The Android native_handle_t is a semi-opaque object, that
 * contains an EGL implementation specific number of int32
 * values and file descriptors.
 *
 * We cannot send a variable size array of file descriptors
 * over the Wayland protocol, so we send them one by one.
 */
extern const struct wl_interface android_wlegl_handle_interface;
/**
 * @page page_iface_android_wlegl_server_buffer_handle android_wlegl_server_buffer_handle
 * @section page_iface_android_wlegl_server_buffer_handle_desc Description
 *
 * On creation a server_side_buffer object will immediately send
 * the "buffer_fd" and "buffer_ints" events needed by the client to
 * reference the gralloc buffer, followed by the "buffer" event carrying
 * the wl_buffer object.
 * @section page_iface_android_wlegl_server_buffer_handle_api API
 * See @ref iface_android_wlegl_server_buffer_handle.
 */
/**
 * @defgroup iface_android_wlegl_server_buffer_handle The android_wlegl_server_buffer_handle interface
 *
 * On creation a server_side_buffer object will immediately send
 * the "buffer_fd" and "buffer_ints" events needed by the client to
 * reference the gralloc buffer, followed by the "buffer" event carrying
 * the wl_buffer object.
 */
extern const struct wl_interface android_wlegl_server_buffer_handle_interface;

#ifndef ANDROID_WLEGL_ERROR_ENUM
#define ANDROID_WLEGL_ERROR_ENUM
enum android_wlegl_error {
	ANDROID_WLEGL_ERROR_BAD_HANDLE = 0,
	ANDROID_WLEGL_ERROR_BAD_VALUE = 1,
};
#endif /* ANDROID_WLEGL_ERROR_ENUM */

#define ANDROID_WLEGL_CREATE_HANDLE 0
#define ANDROID_WLEGL_CREATE_BUFFER 1
#define ANDROID_WLEGL_GET_SERVER_BUFFER_HANDLE 2


/**
 * @ingroup iface_android_wlegl
 */
#define ANDROID_WLEGL_CREATE_HANDLE_SINCE_VERSION 1
/**
 * @ingroup iface_android_wlegl
 */
#define ANDROID_WLEGL_CREATE_BUFFER_SINCE_VERSION 1
/**
 * @ingroup iface_android_wlegl
 */
#define ANDROID_WLEGL_GET_SERVER_BUFFER_HANDLE_SINCE_VERSION 2

/** @ingroup iface_android_wlegl */
static inline void
android_wlegl_set_user_data(struct android_wlegl *android_wlegl, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) android_wlegl, user_data);
}

/** @ingroup iface_android_wlegl */
static inline void *
android_wlegl_get_user_data(struct android_wlegl *android_wlegl)
{
	return wl_proxy_get_user_data((struct wl_proxy *) android_wlegl);
}

static inline uint32_t
android_wlegl_get_version(struct android_wlegl *android_wlegl)
{
	return wl_proxy_get_version((struct wl_proxy *) android_wlegl);
}

/** @ingroup iface_android_wlegl */
static inline void
android_wlegl_destroy(struct android_wlegl *android_wlegl)
{
	wl_proxy_destroy((struct wl_proxy *) android_wlegl);
}

/**
 * @ingroup iface_android_wlegl
 *
 * This creator method initialises the native_handle_t object
 * with everything except the file descriptors, which have to be
 * submitted separately.
 */
static inline struct android_wlegl_handle *
android_wlegl_create_handle(struct android_wlegl *android_wlegl, int32_t num_fds, struct wl_array *ints)
{
	struct wl_proxy *id;

	id = wl_proxy_marshal_constructor((struct wl_proxy *) android_wlegl,
			 ANDROID_WLEGL_CREATE_HANDLE, &android_wlegl_handle_interface, NULL, num_fds, ints);

	return (struct android_wlegl_handle *) id;
}

/**
 * @ingroup iface_android_wlegl
 *
 * Pass the Android native_handle_t to the server and attach it
 * to the new wl_buffer object.
 *
 * The android_wlegl_handle object must be destroyed immediately
 * after this request.
 */
static inline struct wl_buffer *
android_wlegl_create_buffer(struct android_wlegl *android_wlegl, int32_t width, int32_t height, int32_t stride, int32_t format, int32_t usage, struct android_wlegl_handle *native_handle)
{
	struct wl_proxy *id;

	id = wl_proxy_marshal_constructor((struct wl_proxy *) android_wlegl,
			 ANDROID_WLEGL_CREATE_BUFFER, &wl_buffer_interface, NULL, width, height, stride, format, usage, native_handle);

	return (struct wl_buffer *) id;
}

/**
 * @ingroup iface_android_wlegl
 */
static inline struct android_wlegl_server_buffer_handle *
android_wlegl_get_server_buffer_handle(struct android_wlegl *android_wlegl, int32_t width, int32_t height, int32_t format, int32_t usage)
{
	struct wl_proxy *id;

	id = wl_proxy_marshal_constructor((struct wl_proxy *) android_wlegl,
			 ANDROID_WLEGL_GET_SERVER_BUFFER_HANDLE, &android_wlegl_server_buffer_handle_interface, NULL, width, height, format, usage);

	return (struct android_wlegl_server_buffer_handle *) id;
}

#ifndef ANDROID_WLEGL_HANDLE_ERROR_ENUM
#define ANDROID_WLEGL_HANDLE_ERROR_ENUM
enum android_wlegl_handle_error {
	ANDROID_WLEGL_HANDLE_ERROR_TOO_MANY_FDS = 0,
};
#endif /* ANDROID_WLEGL_HANDLE_ERROR_ENUM */

#define ANDROID_WLEGL_HANDLE_ADD_FD 0
#define ANDROID_WLEGL_HANDLE_DESTROY 1


/**
 * @ingroup iface_android_wlegl_handle
 */
#define ANDROID_WLEGL_HANDLE_ADD_FD_SINCE_VERSION 1
/**
 * @ingroup iface_android_wlegl_handle
 */
#define ANDROID_WLEGL_HANDLE_DESTROY_SINCE_VERSION 1

/** @ingroup iface_android_wlegl_handle */
static inline void
android_wlegl_handle_set_user_data(struct android_wlegl_handle *android_wlegl_handle, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) android_wlegl_handle, user_data);
}

/** @ingroup iface_android_wlegl_handle */
static inline void *
android_wlegl_handle_get_user_data(struct android_wlegl_handle *android_wlegl_handle)
{
	return wl_proxy_get_user_data((struct wl_proxy *) android_wlegl_handle);
}

static inline uint32_t
android_wlegl_handle_get_version(struct android_wlegl_handle *android_wlegl_handle)
{
	return wl_proxy_get_version((struct wl_proxy *) android_wlegl_handle);
}

/**
 * @ingroup iface_android_wlegl_handle
 */
static inline void
android_wlegl_handle_add_fd(struct android_wlegl_handle *android_wlegl_handle, int32_t fd)
{
	wl_proxy_marshal((struct wl_proxy *) android_wlegl_handle,
			 ANDROID_WLEGL_HANDLE_ADD_FD, fd);
}

/**
 * @ingroup iface_android_wlegl_handle
 */
static inline void
android_wlegl_handle_destroy(struct android_wlegl_handle *android_wlegl_handle)
{
	wl_proxy_marshal((struct wl_proxy *) android_wlegl_handle,
			 ANDROID_WLEGL_HANDLE_DESTROY);

	wl_proxy_destroy((struct wl_proxy *) android_wlegl_handle);
}

/**
 * @ingroup iface_android_wlegl_server_buffer_handle
 * @struct android_wlegl_server_buffer_handle_listener
 */
struct android_wlegl_server_buffer_handle_listener {
	/**
	 */
	void (*buffer_fd)(void *data,
			  struct android_wlegl_server_buffer_handle *android_wlegl_server_buffer_handle,
			  int32_t fd);
	/**
	 */
	void (*buffer_ints)(void *data,
			    struct android_wlegl_server_buffer_handle *android_wlegl_server_buffer_handle,
			    struct wl_array *ints);
	/**
	 * the wl_buffer
	 *
	 * This event will be sent after the ints and all the fds have
	 * been sent
	 */
	void (*buffer)(void *data,
		       struct android_wlegl_server_buffer_handle *android_wlegl_server_buffer_handle,
		       struct wl_buffer *buffer,
		       int32_t format,
		       int32_t stride);
};

/**
 * @ingroup iface_android_wlegl_server_buffer_handle
 */
static inline int
android_wlegl_server_buffer_handle_add_listener(struct android_wlegl_server_buffer_handle *android_wlegl_server_buffer_handle,
						const struct android_wlegl_server_buffer_handle_listener *listener, void *data)
{
	return wl_proxy_add_listener((struct wl_proxy *) android_wlegl_server_buffer_handle,
				     (void (**)(void)) listener, data);
}

/**
 * @ingroup iface_android_wlegl_server_buffer_handle
 */
#define ANDROID_WLEGL_SERVER_BUFFER_HANDLE_BUFFER_FD_SINCE_VERSION 1
/**
 * @ingroup iface_android_wlegl_server_buffer_handle
 */
#define ANDROID_WLEGL_SERVER_BUFFER_HANDLE_BUFFER_INTS_SINCE_VERSION 1
/**
 * @ingroup iface_android_wlegl_server_buffer_handle
 */
#define ANDROID_WLEGL_SERVER_BUFFER_HANDLE_BUFFER_SINCE_VERSION 1


/** @ingroup iface_android_wlegl_server_buffer_handle */
static inline void
android_wlegl_server_buffer_handle_set_user_data(struct android_wlegl_server_buffer_handle *android_wlegl_server_buffer_handle, void *user_data)
{
	wl_proxy_set_user_data((struct wl_proxy *) android_wlegl_server_buffer_handle, user_data);
}

/** @ingroup iface_android_wlegl_server_buffer_handle */
static inline void *
android_wlegl_server_buffer_handle_get_user_data(struct android_wlegl_server_buffer_handle *android_wlegl_server_buffer_handle)
{
	return wl_proxy_get_user_data((struct wl_proxy *) android_wlegl_server_buffer_handle);
}

static inline uint32_t
android_wlegl_server_buffer_handle_get_version(struct android_wlegl_server_buffer_handle *android_wlegl_server_buffer_handle)
{
	return wl_proxy_get_version((struct wl_proxy *) android_wlegl_server_buffer_handle);
}

/** @ingroup iface_android_wlegl_server_buffer_handle */
static inline void
android_wlegl_server_buffer_handle_destroy(struct android_wlegl_server_buffer_handle *android_wlegl_server_buffer_handle)
{
	wl_proxy_destroy((struct wl_proxy *) android_wlegl_server_buffer_handle);
}

#ifdef  __cplusplus
}
#endif

#endif