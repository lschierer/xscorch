/* $Header: /fridge/cvs/xscorch/sgtk/sexplosion-gtk.c,v 1.14 2009-04-26 17:39:48 jacob Exp $ */
/*

   xscorch - sexplosion-gtk.c Copyright(c) 2000-2003 Justin David Smith
   justins(at)chaos2.org      http://chaos2.org/

   Window painting code, specific to the explosion cache
   I can't believe I wrote this mess...


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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <cairo/cairo.h>

#include <sgtk.h>
#include <sdisplay.h>

#include <scolor-gtk.h>
#include <sexplosion-gtk.h>
#include <smenu-gtk.h>
#include <swindow-gtk.h>

#include <sgame/sland.h>
#include <sgame/splayer.h>
#include <sgame/sweapon.h>
#include <sutil/sfractal.h>



/*

   Congratulations!

   You are about to be treated to some pretty repulsive-looking code!
   You have to understand, the comments are quite intentionally misleading
   All of them are.

   You really are better off reading this file, after writing a mangler
   that strips out all the comments (better keep that copyright, `tho).
   Then you can attain "understanding"
   The comments will only hamper.

   Comprehension is insanity.
   First, question all boundaries.
   This code defies the laws of gravity.
   Keep your hands/feet/etc inside the vehicle at all times.
   Reading this code can be detrimintal to your health.
   Abandon all hope, ye who enter here.
   Batteries not included.
   Good luck.

*/



sc_expl_cache_gtk *sc_expl_cache_new_gtk(void) {
/* sc_expl_cache_new_gtk
   Creates a new explosion cache.  */

   sc_expl_cache_gtk *cache;  /* Newly allocated cache */

   /* Create the cache */
   cache = (sc_expl_cache_gtk *)malloc(sizeof(sc_expl_cache_gtk));
   if(cache == NULL) return(NULL);

   /* Initialise the cache */
   cache->cachesize = 0;
   cache->headptr = 0;

   /* Return the cache */
   return(cache);

}



void sc_expl_cache_free_gtk(sc_expl_cache_gtk **cache) {
/* sc_expl_cache_free_gtk
   Release the cache, and associated data.  */

   int i;            /* Iterator variable */

   /* Sanity check. */
   if(cache == NULL || *cache == NULL) return;

   /* Release surfaces in cache */
   for(i = 0; i < (*cache)->cachesize; ++i) {
      cairo_surface_destroy((*cache)->cache[(*cache)->headptr].surface);
      ++(*cache)->headptr;
      if((*cache)->headptr >= SC_EXPL_CACHE_SIZE) {
         (*cache)->headptr = 0;
      }
   } /* Releasing all surfaces... */

   /* Release the cache memory */
   free(*cache);
   *cache = NULL;

}



static inline void _sc_expl_cache_draw_points_gtk(unsigned char *imgdata, int stride,
                                                  guint32 pixel,
                                                  int dx, int dy, int radius) {
/* sc_expl_cache_draw_points_gtk
   Draw the points (cx +/- dx, cy +/- dy), using the specified pixel value.  */

   guint32 *row;

   /* Iterate through the four quadrants to update display */
   row = (guint32 *)(imgdata + (radius - dy) * stride);
   row[radius - dx] = pixel;
   row[radius + dx] = pixel;

   row = (guint32 *)(imgdata + (radius + dy) * stride);
   row[radius - dx] = pixel;
   row[radius + dx] = pixel;

}



static inline void _sc_expl_cache_annihilate_column_rad2_gtk(unsigned char *imgdata, int stride,
                                                             guint32 *gradient,
                                                             int gsize, int dx, int dy,
                                                             int radius, int rad2) {
/* sc_expl_cache_annihilate_column_rad2_gtk
   Annihilate a column of an explosion.  (cx,cy) is the center coordinate of
   the explosion.  (dx,dy) is the deltax and deltay that this column begins
   at -- that is, the column goes from (cx+dx,cy-dy) to (cx+dx,cy+dy).  rad2
   is the radius, squared, of the entire explosion.  Note, colors fall off
   with the square of the radius; this makes for more "3-D"istic explosions,
   and also happens to be much faster to calculate.  */

   int dx2y2;        /* dx^2 * y^2, for current value of y */
   int tile;         /* Tile value for explosion */
   int y;            /* Current y (offset from cy, y in [0,dy]) */

   /* Iteration for all four columns */
   y = 0;            /* We are starting on the Y axis */
   dx2y2 = dx * dx;  /* This starts off as x^2, since y=0 */
   do {
      /* Calculate tile code -- any way to optimize this? */
      tile = dx2y2 * gsize / rad2;

      /* Adjust dx2y2 for next value of y */
      /* x^2 + y^2 => x^2 + y^2 + y + (y+1)
                    = x^2 + y(y+1) + (y+1)
                    = x^2 + (y+1)^2          */
      /* Suggested by JJP */
      dx2y2 += (y << 1) | 1;

      /* Iterate through the four quadrants to update display */
      _sc_expl_cache_draw_points_gtk(imgdata, stride, gradient[tile], dx, y, radius);

      /* Next value of y (one step farther from central axis) */
      ++y;
   } while(y <= dy);

}



static inline void _sc_expl_cache_annihilate_column_rad_gtk(unsigned char *imgdata, int stride,
                                                            guint32 *gradient,
                                                            int gsize, int dx, int dy, int radius) {
/* sc_expl_cache_annihilate_column_rad_gtk
   Annihilate a column of an explosion.  (cx,cy) is the center coordinate of
   the explosion.  (dx,dy) is the deltax and deltay that this column begins
   at -- that is, the column goes from (cx+dx,cy-dy) to (cx+dx,cy+dy).  rad2
   is the radius, squared, of the entire explosion.  Note, colors fall off
   linearly with radius, which is used by funky bombs.  This is slower to
   calculate, I would not recommend using this routine with very large
   explosions. */

   /*
      Life... Dreams... Hope...
      Where'd they come from...
      And where are they headed?
      These things... I am going to destroy!
         -- Kefka, Fourth Tier, Final Fantasy VI
    */

   int dx2y2;        /* dx^2 * y^2, for current value of y */
   int tile;         /* Tile value for explosion */
   int y;            /* Current y (offset from cy, y in [0,dy]) */

   /* Iteration for all four columns */
   y = 0;            /* We are starting on the Y axis */
   dx2y2 = dx * dx;  /* This starts off as x^2, since y=0 */
   do {
      /* Calculate tile code -- any way to optimize this? */
      tile = (int)(sqrt(dx2y2) * gsize / radius);

      /* Adjust dx2y2 for next value of y */
      /* x^2 + y^2 => x^2 + y^2 + y + (y+1)
                    = x^2 + y(y+1) + (y+1)
                    = x^2 + (y+1)^2          */
      /* Suggested by JJP */
      dx2y2 += (y << 1) | 1;

      /* Iterate through the four quadrants to update display */
      _sc_expl_cache_draw_points_gtk(imgdata, stride, gradient[tile], dx, y, radius);

      /* Next value of y (one step farther from central axis) */
      ++y;
   } while(y <= dy);

}



static inline void _sc_expl_cache_annihilate_column_plasmoid_gtk(unsigned char *imgdata, int stride,
                                                                 guint32 *gradient,
                                                                 int gsize, int dx, int dy, int radius,
                                                                 unsigned char *fractal, int fsize) {
/* sc_expl_cache_annihilate_column_plasmoid_gtk
   Annihilate a column of an explosion.  (cx,cy) is the center coordinate of
   the explosion.  (dx,dy) is the deltax and deltay that this column begins
   at -- that is, the column goes from (cx+dx,cy-dy) to (cx+dx,cy+dy).  rad2
   is the radius, squared, of the entire explosion.  Note, colors are selected
   as a plasmoid, whatever that is.  */

   int dx2y2;        /* dx^2 * y^2, for current value of y */
   int tile;         /* Tile value for explosion */
   int y;            /* Current y (offset from cy, y in [0,dy]) */
   int radminusdx;   /* Value of radius - dx */
   int radplusdx;    /* Value of radius + dx */
   guint32 *row;     /* Pointer to pixel row */

   /* Precompute as much as possible. */
   radminusdx = radius - dx;
   radplusdx  = radius + dx;

   /* Iteration for all four columns */
   y = 0;            /* We are starting on the Y axis */
   dx2y2 = dx * dx;  /* This starts off as x^2, since y=0 */
   do {
      /* Warning: the division must remain inlined with the rest of the
         computation; it cannot be lifted out since this is integer-level
         arithmetic. */

      /* Calculate tile code (quadrant 1) */
      tile = *(fractal + (radius + y) * fsize + radplusdx)  * gsize / 0x100;
      row = (guint32 *)(imgdata + (radius + y) * stride);
      row[radplusdx] = gradient[tile];

      /* Calculate tile code (quadrant 2) */
      tile = *(fractal + (radius + y) * fsize + radminusdx) * gsize / 0x100;
      row[radminusdx] = gradient[tile];

      /* Calculate tile code (quadrant 3) */
      tile = *(fractal + (radius - y) * fsize + radminusdx) * gsize / 0x100;
      row = (guint32 *)(imgdata + (radius - y) * stride);
      row[radminusdx] = gradient[tile];

      /* Calculate tile code (quadrant 4) */
      tile = *(fractal + (radius - y) * fsize + radplusdx)  * gsize / 0x100;
      row[radplusdx] = gradient[tile];

      /* Adjust dx2y2 for next value of y */
      /* Suggested by JJP */
      dx2y2 += (y << 1) | 1;

      /* Next value of y (one step farther from central axis) */
      ++y;
   } while(y <= dy);

}



static void _sc_expl_cache_annihilate_rad2_gtk(unsigned char *imgdata, int stride,
                                               guint32 *gradient,
                                               int gsize, int radius) {
/* sc_expl_annihilate_rad2_gtk
   Annihilate a region centered at (cx,cy) for a radius r.  This function
   lets colors fall off with the square of the radius.  */

   int dx;           /* Delta X (distance away from cx) - iterator variable */
   int dy;           /* Delta Y (distance away from cy) for _edge_ of circle */
   int rad2;         /* Radius squared */
   int rad2major2;   /* Radius^2 + the major_distance^2 */
   int min2thresh;   /* Minimum threshold to avoid redrawing columns where dx>dy */

   /* DX = major axis, DY = minor axis */
   dx = 0;           /* DX starts at zero (iterator) */
   dy = radius;      /* DY is one radius away (edge of circle at cx+dx) */
   rad2 = radius * radius; /* Calculate Radius Squared */
   rad2major2 = rad2;/* Radius^2 + major^2, running total */
   min2thresh = rad2 - dy; /* Minimum threshold before need to redraw edges */

   /* TEMP HACK */
   /* This is a hack to prevent the orange highlights on explosions. */
   /* THIS IS A HACK -- FIXME FOR REAL. */
   rad2 += radius + radius + 1;

   /* Should know that, we are incrementing DX every time.  However,
      if we call the transpose method every time as well, then we will
      be filling parts of the circle multiple times.  Hence the
      min2thresh variable. */
   do {
      _sc_expl_cache_annihilate_column_rad2_gtk(imgdata, stride, gradient, gsize,
                                                dx, dy, radius, rad2);
      ++dx;
      rad2major2 -= (dx << 1) - 1;
      if(rad2major2 <= min2thresh) {
         _sc_expl_cache_annihilate_column_rad2_gtk(imgdata, stride, gradient, gsize,
                                                   dy, dx, radius, rad2);
         --dy;
         min2thresh -= (dy << 1);
      }
   } while(dx <= dy);

}



static void _sc_expl_cache_annihilate_rad_gtk(unsigned char *imgdata, int stride,
                                              guint32 *gradient,
                                              int gsize, int radius) {
/* sc_expl_annihilate_rad_gtk
   Annihilate a region centered at (cx,cy) for a radius r.  This function
   lets colors fall off with the square of the radius.  */

   int dx;           /* Delta X (distance away from cx) - iterator variable */
   int dy;           /* Delta Y (distance away from cy) for _edge_ of circle */
   int rad2;         /* Radius squared */
   int rad2major2;   /* Radius^2 + the major_distance^2 */
   int min2thresh;   /* Minimum threshold to avoid redrawing columns where dx>dy */

   /* DX = major axis, DY = minor axis */
   dx = 0;           /* DX starts at zero (iterator) */
   dy = radius;      /* DY is one radius away (edge of circle at cx+dx) */
   rad2 = radius * radius; /* Calculate Radius Squared */
   rad2major2 = rad2;/* Radius^2 + major^2, running total */
   min2thresh = rad2 - dy; /* Minimum threshold before need to redraw edges */

   /* Should know that, we are incrementing DX every time.  However,
      if we call the transpose method every time as well, then we will
      be filling parts of the circle multiple times.  Hence the
      min2thresh variable. */
   do {
      _sc_expl_cache_annihilate_column_rad_gtk(imgdata, stride, gradient, gsize,
                                               dx, dy, radius);
      ++dx;
      rad2major2 -= (dx << 1) - 1;
      if(rad2major2 <= min2thresh) {
         _sc_expl_cache_annihilate_column_rad_gtk(imgdata, stride, gradient, gsize,
                                                  dy, dx, radius);
         --dy;
         min2thresh -= (dy << 1);
      }
   } while(dx <= dy);

}



static void _sc_expl_cache_annihilate_plasmoid_gtk(unsigned char *imgdata, int stride,
                                                   guint32 *gradient,
                                                   int gsize, int radius) {
/* sc_expl_annihilate_plasmoid_gtk
   Annihilate a region centered at (cx,cy) for a radius r.  This function
   selects colors to form a plasma manifold pattern, whatever that means.  */

   int dx;           /* Delta X (distance away from cx) - iterator variable */
   int dy;           /* Delta Y (distance away from cy) for _edge_ of circle */
   int rad2;         /* Radius squared */
   int rad2major2;   /* Radius^2 + the major_distance^2 */
   int min2thresh;   /* Minimum threshold to avoid redrawing columns where dx>dy */
   unsigned char *fractal;     /* */
   int fsize;

   /* Allocate a new fractal. */
   fractal = sc_fractal_create(&fsize, radius + radius + 1);
   if(fractal == NULL) {
      _sc_expl_cache_annihilate_rad2_gtk(imgdata, stride, gradient, gsize, radius);
      return;
   }

   /* DX = major axis, DY = minor axis */
   dx = 0;           /* DX starts at zero (iterator) */
   dy = radius;      /* DY is one radius away (edge of circle at cx+dx) */
   rad2 = radius * radius; /* Calculate Radius Squared */
   rad2major2 = rad2;/* Radius^2 + major^2, running total */
   min2thresh = rad2 - dy; /* Minimum threshold before need to redraw edges */

   /* Should know that, we are incrementing DX every time.  However,
      if we call the transpose method every time as well, then we will
      be filling parts of the circle multiple times.  Hence the
      min2thresh variable. */
   do {
      _sc_expl_cache_annihilate_column_plasmoid_gtk(imgdata, stride, gradient, gsize,
                                                    dx, dy, radius, fractal, fsize);
      ++dx;
      rad2major2 -= (dx << 1) - 1;
      if(rad2major2 <= min2thresh) {
         _sc_expl_cache_annihilate_column_plasmoid_gtk(imgdata, stride, gradient, gsize,
                                                       dy, dx, radius, fractal, fsize);
         --dy;
         min2thresh -= (dy << 1);
      }
   } while(dx <= dy);

   free(fractal);

}



static inline void _sc_expl_cache_annihilate_happy_gtk(sc_window_gtk *w, cairo_surface_t *surface,
                                                       int radius) {
/* sc_expl_cache_annihilate_happy_gtk
   It's not a bug... it's a ``feature''...  I wonder if anyone has actually
   found this egg in practice.  It's quite fun once it's activated :)  */

   int size = radius + radius + 1;
   int eyesize = (radius >> 2) + (radius >> 4);
   int eyecoff = radius >> 2;
   double cx = size / 2.0;
   double cy = size / 2.0;
   cairo_t *cr = cairo_create(surface);

   /* Fill yellow circle */
   cairo_set_source_rgb(cr,
                        w->colormap->yellow.red   / 65535.0,
                        w->colormap->yellow.green / 65535.0,
                        w->colormap->yellow.blue  / 65535.0);
   cairo_arc(cr, cx, cy, size / 2.0, 0, 2 * G_PI);
   cairo_fill(cr);

   /* Black outline circle */
   cairo_set_source_rgb(cr,
                        w->colormap->black.red   / 65535.0,
                        w->colormap->black.green / 65535.0,
                        w->colormap->black.blue  / 65535.0);
   cairo_set_line_width(cr, 3.0);
   cairo_arc(cr, cx, cy, size / 2.0, 0, 2 * G_PI);
   cairo_stroke(cr);

   /* Left eye (filled circle) */
   {
      double ex = (radius - eyecoff - eyesize) + eyesize / 2.0;
      double ey = (radius - eyecoff - eyesize) + eyesize / 2.0;
      cairo_arc(cr, ex, ey, eyesize / 2.0, 0, 2 * G_PI);
      cairo_fill(cr);
   }

   /* Right eye (filled circle) */
   {
      double ex = (radius + eyecoff) + eyesize / 2.0;
      double ey = (radius - eyecoff - eyesize) + eyesize / 2.0;
      cairo_arc(cr, ex, ey, eyesize / 2.0, 0, 2 * G_PI);
      cairo_fill(cr);
   }

   /* Smile arc: GDK arc from 200*64 to (200+140)*64 = 340*64 (1/64 degree units,
      CCW from 3 o'clock in GDK).  In Cairo (CW from 3 o'clock, radians):
      start = 200 * M_PI / 180, end = 340 * M_PI / 180  */
   {
      /* GDK bounding box: x = radius-eyecoff-eyesize, y = radius+(eyecoff>>1),
         w = (eyecoff+eyesize)<<1, h = eyesize<<1
         center = (x + w/2, y + h/2), r = w/2  */
      double smcx = (radius - eyecoff - eyesize) + (eyecoff + eyesize);
      double smcy = (radius + (eyecoff >> 1)) + eyesize;
      double smr  = (eyecoff + eyesize);
      cairo_arc(cr, smcx, smcy, smr,
                200.0 * M_PI / 180.0,
                340.0 * M_PI / 180.0);
      cairo_stroke(cr);
   }

   cairo_destroy(cr);

}



static inline void _sc_expl_cache_annihilate_gtk(sc_window_gtk *w, cairo_surface_t *surface,
                                                 int radius, sc_explosion_type type) {
/* sc_expl_annihilate_rad_gtk
   Annihilate a region centered at (cx,cy) for a radius r.  This function
   uses the type parameter to dispatch to the appropriate drawing
   subfunction.  */

   cairo_surface_t *image_surf;
   unsigned char *imgdata;
   int stride;
   int size;
   int gsize;
   guint32 *precomputed;
   int i;

   /* ...wonder why this is here... :) */
   if(w->state < 4) {
      /* Create image surface */
      size = radius + radius + 1;
      image_surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size, size);
      if(image_surf == NULL) return;
      if(cairo_surface_status(image_surf) != CAIRO_STATUS_SUCCESS) {
         cairo_surface_destroy(image_surf);
         return;
      }

      /* Flush and get raw pixel data */
      cairo_surface_flush(image_surf);
      imgdata = cairo_image_surface_get_data(image_surf);
      stride  = cairo_image_surface_get_stride(image_surf);

      /* Draw the appropriate type of explosion. */
      switch(type) {
         case SC_EXPLOSION_NORMAL:
            gsize = w->c->colors->gradsize[SC_GRAD_EXPLOSION];
            precomputed = (guint32 *)malloc(gsize * sizeof(guint32));
            if(precomputed != NULL) {
               for(i = 0; i < gsize; i++) {
                  GdkColor *c = &w->colormap->gradient[SC_GRAD_EXPLOSION][i];
                  precomputed[i] = 0xFF000000u
                                 | ((guint32)(c->red   >> 8) << 16)
                                 | ((guint32)(c->green >> 8) <<  8)
                                 |  (guint32)(c->blue  >> 8);
               }
               _sc_expl_cache_annihilate_rad2_gtk(imgdata, stride, precomputed, gsize, radius);
               free(precomputed);
            }
            break;
         case SC_EXPLOSION_PLASMA:
            gsize = w->c->colors->gradsize[SC_GRAD_EXPLOSION];
            precomputed = (guint32 *)malloc(gsize * sizeof(guint32));
            if(precomputed != NULL) {
               for(i = 0; i < gsize; i++) {
                  GdkColor *c = &w->colormap->gradient[SC_GRAD_EXPLOSION][i];
                  precomputed[i] = 0xFF000000u
                                 | ((guint32)(c->red   >> 8) << 16)
                                 | ((guint32)(c->green >> 8) <<  8)
                                 |  (guint32)(c->blue  >> 8);
               }
               _sc_expl_cache_annihilate_plasmoid_gtk(imgdata, stride, precomputed, gsize, radius);
               free(precomputed);
            }
            break;
         case SC_EXPLOSION_SPIDER:
            gsize = w->c->colors->gradsize[SC_GRAD_FUNKY_EXPLOSION];
            precomputed = (guint32 *)malloc(gsize * sizeof(guint32));
            if(precomputed != NULL) {
               for(i = 0; i < gsize; i++) {
                  GdkColor *c = &w->colormap->gradient[SC_GRAD_FUNKY_EXPLOSION][i];
                  precomputed[i] = 0xFF000000u
                                 | ((guint32)(c->red   >> 8) << 16)
                                 | ((guint32)(c->green >> 8) <<  8)
                                 |  (guint32)(c->blue  >> 8);
               }
               _sc_expl_cache_annihilate_rad_gtk(imgdata, stride, precomputed, gsize, radius);
               free(precomputed);
            }
            break;
         default:
            /* do nothing */;
      } /* Which type of explosion to draw? */

      /* Mark dirty and commit image surface to the cache surface */
      cairo_surface_mark_dirty(image_surf);
      {
         cairo_t *surf_cr = cairo_create(surface);
         cairo_set_source_surface(surf_cr, image_surf, 0, 0);
         cairo_paint(surf_cr);
         cairo_destroy(surf_cr);
      }

      /* Destroy the local image surface */
      cairo_surface_destroy(image_surf);
   } else {
      _sc_expl_cache_annihilate_happy_gtk(w, surface, radius);
   }

}



static int _sc_expl_cache_lookup_gtk(sc_window_gtk *w, int radius, sc_explosion_type type) {
/* sc_expl_cache_lookup_gtk
   Lookup an entry in the cache matching this description.
   If no such entry is found, -1 is returned.  */

   sc_expl_cache_gtk *cache = w->explcache;  /* Cache */
   int size;         /* Size of cache */
   int ptr;          /* Current ptr into cache */

   /* Determine size and starting pointer */
   size = cache->cachesize;
   ptr = cache->headptr;

   /* Try to find an explosion matching these characteristics */
   while(size > 0) {
      /* Does this explosion match? */
      if(cache->cache[ptr].radius == radius && cache->cache[ptr].type == type) {
         /* Match found; return its cacheid */
         return(ptr);
      } /* Found a match? */

      /* Not a match; advance to next entry in the cache. */
      ++ptr;
      if(ptr >= SC_EXPL_CACHE_SIZE) ptr = 0;
      --size;
   } /* Searching for a match... */

   /* No matching explosion was found */
   return(-1);

}



int sc_expl_cache_new(sc_window *w_, int radius, sc_explosion_type type) {
/* sc_expl_cache_new
   Creates a new explosion with these characteristics, and returns its cache
   ID.  Note, if an explosion of these characteristics already exists in the
   cache, then its cache ID will be returned, and no new explosion will be
   created.  */

   sc_window_gtk *w = (sc_window_gtk *)w_;   /* Window structure */
   sc_expl_cache_gtk *cache =  w->explcache; /* Cache structure */
   sc_expl_cache_entry_gtk *centry; /* An entry in the cache. */
   int cacheid;      /* Cache ID of a matching explosion */
   int size;         /* Size (wid/hei) of the surface to create. */

   /* Try to find this explosion in the cache. */
   cacheid = _sc_expl_cache_lookup_gtk(w, radius, type);
   if(cacheid < 0) {
      /* No matching explosion found; overwrite the oldest
         entry in the cache with our new explosion.  */
      /* TEMP BUG:  We really shouldn't overwrite the OLDEST
         entry; we'd get better performance if we overwrite
         the LEAST-RECENTLY-USED entry.  Need to fix this... */
      --cache->headptr;
      if(cache->headptr < 0) cache->headptr = SC_EXPL_CACHE_SIZE - 1;
      cacheid = cache->headptr;

      /* Get the entry for the explosion we are overwriting */
      centry = &cache->cache[cacheid];
      if(cache->cachesize < SC_EXPL_CACHE_SIZE) {
         /* Cache wasn't full; this entry is vacant. */
         ++cache->cachesize;
      } else {
         /* Cache was full; need to release the old surface */
         cairo_surface_destroy(centry->surface);
      } /* Was cache already full? */

      /* Create the new surface and set characteristics */
      size = radius + radius + 1;
      centry->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size, size);
      centry->radius = radius;
      centry->type   = type;

      /* Draw an explosion into the new surface. */
      _sc_expl_cache_annihilate_gtk(w, centry->surface, radius, type);
   } /* Did explosion already exist in cache? */

   /* Return the cache ID of our new explosion */
   return(cacheid);

}



void sc_expl_cache_draw(sc_window *w_, int ptr, int centerx, int centery, int rad) {
/* sc_expl_cache_draw
   Draws the explosion to the screen buffer.  */

   sc_window_gtk *w = (sc_window_gtk *)w_;/* Window structure */
   sc_expl_cache_entry_gtk *centry = &(w->explcache->cache[ptr]);
   cairo_t *cr;                           /* Cairo context for drawing */
   int radius;                            /* Radius of explosion */
   int size;                              /* Size (wid/hei) of expl */

   /* Calculate the screen center coordinates */
   radius = centry->radius;
   centerx = centerx;
   centery = w->c->fieldheight - centery - 1;
   if(rad > radius) rad = radius;
   size = rad + rad + 1;

   /* Get the Cairo context for the screen buffer */
   cr = sc_display_get_cr(SC_DISPLAY(w->screen));

   /* Clip to a circle, then draw the explosion surface */
   cairo_save(cr);
   cairo_arc(cr, centerx, centery, rad + 0.5, 0, 2 * G_PI);
   cairo_clip(cr);

   /* Draw the cached explosion surface, offset so its center aligns */
   cairo_set_source_surface(cr, centry->surface,
                            centerx - radius,
                            centery - radius);
   cairo_paint(cr);

   cairo_restore(cr);

   /* Update drawing */
   sc_display_queue_draw(SC_DISPLAY(w->screen),
                         centerx - rad, centery - rad,
                         size, size);

}
