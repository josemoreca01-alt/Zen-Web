#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    if (event->keyval == GDK_KEY_Escape) {
        gtk_main_quit();
        return TRUE;
    }
    return FALSE;
}

static gboolean is_local_file(const char *input) {
    struct stat st;
    return stat(input, &st) == 0;
}

static gchar *resolve_uri(const char *input) {
    if (is_local_file(input)) {
        char *resolved = realpath(input, NULL);
        if (resolved) {
            gchar *uri = g_strdup_printf("file://%s", resolved);
            free(resolved);
            return uri;
        }
    }

    if (g_str_has_prefix(input, "http://") || g_str_has_prefix(input, "https://")) {
        return g_strdup(input);
    }

    return g_strdup_printf("https://%s", input);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    const char *target = argc > 1 ? argv[1] : "./zen.html";
    gchar *uri = resolve_uri(target);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Zen Kiosk");
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);

    // Crear contexto efímero (modo incógnito)
    WebKitWebsiteDataManager *data_manager = webkit_website_data_manager_new_ephemeral();
    WebKitWebContext *context = webkit_web_context_new_with_website_data_manager(data_manager);
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new_with_context(context));

    // Activar capacidades modernas
    WebKitSettings *settings = webkit_web_view_get_settings(web_view);
    webkit_settings_set_enable_javascript(settings, TRUE);
    webkit_settings_set_enable_webgl(settings, TRUE);
    webkit_settings_set_enable_webaudio(settings, TRUE);
    webkit_settings_set_enable_media_stream(settings, TRUE);
    webkit_settings_set_enable_write_console_messages_to_stdout(settings, TRUE);
    webkit_settings_set_enable_developer_extras(settings, TRUE);
    webkit_settings_set_enable_page_cache(settings, TRUE);
    webkit_settings_set_enable_site_specific_quirks(settings, TRUE);

    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(web_view));

    // Ejecutar .jar si es necesario
    if (g_str_has_suffix(target, ".jar") && is_local_file(target)) {
        if (fork() == 0) {
            execlp("java", "java", "-jar", target, (char *)NULL);
            _exit(1);
        }
        webkit_web_view_load_html(web_view,
            "<html><body style='font-family:sans-serif;text-align:center;padding-top:20%;'>"
            "<h2>Ejecutando archivo .jar...</h2>"
            "<p>Esta aplicación Java se está ejecutando externamente.</p>"
            "</body></html>", NULL);
    } else {
        webkit_web_view_load_uri(web_view, uri);
    }

    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    g_free(uri);
    return 0;
}
