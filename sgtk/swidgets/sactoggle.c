/* $Header: /fridge/cvs/xscorch/sgtk/swidgets/sactoggle.c,v 1.33 2011-08-01 00:01:42 jacob Exp $ */
/*

   xscorch - sactoggle.c      Copyright(c) 2001-2011 Jacob Luna Lundberg
                              Copyright(c) 2001-2003 Justin David Smith
   jacob(at)gnifty.net        http://www.gnifty.net/
   justins(at)chaos2.org      http://chaos2.org/~justins

   Basic toggle button widget for xscorch consoles.


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
#include <assert.h>
#include <stdio.h>

#include <sactoggle.h>

#include <gdk/gdkkeysyms.h>



static ScGadgetClass *parent_class;



static inline void _sc_ac_toggle_emit_paint(ScACToggle *toggle) {

   assert(IS_SC_AC_TOGGLE(toggle));

   g_signal_emit_by_name(G_OBJECT(toggle), "paint", NULL, NULL);

}



static void _sc_ac_toggle_draw(ScGadget *gadget) {
/* _sc_ac_toggle_draw
   Draw a toggle, depressed or not. */

   ScACToggle *toggle = SC_AC_TOGGLE(gadget);
   ScActiveConsole *cons = gadget->console;
   gboolean focus, sensitive;
   GdkColor *fgcolor;
   GdkColor interior;
   cairo_t *cr;
   GdkRectangle bounds;
   double cx, cy, r;

   /* Can we even draw yet? */
   if(sc_drawbuf_get_buffer(SC_DRAWBUF(cons)) == NULL) return;

   cr = sc_drawbuf_get_cr(SC_DRAWBUF(cons));

   /* Figure out our extents */
   sc_gadget_get_extents(gadget, &bounds);

   /* Find out if this gadget has the focus right now. */
   focus = (gtk_widget_has_focus((GtkWidget *)gadget->console) &&
            (gadget->console->current->data == gadget->spot))
           ? TRUE : FALSE;

   /* Find out if the gadget will be sensitive. */
   sensitive = gtk_widget_is_sensitive(GTK_WIDGET(gadget->console));

   /* Draw the outer circle of the toggle. */
   if(!sensitive)
      fgcolor = sc_console_get_color(SC_CONSOLE(cons), SC_CONSOLE_FOREDISABLED);
   else if(focus)
      fgcolor = sc_console_get_color(SC_CONSOLE(cons), SC_CONSOLE_FORECURSOR);
   else
      fgcolor = sc_console_get_color(SC_CONSOLE(cons), SC_CONSOLE_FOREGROUND);

   cx = bounds.x + (bounds.width - 1) / 2.0;
   cy = bounds.y + (bounds.height - 1) / 2.0;
   r  = (bounds.width - 1) / 2.0;
   cairo_set_source_rgb(cr, fgcolor->red/65535.0, fgcolor->green/65535.0, fgcolor->blue/65535.0);
   cairo_arc(cr, cx, cy, r, 0, 2 * G_PI);
   cairo_fill(cr);

   /* Draw interior lighted section (use temporary palette hack color). */
   if(!sensitive)
      gdk_color_parse("#444444", &interior);
   else if(focus)
      gdk_color_parse("#886666", &interior);
   else
      gdk_color_parse("#777777", &interior);

   cairo_set_source_rgb(cr, interior.red/65535.0, interior.green/65535.0, interior.blue/65535.0);
   cairo_arc(cr, bounds.x + bounds.width / 2.0, bounds.y + bounds.height / 2.0,
             bounds.width * 3.0 / 8.0, 0, 2 * G_PI);
   cairo_fill(cr);

   /* If the toggle is depressed, draw its little filled inner circle. */
   if(toggle->state) {
      if(!sensitive)
         fgcolor = sc_console_get_color(SC_CONSOLE(cons), SC_CONSOLE_FOREDISABLED);
      else if(focus)
         fgcolor = sc_console_get_color(SC_CONSOLE(cons), SC_CONSOLE_FORECURSOR);
      else
         fgcolor = sc_console_get_color(SC_CONSOLE(cons), SC_CONSOLE_FOREGROUND);

      cairo_set_source_rgb(cr, fgcolor->red/65535.0, fgcolor->green/65535.0, fgcolor->blue/65535.0);
      cairo_arc(cr, bounds.x + bounds.width / 2.0, bounds.y + bounds.height / 2.0,
                bounds.width / 6.0, 0, 2 * G_PI);
      cairo_fill(cr);
   }

   /* Make sure everything is queued to be drawn. */
   sc_drawbuf_queue_draw(SC_DRAWBUF(cons), bounds.x, bounds.y, bounds.width, bounds.height);

}



static gint _sc_ac_toggle_button(ScGadget *gadget, GdkEventButton *event) {
/* _sc_ac_toggle_button
   Button press events for the toggle. */

   ScACToggle *toggle = SC_AC_TOGGLE(gadget);

   /* Try out parent handler first */
   if(parent_class->button_press_spot != NULL) {
      if(parent_class->button_press_spot(gadget, event)) {
         /* Crap. The signal's already been handled */
         return(TRUE);
      } /* Signal processed? */
   } /* Signal handler available? */

   #if SC_GTK_DEBUG_GTK && __debugging_macros
      SC_DEBUG_ENTER("will process button %d press", event->button);
   #endif /* debug */

   switch(event->button) {
      case 1:  /* Left mouse */
         toggle->state = toggle->state ? FALSE : TRUE;
         _sc_ac_toggle_emit_paint(toggle);
         return(TRUE);
   }

   /* Fallthrough */
   return(FALSE);

}



static gint _sc_ac_toggle_key(ScGadget *gadget, GdkEventKey *event) {
/* _sc_ac_toggle_key
   Key press events for the toggle. */

   ScACToggle *toggle = SC_AC_TOGGLE(gadget);

   /* Try out parent handler first */
   if(parent_class->key_press_spot != NULL) {
      if(parent_class->key_press_spot(gadget, event)) {
         /* Crap. The signal's already been handled */
         return(TRUE);
      } /* Signal processed? */
   } /* Signal handler available? */

   #if SC_GTK_DEBUG_GTK && __debugging_macros
      SC_DEBUG_ENTER("will process key %d press", event->keyval);
   #endif /* debug */

   switch(event->keyval) {
      case GDK_KEY_Return:
      case GDK_KEY_KP_Enter:
      case GDK_KEY_space:
      case GDK_KEY_KP_Space:
         toggle->state = toggle->state ? FALSE : TRUE;
         _sc_ac_toggle_emit_paint(toggle);
         return(TRUE);
   }

   /* Fallthrough */
   return(FALSE);

}



static void _sc_ac_toggle_class_init(ScACToggleClass *klass) {
/* _sc_ac_toggle_class_init
   Initialize the toggle class for GTK. */

   parent_class = g_type_class_peek(sc_gadget_get_type());

   /* Setup new signal default handlers */
   SC_GADGET_CLASS(klass)->paint             = _sc_ac_toggle_draw;
   SC_GADGET_CLASS(klass)->button_press_spot = _sc_ac_toggle_button;
   SC_GADGET_CLASS(klass)->key_press_spot    = _sc_ac_toggle_key;

}



static void _sc_ac_toggle_init_obj(ScACToggle *toggle) {
/* _sc_ac_toggle_init_obj
   Initialize a toggle object to default values. */

   toggle->state = FALSE;

}



GType sc_ac_toggle_get_type(void) {
/* sc_ac_toggle_get_type
   Define the toggle's GTK type. */

   static GType sc_ac_toggle_type = 0;

   if(sc_ac_toggle_type == 0) {
      static const GTypeInfo sc_ac_toggle_info = {
         sizeof(ScACToggleClass),         /* Size of the class object */
         NULL,                            /* Base initializer */
         NULL,                            /* Base finalizer */
         (GClassInitFunc)_sc_ac_toggle_class_init,
                                          /* Class initializer */
         NULL,                            /* Class finalizer */
         NULL,                            /* Class data pointer */
         sizeof(ScACToggle),              /* Size of an instance object */
         0,                               /* Number of preallocs */
         (GInstanceInitFunc)_sc_ac_toggle_init_obj,
                                          /* Instance initializer */
         NULL                             /* Value table */
      };
      sc_ac_toggle_type = g_type_register_static(sc_gadget_get_type(), "ScACToggle",
                                                 &sc_ac_toggle_info, 0);
   }

   return(sc_ac_toggle_type);

}



ScGadget *sc_ac_toggle_new(gint x, gint y, gint width, gint height) {
/* sc_ac_toggle_new
   Instantiate a new toggle widget. */

   ScACToggle *toggle;
   ScGadget   *gadget;

   toggle = g_object_new(sc_ac_toggle_get_type(), NULL);
   g_return_val_if_fail(toggle != NULL, NULL);

   gadget = SC_GADGET(toggle);
   gadget->x = x;
   gadget->y = y;
   gadget->width  = width;
   gadget->height = height;
   toggle->state = FALSE;

   return(SC_GADGET(toggle));

}



void sc_ac_toggle_set(ScACToggle *toggle, gboolean newstate) {
/* sc_ac_toggle_set
   Set a toggle and update on screen. */

   if(toggle == NULL) return;

   if(toggle->state != newstate) {
      toggle->state = newstate;
      _sc_ac_toggle_emit_paint(toggle);
   }

}



gboolean sc_ac_toggle_get(const ScACToggle *toggle) {
/* sc_ac_toggle_get
   Read the state of a toggle. */

   if(toggle == NULL) return(FALSE);

   return(toggle->state);

}
