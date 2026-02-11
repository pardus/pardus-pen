#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>

static char* listen_for_response(DBusConnection *conn, const char *request_path) {
    DBusError err;
    dbus_error_init(&err);
    char *ret = NULL;

    char match[512];
    snprintf(match, sizeof(match),
             "type='signal',interface='org.freedesktop.portal.Request',member='Response',path='%s'",
             request_path);

    dbus_bus_add_match(conn, match, &err);
    dbus_connection_flush(conn);

    printf("Waiting for Response signal...\n");

    while (1) {
        dbus_connection_read_write(conn, 0);
        DBusMessage *msg = dbus_connection_pop_message(conn);

        if (!msg) {
            usleep(10);
            continue;
        }

        if (dbus_message_is_signal(msg,
                                   "org.freedesktop.portal.Request",
                                   "Response")) {

            DBusMessageIter args;
            if (!dbus_message_iter_init(msg, &args)) {
                printf("Signal has no arguments\n");
            } else {
                dbus_uint32_t response;
                dbus_message_iter_get_basic(&args, &response);

                if (response == 0) {
                    dbus_message_iter_next(&args);

                    DBusMessageIter dict;
                    dbus_message_iter_recurse(&args, &dict);

                    while (dbus_message_iter_get_arg_type(&dict) != DBUS_TYPE_INVALID) {
                        DBusMessageIter entry;
                        dbus_message_iter_recurse(&dict, &entry);

                        const char *key;
                        dbus_message_iter_get_basic(&entry, &key);

                        if (strcmp(key, "uri") == 0) {
                            dbus_message_iter_next(&entry);

                            DBusMessageIter variant;
                            dbus_message_iter_recurse(&entry, &variant);

                            const char *uri;
                            dbus_message_iter_get_basic(&variant, &uri);

                            ret = calloc(sizeof(char), strlen(uri));
                            strncpy(ret, uri, strlen(uri));
                        }

                        dbus_message_iter_next(&dict);
                    }
                } else {
                    printf("Failed to take screenshot\n");
                }
            }

            dbus_message_unref(msg);
            break;
        }

        dbus_message_unref(msg);
    }
    return ret;
}

char* screenshot_xdg_portal() {
    DBusError err;
    dbus_error_init(&err);

    DBusConnection *conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error: %s\n", err.message);
        dbus_error_free(&err);
        return NULL;
    }

    if (!conn) return NULL;

    DBusMessage *msg = dbus_message_new_method_call(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.Screenshot",
        "Screenshot"
    );

    if (!msg) return NULL;

    const char *parent_window = "";

    DBusMessageIter args;
    dbus_message_iter_init_append(msg, &args);

    // string parent_window
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &parent_window);

    // a{sv} options
    DBusMessageIter dict;
    dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &dict);

    // handle_token
    DBusMessageIter entry;
    dbus_message_iter_open_container(&dict, DBUS_TYPE_DICT_ENTRY, NULL, &entry);

    const char *key = "handle_token";
    dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);

    DBusMessageIter variant;
    dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "s", &variant);

    const char *token = "mytoken";
    dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &token);

    dbus_message_iter_close_container(&entry, &variant);
    dbus_message_iter_close_container(&dict, &entry);

    dbus_message_iter_close_container(&args, &dict);

    DBusPendingCall *pending;
    if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
        fprintf(stderr, "Out Of Memory!\n");
        return NULL;
    }

    if (!pending) return NULL;

    dbus_connection_flush(conn);
    dbus_message_unref(msg);

    dbus_pending_call_block(pending);
    DBusMessage *reply = dbus_pending_call_steal_reply(pending);
    dbus_pending_call_unref(pending);

    if (!reply) return NULL;

    const char *request_path;
    if (!dbus_message_get_args(reply, &err,
                               DBUS_TYPE_OBJECT_PATH, &request_path,
                               DBUS_TYPE_INVALID)) {
        fprintf(stderr, "Reply Error: %s\n", err.message);
        dbus_error_free(&err);
        return NULL;
    }

    printf("Request path: %s\n", request_path);

    dbus_message_unref(reply);

    return listen_for_response(conn, request_path);
}
