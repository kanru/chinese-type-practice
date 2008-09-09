#include <gtk/gtk.h>
#include <glib.h>
#include <pango/pangocairo.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "structs.h"

int gwidth, gheight;
GArray *units;
int score;
float speed;

static void display_units(cairo_t *cr)
{
    int i;
    unit_t u;
    PangoLayout *layout;
    PangoFontDescription *desc;
    layout = pango_cairo_create_layout(cr);
    desc = pango_font_description_from_string("Sans 20");
    pango_layout_set_font_description(layout, desc);
    pango_font_description_free(desc);
    cairo_save(cr);
    cairo_set_source_rgb(cr, 1., 1., 1.);
    if(units)
        for(i = 0; i < units->len; ++i) {
            u = g_array_index(units, unit_t, i);
            cairo_move_to(cr, u.x, u.y);
            pango_layout_set_text(layout, u.v, -1);
            pango_cairo_show_layout (cr, layout);
        }
    cairo_restore(cr);
    g_object_unref (layout);
}

static gboolean update_da(GtkWidget *w, GdkEventExpose *ev, gpointer ud)
{
    GdkRectangle *r = &ev->area;
    GtkAllocation *al = &w->allocation;
    cairo_t *cr = gdk_cairo_create(GDK_DRAWABLE(w->window));
    cairo_set_source_rgb(cr, 0., 0., 0.);
    cairo_rectangle(cr, r->x, r->y, r->width, r->height);
    cairo_clip(cr);
    cairo_rectangle(cr, r->x, r->y, r->width, r->height);
    cairo_fill(cr);
    cairo_set_source_rgb(cr, 0.5, 0., 0.);
    cairo_translate(cr, gwidth/2., gheight/2.);
    cairo_arc(cr, 0., 0., 100., 0., 2.*M_PI);
    cairo_stroke(cr);
    cairo_arc(cr, 0., 0., 50., 0., 2.*M_PI);
    cairo_fill(cr);
    display_units(cr);
    cairo_destroy(cr);
    return TRUE;
}

static void size_allocate(GtkWidget *w, GtkAllocation *alloc, gpointer ud)
{
    gwidth = alloc->width;
    gheight = alloc->height;
}

static void create_units()
{
    int i;
    unit_t t;
    units = g_array_new(TRUE, TRUE, sizeof(unit_t));
    t.v = g_strdup("測");
    for(i = 0; i < 10; ++i) {
        t.x = random() % gwidth;
        t.y = random() % gheight;
        g_array_append_val(units, t);
    }
}

static gboolean im_commit(GtkIMContext *imctx, gchar *str, gpointer ud)
{
    int i;
    unit_t u;
    if(units)
        for(i = 0; i < units->len; ++i) {
            u = g_array_index(units, unit_t, i);
            if (!strcmp(str, u.v)) {
                g_array_remove_index_fast(units, i);
                break;
            }
        }
    gtk_widget_queue_draw(GTK_WIDGET(ud));
}

static gboolean on_key_pressed(GtkWidget *w, GdkEventKey *ev, gpointer ud)
{
    gboolean ret = gtk_im_context_filter_keypress(GTK_IM_CONTEXT(ud), ev);
    return ret;
}

static gboolean on_focus_in(GtkWidget *w, GdkEventFocus *ev, gpointer ud)
{
    gtk_im_context_set_client_window(GTK_IM_CONTEXT(ud), w->window);
    gtk_im_context_focus_in(GTK_IM_CONTEXT(ud));
}

static gboolean on_focus_out(GtkWidget *w, GdkEventFocus *ev, gpointer ud)
{
    gtk_im_context_set_client_window(GTK_IM_CONTEXT(ud), NULL);
    gtk_im_context_focus_out(GTK_IM_CONTEXT(ud));
}

static gboolean on_timeout(gpointer ud)
{
    int i;
    unit_t u;
    if (!units)
        create_units();
    for(i = 0; i < units->len; ++i) {
        u = g_array_index(units, unit_t, i);
    }
    gtk_widget_queue_draw(GTK_WIDGET(ud));
}

int main(int argc, char *argv[])
{
    GtkWidget *frame, *da;
    GtkIMContext *imctx;

    gtk_init(&argc, &argv);

    srandom(time(NULL));

    speed = 1./30.;

    frame = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(frame), "Type War");
    g_signal_connect(G_OBJECT(frame), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    da = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(frame), da);
	gtk_widget_add_events(da, GDK_KEY_PRESS_MASK);
    g_signal_connect(G_OBJECT(da), "expose-event", G_CALLBACK(update_da), NULL);
    g_signal_connect(G_OBJECT(da), "size-allocate", G_CALLBACK(size_allocate), NULL);

    imctx = gtk_im_multicontext_new();
    gtk_im_context_set_use_preedit(imctx, FALSE);
    g_signal_connect(G_OBJECT(imctx), "commit", G_CALLBACK(im_commit), da);
	g_signal_connect(G_OBJECT(frame), "key-press-event", G_CALLBACK(on_key_pressed), imctx);
	g_signal_connect(G_OBJECT(frame), "focus-in-event", G_CALLBACK(on_focus_in), imctx);
	g_signal_connect(G_OBJECT(frame), "focus-out-event", G_CALLBACK(on_focus_out), imctx);

    gtk_widget_show_all(frame);

    gtk_main();
    return 0;
}
