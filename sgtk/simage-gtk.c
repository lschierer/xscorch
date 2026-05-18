/* $Header: /fridge/cvs/xscorch/sgtk/simage-gtk.c,v 1.7 2009-04-26 17:39:48 jacob Exp $ */
/*

   xscorch - simage-gtk.c     Copyright(c) 2000-2003 Justin David Smith
   justins(at)chaos2.org      http://chaos2.org/

   GTK interface to image drawing


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
#include <sgtk.h>
#include <simage-gtk.h>



gint sc_pixmap_width_gtk(cairo_surface_t *surf) {

   if(surf == NULL) return(0);
   return(cairo_image_surface_get_width(surf));

}



gint sc_pixmap_height_gtk(cairo_surface_t *surf) {

   if(surf == NULL) return(0);
   return(cairo_image_surface_get_height(surf));

}



void sc_pixmap_copy_gtk(cairo_surface_t *dest, cairo_t *cr, cairo_surface_t *src, int dx, int dy) {

   /* If cr is the context for dest, just paint src at (dx, dy).
      The source surface may have an alpha channel (logo transparency). */
   (void)dest;  /* cr is already a context for dest */
   cairo_save(cr);
   cairo_set_source_surface(cr, src, dx, dy);
   cairo_paint(cr);
   cairo_restore(cr);

}
