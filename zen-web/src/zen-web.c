#include <webkit2/webkit2.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glib/gstdio.h>

static gchar *zen_home_uri = NULL;
static gboolean is_fullscreen = FALSE;
static char *program_path = NULL;
static GtkComboBoxText *search_menu = NULL;

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

static void on_entry_activate(GtkEntry *entry, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(entry);
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(user_data);
    gchar *uri = NULL;

    if (g_str_has_prefix(text, "http://") || g_str_has_prefix(text, "https://") || g_str_has_prefix(text, "file://")) {
        uri = g_strdup(text);
    } else {
        const gchar *engine = gtk_combo_box_text_get_active_text(search_menu);
        const gchar *template =
            g_strcmp0(engine, "Bing") == 0 ? "https://www.bing.com/search?q=%s" :
            g_strcmp0(engine, "DuckDuckGo") == 0 ? "https://duckduckgo.com/?q=%s" :
            g_strcmp0(engine, "Quora") == 0 ? "https://www.quora.com/search?q=%s" :
            "https://www.google.com/search?q=%s";
        uri = g_strdup_printf(template, text);
    }

    webkit_web_view_load_uri(web_view, uri);
    g_free(uri);
}

static void go_home(GtkButton *button, gpointer user_data) {
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(user_data);
    if (zen_home_uri) {
        webkit_web_view_load_uri(web_view, zen_home_uri);
    }
}

static void toggle_fullscreen(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    GtkWidget *icon = gtk_button_get_image(button);

    if (!is_fullscreen) {
        gtk_window_fullscreen(window);
        gtk_image_set_from_icon_name(GTK_IMAGE(icon), "view-restore-symbolic", GTK_ICON_SIZE_BUTTON);
        is_fullscreen = TRUE;
    } else {
        gtk_window_unfullscreen(window);
        gtk_image_set_from_icon_name(GTK_IMAGE(icon), "view-fullscreen-symbolic", GTK_ICON_SIZE_BUTTON);
        is_fullscreen = FALSE;
    }
}

static void clear_data(GtkButton *button, gpointer user_data) {
    WebKitWebContext *context = WEBKIT_WEB_CONTEXT(user_data);

    webkit_web_context_clear_cache(context);
    webkit_website_data_manager_clear(
        webkit_web_context_get_website_data_manager(context),
        WEBKIT_WEBSITE_DATA_ALL, 0, NULL, NULL, NULL);

    gchar *data_dir = g_build_filename(g_get_user_data_dir(), "zen-web", NULL);
    gchar *cache_dir = g_build_filename(g_get_user_cache_dir(), "zen-web", NULL);
    g_remove(data_dir);
    g_remove(cache_dir);
    g_free(data_dir);
    g_free(cache_dir);

    execlp(program_path, program_path, NULL);
    _exit(1);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    program_path = argv[0];

    zen_home_uri = resolve_uri("./index.html");

    WebKitWebsiteDataManager *data_manager = webkit_website_data_manager_new(
        "base-data-directory", g_build_filename(g_get_user_data_dir(), "zen-web", NULL),
        "base-cache-directory", g_build_filename(g_get_user_cache_dir(), "zen-web", NULL),
        NULL);
    WebKitWebContext *context = webkit_web_context_new_with_website_data_manager(data_manager);

    WebKitUserContentManager *content_manager = webkit_user_content_manager_new();
    gchar *adblock_script = NULL;
    g_file_get_contents("assets/adblock.js", &adblock_script, NULL, NULL);
    if (adblock_script) {
        WebKitUserScript *script = webkit_user_script_new(
            adblock_script,
            WEBKIT_USER_CONTENT_INJECT_ALL_FRAMES,
            WEBKIT_USER_SCRIPT_INJECT_AT_DOCUMENT_START,
            NULL, NULL);
        webkit_user_content_manager_add_script(content_manager, script);
        g_free(adblock_script);
    }

    WebKitWebView *web_view = WEBKIT_WEB_VIEW(
        webkit_web_view_new_with_user_content_manager(content_manager));

    WebKitSettings *settings = webkit_web_view_get_settings(web_view);
    webkit_settings_set_enable_javascript(settings, TRUE);
    webkit_settings_set_enable_webgl(settings, TRUE);
    webkit_settings_set_enable_webaudio(settings, TRUE);
    webkit_settings_set_enable_media_stream(settings, TRUE);
    webkit_settings_set_enable_write_console_messages_to_stdout(settings, TRUE);
    webkit_settings_set_enable_developer_extras(settings, TRUE);
    webkit_settings_set_enable_page_cache(settings, TRUE);
    webkit_settings_set_enable_site_specific_quirks(settings, TRUE);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Zen-Web");
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);

    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Buscar o escribir URL...");
    gtk_box_pack_start(GTK_BOX(header), entry, TRUE, TRUE, 0);

    search_menu = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
    gtk_combo_box_text_append_text(search_menu, "Google");
    gtk_combo_box_text_append_text(search_menu, "Bing");
    gtk_combo_box_text_append_text(search_menu, "DuckDuckGo");
    gtk_combo_box_text_append_text(search_menu, "Quora");
    gtk_combo_box_set_active(GTK_COMBO_BOX(search_menu), 0);
    gtk_box_pack_start(GTK_BOX(header), GTK_WIDGET(search_menu), FALSE, FALSE, 0);

    GtkWidget *home_btn = gtk_button_new();
    GtkWidget *home_icon = gtk_image_new_from_icon_name("go-home-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(home_btn), home_icon);
    gtk_box_pack_start(GTK_BOX(header), home_btn, FALSE, FALSE, 0);

    GtkWidget *fullscreen_btn = gtk_button_new();
    GtkWidget *fullscreen_icon = gtk_image_new_from_icon_name("view-fullscreen-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(fullscreen_btn), fullscreen_icon);
    gtk_box_pack_start(GTK_BOX(header), fullscreen_btn, FALSE, FALSE, 0);

    GtkWidget *clear_btn = gtk_button_new();
    GtkWidget *trash_icon = gtk_image_new_from_icon_name("user-trash-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(clear_btn), trash_icon);
    gtk_box_pack_start(GTK_BOX(header), clear_btn, FALSE, FALSE, 0);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(web_view), TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    g_signal_connect(entry, "activate", G_CALLBACK(on_entry_activate), web_view);
    g_signal_connect(home_btn, "clicked", G_CALLBACK(go_home), web_view);
    g_signal_connect(fullscreen_btn, "clicked", G_CALLBACK(toggle_fullscreen), window);
    g_signal_connect(clear_btn, "clicked", G_CALLBACK(clear_data), context);
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    webkit_web_view_load_uri(web_view, zen_home_uri);
    gtk_widget_show_all(window);
    gtk_main();

    g_free(zen_home_uri);
    return 0;
}
