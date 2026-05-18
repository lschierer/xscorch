/* $Header: /fridge/cvs/xscorch/sgtk/swidgets/sdrawbuf.c,v 1.20 2011-07-31 19:48:00 jacob Exp $ */
/*

   xscorch - sdrawbuf.c       Copyright(c) 2000-2003 Justin David Smith
   justins(at)chaos2.org      http://chaos2.org/

   Drawing area with an offscreen buffer associated with it


   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2 of the License ONLY.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA

*/
#include <sdrawbuf.h>



static GtkDrawingAreaClass *parent_class;



static inline void _sc_drawbuf_unref_surface_and_cr(ScDrawbuf *draw) {

   if(sc_drawbuf_get_cr(draw) != NULL) {
      cairo_destroy(sc_drawbuf_get_cr(draw));
      sc_drawbuf_get_cr(draw) = NULL;
   }
   if(sc_drawbuf_get_buffer(draw) != NULL) {
      cairo_surface_destroy(sc_drawbuf_get_buffer(draw));
      sc_drawbuf_get_buffer(draw) = NULL;
   }

}



static void _sc_drawbuf_destroy(GtkWidget *widget) {

   ScDrawbuf *draw = SC_DRAWBUF(widget);

   _sc_drawbuf_unref_surface_and_cr(draw);

   if(GTK_WIDGET_CLASS(parent_class)->destroy != NULL) {
      GTK_WIDGET_CLASS(parent_class)->destroy(widget);
   }

}



static gint _sc_drawbuf_configure(GtkWidget *widget, GdkEventConfigure *event) {

   ScDrawbuf *draw = SC_DRAWBUF(widget);
   gint width  = gtk_widget_get_allocated_width(widget);
   gint height = gtk_widget_get_allocated_height(widget);
   gint surface_width  = -1;
   gint surface_height = -1;

   if(GTK_WIDGET_CLASS(parent_class)->configure_event != NULL) {
      if(GTK_WIDGET_CLASS(parent_class)->configure_event(widget, event)) {
         return(TRUE);
      }
   }

   if(sc_drawbuf_get_buffer(draw) != NULL) {
      surface_width  = cairo_image_surface_get_width(sc_drawbuf_get_buffer(draw));
      surface_height = cairo_image_surface_get_height(sc_drawbuf_get_buffer(draw));
   }

   if(width != surface_width || height != surface_height) {
      _sc_drawbuf_unref_surface_and_cr(draw);

      draw->screen_buffer = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
      draw->screen_cr = cairo_create(draw->screen_buffer);

      cairo_set_source_rgb(draw->screen_cr, 0, 0, 0);
      cairo_paint(draw->screen_cr);
   }

   return(FALSE);

}



static gboolean _sc_drawbuf_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {

   ScDrawbuf *draw = SC_DRAWBUF(widget);

   g_return_val_if_fail(sc_drawbuf_get_buffer(draw) != NULL, FALSE);

   cairo_set_source_surface(cr, draw->screen_buffer, 0, 0);
   cairo_paint(cr);

   return(FALSE);

}



static void _sc_drawbuf_class_init(ScDrawbufClass *klass) {

   parent_class = g_type_class_peek(gtk_drawing_area_get_type());

   GTK_WIDGET_CLASS(klass)->configure_event = _sc_drawbuf_configure;
   GTK_WIDGET_CLASS(klass)->destroy         = _sc_drawbuf_destroy;

}



static void _sc_drawbuf_init_obj(ScDrawbuf *draw) {

   draw->screen_buffer = NULL;
   draw->screen_cr = NULL;
   draw->style_configured = FALSE;

   gtk_widget_set_can_focus(GTK_WIDGET(draw), FALSE);
   gtk_widget_add_events(GTK_WIDGET(draw), GDK_EXPOSURE_MASK);
   gtk_widget_set_app_paintable(GTK_WIDGET(draw), TRUE);

   g_signal_connect(G_OBJECT(draw), "draw",
                    (GCallback)_sc_drawbuf_draw, NULL);

}



GType sc_drawbuf_get_type(void) {

   static GType sc_drawbuf_type = 0;

   if(sc_drawbuf_type == 0) {
      static const GTypeInfo sc_drawbuf_info = {
         sizeof(ScDrawbufClass),
         NULL,
         NULL,
         (GClassInitFunc)_sc_drawbuf_class_init,
         NULL,
         NULL,
         sizeof(ScDrawbuf),
         0,
         (GInstanceInitFunc)_sc_drawbuf_init_obj,
         NULL
      };
      sc_drawbuf_type = g_type_register_static(gtk_drawing_area_get_type(), "ScDrawbuf",
                                               &sc_drawbuf_info, 0);
   }

   return(sc_drawbuf_type);

}



GtkWidget *sc_drawbuf_new(gint width, gint height) {

   ScDrawbuf *draw;

   draw = g_object_new(sc_drawbuf_get_type(), NULL);
   g_return_val_if_fail(draw != NULL, NULL);

   gtk_widget_set_size_request(GTK_WIDGET(draw), width, height);

   return(GTK_WIDGET(draw));

}



void sc_drawbuf_queue_draw(ScDrawbuf *draw, gint x, gint y, gint width, gint height) {

   if(x < 0) {
      width += x;
      x = 0;
   }
   if(y < 0) {
      height += y;
      y = 0;
   }
   gtk_widget_queue_draw_area((GtkWidget *)draw, x, y, width + 1, height + 1);

}
