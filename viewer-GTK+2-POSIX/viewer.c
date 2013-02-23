/* -*- mode: c; indent-tabs-mode: nil; c-indent-level: 4; -*-

 * $Id: viewer.c,v 1.18 2007/08/28 13:46:49 desrod Exp $
 *
 * Viewer - a part of Plucker, the open-source, open-format ebook system
 * Copyright (c) 2002, Bill Janssen
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixdata.h>
#include <stdlib.h>             /* for exit() */
#include <stdio.h>
#include <string.h>
#include <unpluck.h>
#include <sys/stat.h>
#include <time.h>

#include "gtkviewer.h"

#define READ_BIGENDIAN_SHORT(p) (((p)[0] << 8)|((p)[1]))
#define READ_BIGENDIAN_LONG(p)  (((p)[0] << 24)|((p)[1] << 16)|((p)[2] << 8)|((p)[3]))

#define UTF8_CHARSET_MIBENUM    106

#define MAX_RECORD_SIZE         0x10000

#define NEWLINE                 '\n'
#define UNICODE_LINE_SEPARATOR          0x2028
#define UNICODE_PARAGRAPH_SEPARATOR     0x2029

#define UNICODE_LS_STRING       "\xe2\x80\xa8"
#define UNICODE_PS_STRING       "\xe2\x80\xa9"

#define GET_FUNCTION_CODE_TYPE(x)               (((x)>>3) & 0x1F)
#define GET_FUNCTION_CODE_DATALEN(x)            ((x) & 0x7)

#define CELLS(row,col) cells[row*cols+col]

extern GdkPixbuf *GetImageRecord
    (
    plkr_Document * doc,
    int id,
    double scale
    );

extern GdkPixbuf *GetGlyph
    (
    plkr_Document * doc,
    int record_id,
    int glyph_index,
    int desired_dpi
    );
GdkPixbuf *GetSpacerPixbuf
    (
    int width_in_pixels
    );

static GMainLoop *TheMainloop = NULL;

static GtkTextTagTable *TheTagsTable = NULL;

static GTree *CharsetNames = NULL;

static double TheScale;
static int ScreenDPI = 0;

static gboolean UseUnicode = FALSE;
static gboolean UnicodeNewlineSeparatorBroken = TRUE;

static gboolean Verbose = FALSE;

static GtkWidget *TheLibrary = NULL;
static gboolean LibraryFirstSelection = TRUE;

static GtkWidget *RenderTable(plkr_Document * doc, gint id);

extern void initialize_icons (void);
extern void initialize_library (void);
extern void enumerate_library
    (
    GTraverseFunc user_func,
    gpointer user_data
    );

typedef enum {
    UrlLink,
    MailtoLink,
    RecordLink,
    RecordParaLink
} LinkType;

typedef struct {
    LinkType type;
    gint start;
    gint end;
    gint record;
    gint para;
    gint image;
    gchar *url;
} Link;

typedef struct Page_s {
    int index;
    int page_id;
    plkr_Document *doc;
    GtkTextBuffer *formatted_view;

    /* The links tree is keyed by start-end ranges, and the data is Link structs */
    GTree *links;

    /* The paragraphs tree maps keys (para_no << 16 + record_no) to offsets */
    GTree *paragraphs;

    /* a list of all the hrules which need to be inserted for this buffer */
    GSList *hrules;

    /* a list of all the tables which need to be inserted for this buffer */
    GSList *tables;

    Link *current_link;

} Page;

typedef struct {
    Page *page;
    int offset;
} HistoryNode;

#define GLIST_TO_HISTORY_NODE(x)        ((HistoryNode *)((x)->data))

typedef struct {
    plkr_Document *doc;
    GtkTextView *textview;
    GtkWindow *window;          /* top-level window */
    GTree *pages;
    GList *history;
    GList *current_node;
    double scale;               /* scaling factor for images */
} UI;

typedef struct {
    GtkTextChildAnchor *anchor;
    gint table;
} TABLE;

typedef struct {
    int align;
    int colspan;
    int rowspan;
    int len;
    int link;
    GtkWidget *subTable;
    GdkPixbuf *image;
    char *text;
} Cell;

static char *link_type_names[] = { "url-link",
    "mailto-link",
    "record-link",
    "record-para-link"
};

/* GdkPixbuf RGBA C-Source image dump 1-byte-run-length-encoded */

static const GdkPixdata table_icon_pixdata = {
    0x47646b50,                 /* Pixbuf magic: 'GdkP' */
    24 + 651,                   /* header length + pixel_data length */
    0x2010002,                  /* pixdata_type */
    120,                        /* rowstride */
    30,                         /* width */
    20,                         /* height */
    /* pixel_data: */
    "\237\0\0\0\377\7\377\377\377\377\0\0\0\377\377\377\377\377\0\0\0\377"
    "\377\377\377\377\0\0\0\377\377\377\377\377\203\0\0\0\377\7\377\377\377"
    "\377\0\0\0\377\377\377\377\377\0\0\0\377\377\377\377\377\0\0\0\377\377"
    "\377\377\377\203\0\0\0\377\7\377\377\377\377\0\0\0\377\377\377\377\377"
    "\0\0\0\377\377\377\377\377\0\0\0\377\377\377\377\377\204\0\0\0\377\207"
    "\377\377\377\377\202\0\0\0\377\210\377\377\377\377\202\0\0\0\377\210"
    "\377\377\377\377\202\0\0\0\377\210\377\377\377\377\1\0\0\0\377\211\377"
    "\377\377\377\1\0\0\0\377\211\377\377\377\377\203\0\0\0\377\207\377\377"
    "\377\377\202\0\0\0\377\210\377\377\377\377\202\0\0\0\377\210\377\377"
    "\377\377\202\0\0\0\377\210\377\377\377\377\1\0\0\0\377\211\377\377\377"
    "\377\1\0\0\0\377\211\377\377\377\377\203\0\0\0\377\207\377\377\377\377"
    "\202\0\0\0\377\210\377\377\377\377\202\0\0\0\377\210\377\377\377\377"
    "\202\0\0\0\377\210\377\377\377\377\1\0\0\0\377\211\377\377\377\377\1"
    "\0\0\0\377\211\377\377\377\377\203\0\0\0\377\207\377\377\377\377\202"
    "\0\0\0\377\210\377\377\377\377\202\0\0\0\377\210\377\377\377\377\242"
    "\0\0\0\377\5\377\377\377\377\0\0\0\377\377\377\377\377\0\0\0\377\377"
    "\377\377\377\203\0\0\0\377\7\377\377\377\377\0\0\0\377\377\377\377\377"
    "\0\0\0\377\377\377\377\377\0\0\0\377\377\377\377\377\203\0\0\0\377\7"
    "\377\377\377\377\0\0\0\377\377\377\377\377\0\0\0\377\377\377\377\377"
    "\0\0\0\377\377\377\377\377\203\0\0\0\377\210\377\377\377\377\1\0\0\0"
    "\377\211\377\377\377\377\1\0\0\0\377\211\377\377\377\377\203\0\0\0\377"
    "\207\377\377\377\377\202\0\0\0\377\210\377\377\377\377\202\0\0\0\377"
    "\210\377\377\377\377\202\0\0\0\377\210\377\377\377\377\1\0\0\0\377\211"
    "\377\377\377\377\1\0\0\0\377\211\377\377\377\377\203\0\0\0\377\207\377"
    "\377\377\377\202\0\0\0\377\210\377\377\377\377\202\0\0\0\377\210\377"
    "\377\377\377\202\0\0\0\377\210\377\377\377\377\1\0\0\0\377\211\377\377"
    "\377\377\1\0\0\0\377\211\377\377\377\377\203\0\0\0\377\207\377\377\377"
    "\377\202\0\0\0\377\210\377\377\377\377\202\0\0\0\377\210\377\377\377"
    "\377\202\0\0\0\377\210\377\377\377\377\1\0\0\0\377\211\377\377\377\377"
    "\1\0\0\0\377\211\377\377\377\377\203\0\0\0\377\207\377\377\377\377\202"
    "\0\0\0\377\210\377\377\377\377\202\0\0\0\377\210\377\377\377\377\237"
    "\0\0\0\377",
};


static char *FixText
    (
    char *text,
    int charset
    )
{
    gchar *newchars;
    gsize newcharslen, bytesread;
    char *charset_name;

    if (charset == UTF8_CHARSET_MIBENUM)
        return text;

    if (charset == 0)
        charset_name = "ISO-8859-1";
    else
        charset_name =
            g_tree_lookup (CharsetNames, GINT_TO_POINTER (charset));
    if (charset_name == NULL) {
        fprintf (stderr,
                 "Unknown charset %d encountered.  Skipping text.\n",
                 charset);
    }
    else {
        newchars = g_convert_with_fallback (text, strlen (text), "UTF-8",
                                     charset_name, NULL, &bytesread,
                                     &newcharslen, NULL);
        if (newchars)
            return newchars;
    }

    return NULL;
}


static void BgColor
    (
    GtkWidget * widget,
    int color_num
    )
{
    GdkColor color;
    GtkStyle *style;
    char format[10];

    sprintf (format, "#%06X", color_num);
    gdk_color_parse (format, &color);
    style = gtk_style_copy (gtk_widget_get_style (widget));
    style->bg[GTK_STATE_NORMAL] = color;
    style->bg[GTK_STATE_ACTIVE] = color;
    style->bg[GTK_STATE_PRELIGHT] = color;
    style->bg[GTK_STATE_SELECTED] = color;
    style->bg[GTK_STATE_INSENSITIVE] = color;
    gtk_widget_set_style (widget, style);
    gtk_style_unref (style);
}


static void FgColor
    (
    GtkWidget * widget,
    int color_num
    )
{
    GdkColor color;
    GtkStyle *style;
    char format[10];

    sprintf (format, "#%06X", color_num);
    gdk_color_parse (format, &color);
    style = gtk_style_copy (gtk_widget_get_style (widget));
    style->fg[GTK_STATE_NORMAL] = color;
    style->fg[GTK_STATE_ACTIVE] = color;
    style->fg[GTK_STATE_PRELIGHT] = color;
    style->fg[GTK_STATE_SELECTED] = color;
    style->fg[GTK_STATE_INSENSITIVE] = color;
    gtk_widget_set_style (widget, style);
    gtk_style_unref (style);
}


static void AddSpan
    (
    char *text,
    int font,
    int color,
    gboolean ul,
    gboolean st,
    gboolean it,
    int link
    )
{

    switch (font) {
        case 9:
            strcat (text, "<span size=\"small\"");
            break;
        case 8:
            strcat (text, "<span face=\"courier\"");
            break;
        case 7:
            strcat (text, "<span weight=\"bold\"");
            break;
        case 6:
            strcat (text, "<span size=\"10\" weight=\"bold\"");
            break;
        case 5:
            strcat (text, "<span size=\"12\" weight=\"bold\"");
            break;
        case 4:
            strcat (text, "<span size=\"14\" weight=\"bold\"");
            break;
        case 3:
            strcat (text, "<span size=\"16\" weight=\"bold\"");
            break;
        case 2:
            strcat (text, "<span size=\"18\" weight=\"bold\"");
            break;
        case 1:
            strcat (text, "<span size=\"20\" weight=\"bold\"");
            break;
        case 0:
        default:
            strcat (text, "<span");
            break;
    }
    sprintf (&text[strlen (text)], " color=\"#%06X\"", color);
    sprintf (&text[strlen (text)], " underline=\"%s\"",
             link ? "double" : ul ? "single" : "none");
    sprintf (&text[strlen (text)], " strikethrough=\"%s\"",
             st ? "true" : "false");
    sprintf (&text[strlen (text)], " style=\"%s\">",
             it ? "italic" : "normal");
}


static void AddCell
    (
    plkr_Document * doc,
    GtkWidget * table,
    char *text,
    int len,
    int x,
    int y,
    int cols,
    int rows,
    int align,
    int border,
    int border_color,
    GdkPixbuf * image,
    int link_color,
    int charset
    )
{
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *hbox;
    GtkWidget *vbox;
    GtkWidget *linebox;
    GtkWidget *pix;
    GtkWidget *subTable;
    GtkWidget *padding;
    char *chars;
    unsigned char *ptr;
    static int sp = 0, cl = 0;
    static gboolean ul = FALSE;
    static gboolean st = FALSE;
    static gboolean it = FALSE;
    guint link = 0;
    char the_text[MAX_RECORD_SIZE];
    int fctype, fclen;
    GdkPixbuf * subImage;

    if (x == 1 && y == 1) {
        sp = 0;
        cl = 0;
        ul = FALSE;
        st = FALSE;
        it = FALSE;
    }

    vbox = gtk_vbox_new (FALSE, 0);
    hbox = gtk_hbox_new (FALSE, 0);

    if (border) {
        GtkWidget *top = gtk_hseparator_new ();
        GtkWidget *left = gtk_vseparator_new ();
        BgColor (top, border_color);
        BgColor (left, border_color);
        gtk_box_pack_start (GTK_BOX (vbox), top, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (hbox), left, FALSE, FALSE, 0);
    }

    gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);
    padding = gtk_alignment_new (0, 0, 1, 1);
    gtk_box_pack_start (GTK_BOX (vbox), padding, TRUE, TRUE, 0);
    the_text[0] = '\0';
    AddSpan (the_text, sp, cl, ul, st, it, link);

    ptr = text;
    linebox = gtk_hbox_new (FALSE, 0);
    if (align != 0) {
        padding = gtk_alignment_new (0, 0, 1, 1);
        label = gtk_label_new (NULL);
        gtk_container_add (GTK_CONTAINER (padding), label);
        gtk_box_pack_start (GTK_BOX (linebox), padding, TRUE, TRUE, 0);
    }

    if (image) {
        pix = gtk_image_new_from_pixbuf (image);
        gtk_box_pack_start (GTK_BOX (linebox), pix, FALSE, TRUE, 0);
    }

    while (ptr < (unsigned char *) (text + len)) {
        if (ptr[0] == '\0') {
            fctype = GET_FUNCTION_CODE_TYPE (ptr[1]);
            fclen = 2 + GET_FUNCTION_CODE_DATALEN (ptr[1]);
            switch (fctype) {
                case PLKR_TFC_LINK:
                    strcat (the_text, "</span>");
                    label = gtk_label_new (NULL);
                    chars = FixText (the_text, charset);
                    gtk_label_set_markup (GTK_LABEL (label), chars);
                    gtk_box_pack_start (GTK_BOX (linebox), label,
                                        FALSE, TRUE, 0);
                    if (link) {
                        g_object_set_data (G_OBJECT (label), "link",
                                           GUINT_TO_POINTER (link));
                        gtk_label_set_selectable (GTK_LABEL (label), TRUE);
                    }

                    if (fclen == 4) /* ANCHOR_BEGIN */
                        link = READ_BIGENDIAN_SHORT (&ptr[2]);
                    else
                        link = 0;

                    the_text[0] = '\0';
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
                case PLKR_TFC_FONT:
                    strcat (the_text, "</span>");
                    sp = ptr[2];
                    AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
                case PLKR_TFC_NEWLINE:
                    strcat (the_text, "</span>");
                    label = gtk_label_new (NULL);
                    chars = FixText (the_text, charset);
                    gtk_label_set_markup (GTK_LABEL (label), chars);
                    gtk_box_pack_start (GTK_BOX (linebox), label,
                                        FALSE, TRUE, 0);
                    if (link) {
                        g_object_set_data (G_OBJECT (label), "link",
                                           GUINT_TO_POINTER (link));
                        gtk_label_set_selectable (GTK_LABEL (label), TRUE);
                    }
                    if (align != 1) {
                        padding = gtk_alignment_new (0, 0, 1, 1);
                        label = gtk_label_new (NULL);
                        gtk_container_add (GTK_CONTAINER (padding), label);
                        gtk_box_pack_start (GTK_BOX (linebox), padding,
                                            TRUE, TRUE, 0);
                    }

                    gtk_box_pack_start (GTK_BOX (vbox), linebox,
                                        FALSE, TRUE, 0);
                    the_text[0] = '\0';
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    linebox = gtk_hbox_new (FALSE, 0);
                    if (align != 0) {
                        padding = gtk_alignment_new (0, 0, 1, 1);
                        label = gtk_label_new (NULL);
                        gtk_container_add (GTK_CONTAINER (padding), label);
                        gtk_box_pack_start (GTK_BOX (linebox), padding,
                                            TRUE, TRUE, 0);
                    }
                    break;
                case PLKR_TFC_BITALIC:
                    strcat (the_text, "</span>");
                    it = TRUE;
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
                case PLKR_TFC_EITALIC:
                    strcat (the_text, "</span>");
                    it = FALSE;
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
                case PLKR_TFC_COLOR:
                    strcat (the_text, "</span>");
                    cl = (ptr[2] << 16) + (ptr[3] << 8) + ptr[4];
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
                case PLKR_TFC_BULINE:
                    strcat (the_text, "</span>");
                    ul = TRUE;
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
                case PLKR_TFC_EULINE:
                    strcat (the_text, "</span>");
                    ul = FALSE;
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
                case PLKR_TFC_BSTRIKE:
                    strcat (the_text, "</span>");
                    st = TRUE;
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
                case PLKR_TFC_ESTRIKE:
                    strcat (the_text, "</span>");
                    st = FALSE;
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
                case PLKR_TFC_TABLE:
                    if (fclen != 4) /* NEW_TABLE */
                        break;
                    strcat (the_text, "</span>");
                    label = gtk_label_new (NULL);
                    chars = FixText (the_text, charset);
                    gtk_label_set_markup (GTK_LABEL (label), chars);
                    gtk_box_pack_start (GTK_BOX (linebox), label,
                                        FALSE, TRUE, 0);
                    if (link) {
                        g_object_set_data (G_OBJECT (label), "link",
                                           GUINT_TO_POINTER (link));
                        gtk_label_set_selectable (GTK_LABEL (label), TRUE);
                    }

                    subTable = RenderTable (doc,
                                            READ_BIGENDIAN_SHORT (&ptr[2]));
                    gtk_box_pack_start (GTK_BOX (linebox), subTable,
                                        FALSE, TRUE, 0);
                    the_text[0] = '\0';
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
                case PLKR_TFC_IMAGE:
                case PLKR_TFC_IMAGE2:
                    strcat (the_text, "</span>");
                    label = gtk_label_new (NULL);
                    chars = FixText (the_text, charset);
                    gtk_label_set_markup (GTK_LABEL (label), chars);
                    gtk_label_set_justify (GTK_LABEL (label), align);
                    gtk_misc_set_alignment (GTK_MISC (label), align * 0.5, 1);
                    gtk_box_pack_start (GTK_BOX (linebox), label,
                                        FALSE, TRUE, 0);
                    if (link) {
                        g_object_set_data (G_OBJECT (label), "link",
                                           GUINT_TO_POINTER (link));
                        gtk_label_set_selectable (GTK_LABEL (label), TRUE);
                    }

                    if (fclen == 4) /* single image */
                        subImage = GetImageRecord (doc,
                                     READ_BIGENDIAN_SHORT (&ptr[2]), 1);
                    else
                        subImage = GetImageRecord (doc,
                                     READ_BIGENDIAN_SHORT (&ptr[4]), 1);
                    pix = gtk_image_new_from_pixbuf (subImage);

                    if (link || fclen == 6) {
                        GtkWidget *eventbox;
                        GtkWidget *alignment;
                        guint doublelink = 0;

                        eventbox = gtk_event_box_new();
                        alignment = gtk_alignment_new(0.5, 0.5, 0, 0);
                        if (fclen == 6)
                            doublelink = READ_BIGENDIAN_SHORT (&ptr[2]);
                        if (link)
                            doublelink = (doublelink << 16) | link;
                        gtk_container_add (GTK_CONTAINER (eventbox), pix);
                        gtk_container_add (GTK_CONTAINER (alignment), eventbox);
                        g_object_set_data (G_OBJECT (eventbox), "link",
                                           GUINT_TO_POINTER (doublelink));
                        gtk_box_pack_start (GTK_BOX (linebox), alignment,
                                            TRUE, TRUE, 0);
                    } 
                    else {
                        gtk_box_pack_start (GTK_BOX (linebox), pix,
                                            TRUE, TRUE, 0);
                    }

                    the_text[0] = '\0';
                    if (link)
                        AddSpan (the_text, sp, link_color, ul, st, it, link);
                    else
                        AddSpan (the_text, sp, cl, ul, st, it, link);
                    break;
            }
            ptr += fclen;
        }
        else {
            unsigned char *end;

            end = ptr + strlen (ptr);
            while (ptr < end) {
                switch (*ptr) {
                    case '&':
                        strcat (the_text, "&amp;");
                        break;
                    case '<':
                        strcat (the_text, "&lt;");
                        break;
                    case '>':
                        strcat (the_text, "&gt;");
                        break;
                    case '\'':
                        strcat (the_text, "&apos;");
                        break;
                    case '"':
                        strcat (the_text, "&quot;");
                        break;
                    default:
                        strncat (the_text, ptr, 1);
                        break;
                }
                ptr++;
            }
        }
    }
    strcat (the_text, "</span>");
    label = gtk_label_new (NULL);
    chars = FixText (the_text, charset);
    gtk_label_set_markup (GTK_LABEL (label), chars);
    gtk_box_pack_start (GTK_BOX (linebox), label, FALSE, TRUE, 0);
    if (align != 1) {
        padding = gtk_alignment_new (0, 0, 1, 1);
        label = gtk_label_new (NULL);
        gtk_container_add (GTK_CONTAINER (padding), label);
        gtk_box_pack_start (GTK_BOX (linebox), padding, TRUE, TRUE, 0);
    }
    gtk_box_pack_start (GTK_BOX (vbox), linebox, FALSE, TRUE, 0);
    padding = gtk_alignment_new (0, 0, 1, 1);
    gtk_box_pack_start (GTK_BOX (vbox), padding, TRUE, TRUE, 0);

    if (border) {
        GtkWidget *bottom = gtk_hseparator_new ();
        GtkWidget *right = gtk_vseparator_new ();
        BgColor (bottom, border_color);
        BgColor (right, border_color);
        gtk_box_pack_start (GTK_BOX (vbox), bottom, FALSE, FALSE, 0);
        gtk_box_pack_start (GTK_BOX (hbox), right, FALSE, FALSE, 0);
    }

    gtk_table_attach_defaults (GTK_TABLE (table), hbox, x, x + cols, y,
                               y + rows);
}


static GtkWidget *RenderTable
    (
    plkr_Document * doc,
    gint id
    )
{
    GtkWidget *table;
    int x, y, x2, y2;
    plkr_DataRecordType type;
    unsigned char record_bytes[MAX_RECORD_SIZE];
    int record_bytes_len;
    int rows, cols, border, len;
    unsigned char *ptr, *last;
    Cell *cells, *acell;
    int border_color, link_color;
    int charset;
    int fctype, fclen;

    record_bytes_len =
        plkr_CopyRecordBytes (doc, id, &record_bytes[0],
                              sizeof (record_bytes), &type);
    if (record_bytes_len == 0) {
        g_warning ("Can't get bytes of record %d", id);
        return NULL;
    }

    charset = plkr_GetRecordCharset (doc, id);

    len = READ_BIGENDIAN_SHORT (&record_bytes[8]);
    cols = READ_BIGENDIAN_SHORT (&record_bytes[10]);
    rows = READ_BIGENDIAN_SHORT (&record_bytes[12]);
    border = record_bytes[15];
    border_color = READ_BIGENDIAN_LONG (&record_bytes[16]);
    link_color = READ_BIGENDIAN_LONG (&record_bytes[20]);

    cells = (Cell *) calloc (cols * rows, sizeof (Cell));

    table = gtk_table_new (cols + 2, rows + 2, FALSE);

    ptr = &record_bytes[24];
    last = ptr + READ_BIGENDIAN_SHORT (&record_bytes[8]) - 1;

    x = y = -1;
    while (ptr < last) {
        if (ptr[0] == '\0') {
            fctype = GET_FUNCTION_CODE_TYPE (ptr[1]);
            fclen = 2 + GET_FUNCTION_CODE_DATALEN (ptr[1]);
            switch (fctype) {
                case PLKR_TFC_TABLE:
                    switch (fclen) {
                        case 2:        /* NEW_ROW */
                            y++;
                            x = -1;
                            ptr += fclen;
                            break;
                        case 9:        /* NEW_CELL */
                            x++;
                            acell = &CELLS (y, x);
                            acell->align = ptr[2];
                            if (READ_BIGENDIAN_SHORT (&ptr[3]))
                                acell->image =
                                    GetImageRecord (doc,
                                                    READ_BIGENDIAN_SHORT
                                                    (&ptr[3]), 1);
                            acell->colspan = ptr[5];
                            acell->rowspan = ptr[6];
                            acell->len = READ_BIGENDIAN_SHORT (&ptr[7]);
                            ptr += fclen;
                            acell->text = ptr;
                            /* Skip text */
                            ptr += acell->len;
                            /* add empty cells for colspan */
                            if (1 < acell->colspan)
                                x += acell->colspan - 1;
                            break;
                        default:
                            ptr += fclen;
                    }
                    break;
                default:
                    ptr += fclen;
            }
        }
        else {
            return NULL;
        }
    }

    for (y = 0; y < rows; y++)
        for (x = 0; x < cols; x++)
            if (CELLS (y, x).rowspan > 1)
                for (y2 = y + 1; y2 < y + CELLS (y, x).rowspan; y2++) {
                    for (x2 = cols - 1; x < x2; x2--)
                        memmove (&CELLS (y2, x2), &CELLS (y2, x2 - 1),
                                 sizeof (Cell));
                    memset (&CELLS (y2, x), 0, sizeof (Cell));
                }

    for (y = 0; y < rows; y++) {
        for (x = 0; x < cols; x++) {
            acell = &CELLS (y, x);
            if (acell->len || acell->image)
                AddCell (doc, table, acell->text, acell->len, x + 1, y + 1,
                          acell->colspan, acell->rowspan, acell->align, border,
                          border_color, acell->image, link_color, charset);

        }
    }

    free (cells);

    if (border) {
        GtkWidget *sep;

        sep = gtk_hseparator_new ();
        BgColor (sep, border_color);
        gtk_table_attach (GTK_TABLE (table), sep,
                          0, cols + 2, 0, 1, GTK_FILL, 0, 0, 0);

        sep = gtk_hseparator_new ();
        BgColor (sep, border_color);
        gtk_table_attach (GTK_TABLE (table), sep,
                          0, cols + 2, rows + 1, rows + 2,
                          GTK_FILL, 0, 0, 0);

        sep = gtk_vseparator_new ();
        BgColor (sep, border_color);
        gtk_table_attach (GTK_TABLE (table), sep,
                          0, 1, 0, rows + 2, 0, GTK_FILL, 0, 0);

        sep = gtk_vseparator_new ();
        BgColor (sep, border_color);
        gtk_table_attach (GTK_TABLE (table), sep,
                          cols + 1, cols + 2, 0, rows + 2,
                          0, GTK_FILL, 0, 0);
    }

    return table;

}

static GtkTextTagTable *CreateTags
    (
    )
{
    GtkTextTagTable *table;
    GtkTextTag *tag;

    table = gtk_text_tag_table_new ();

    tag = gtk_text_tag_new ("h1");
    g_object_set (tag, "scale", 2.0, "weight", PANGO_WEIGHT_BOLD,
                  "justification", GTK_JUSTIFY_CENTER, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("h2");
    g_object_set (tag, "scale", 1.8, "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("h3");
    g_object_set (tag, "scale", 1.6, "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("h4");
    g_object_set (tag, "scale", 1.4, "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("h5");
    g_object_set (tag, "scale", 1.2, "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("h6");
    g_object_set (tag, "scale", 1.0, "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("tt");
    g_object_set (tag, "family", "monospace", NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("small");
    g_object_set (tag, "scale", 0.75, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("italic");
    g_object_set (tag, "style", PANGO_STYLE_ITALIC, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("bold");
    g_object_set (tag, "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("underline");
    g_object_set (tag, "underline", PANGO_UNDERLINE_SINGLE, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("strikethrough");
    g_object_set (tag, "strikethrough", TRUE, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("centered");
    g_object_set (tag, "justification", GTK_JUSTIFY_CENTER, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("left-justified");
    g_object_set (tag, "justification", GTK_JUSTIFY_LEFT, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("right-justified");
    g_object_set (tag, "justification", GTK_JUSTIFY_RIGHT, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("filled");
    g_object_set (tag, "justification", GTK_JUSTIFY_FILL, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("color-000000");
    g_object_set (tag, "foreground", "#000000", NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("margins-03-03");
    g_object_set (tag, "left-margin", 3, "right-margin", 3, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("url-link");
    g_object_set (tag, "underline", PANGO_UNDERLINE_DOUBLE, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("mailto-link");
    g_object_set (tag, "underline", PANGO_UNDERLINE_DOUBLE, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("record-link");
    g_object_set (tag, "underline", PANGO_UNDERLINE_DOUBLE, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("record-para-link");
    g_object_set (tag, "underline", PANGO_UNDERLINE_DOUBLE, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("selected-link");
    g_object_set (tag, "background", "pink", NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("wrap");
    g_object_set (tag, "wrap_mode", GTK_WRAP_WORD, NULL);
    gtk_text_tag_table_add (table, tag);

    tag = gtk_text_tag_new ("overall_attributes");
    g_object_set (tag,
                  "editable", FALSE,
                  "indent", 0,
                  "pixels-above-lines", 8, "pixels-below-lines", 2, NULL);
    gtk_text_tag_table_add (table, tag);

    return table;
}

static gint CompareLinks
    (
    gconstpointer p1,
    gconstpointer p2
    )
{
    if (((Link *) p1)->start < ((Link *) p2)->start)
        return -1;
    else if (((Link *) p1)->start > ((Link *) p2)->start)
        return 1;
    else
        return 0;
}

static gint LinkSearch
    (
    gconstpointer p1,
    gconstpointer p2
    )
{
    /* g_tree_search is broken; the direction is wrong.
       So we return the wrong values here to make it work.
     */

    if (((Link *) p1)->start <= GPOINTER_TO_INT (p2)
        && ((Link *) p1)->end > GPOINTER_TO_INT (p2))
        return 0;
    else if (((Link *) p1)->end <= GPOINTER_TO_INT (p2))
        return 1;
    else
        return -1;
}

static gint ComparePointerInts
    (
    gconstpointer p1,
    gconstpointer p2
    )
{
    if (GPOINTER_TO_UINT (p1) < GPOINTER_TO_UINT (p2))
        return -1;
    else if (GPOINTER_TO_UINT (p1) > GPOINTER_TO_UINT (p2))
        return 1;
    else
        return 0;
}

static int PagelistGetPageID
    (
    GTree * list,
    int record_index
    )
{
    Page *p =
        (Page *) g_tree_lookup (list, GINT_TO_POINTER (record_index));

    if (p)
        return p->page_id;
    else
        return record_index;
}

static Page *PagelistAddRecord
    (
    GTree * list,
    int index
    )
{
    Page *newr;
    Page *ptr = (Page *) g_tree_lookup (list, GINT_TO_POINTER (index));

    if (ptr)
        return ptr;

    newr = (Page *) malloc (sizeof (Page));
    newr->index = index;
    newr->page_id = index;
    newr->formatted_view = NULL;
    newr->links = g_tree_new (CompareLinks);
    newr->paragraphs = g_tree_new (ComparePointerInts);
    newr->current_link = NULL;
    newr->doc = NULL;
    newr->hrules = NULL;
    newr->tables = NULL;
    g_tree_insert (list, GINT_TO_POINTER (index), newr);
    return newr;
}

static void PagelistSetPageID
    (
    GTree * list,
    int index,
    int page_id
    )
{
    Page *ptr = (Page *) g_tree_lookup (list, GINT_TO_POINTER (index));

    if (ptr == NULL)
        ptr = PagelistAddRecord (list, index);

    ptr->page_id = page_id;
}

static void PagelistAddLink
    (
    GTree * list,
    int index,
    LinkType type,
    int start,
    int end,
    char *url,
    int record,
    int para,
    int image
    )
{
    Page *ptr = (Page *) g_tree_lookup (list, GINT_TO_POINTER (index));
    Link *newl;

    if (ptr == NULL)
        ptr = PagelistAddRecord (list, index);

    newl = (Link *) malloc (sizeof (Link));
    newl->start = start;
    newl->end = end;
    newl->type = type;
    newl->record = record;
    newl->para = para;
    newl->url = url;
    newl->image = image;
    g_tree_insert (ptr->links, newl, newl);
}

typedef struct {
    int size;
    int attributes;
} ParagraphInfo;

static ParagraphInfo *ParseParagraphInfo
    (
    unsigned char *bytes,
    int len,
    int *nparas
    )
{
    ParagraphInfo *paragraph_info;
    int j, n;

    n = (bytes[2] << 8) + bytes[3];
    paragraph_info = (ParagraphInfo *) malloc (sizeof (ParagraphInfo) * n);
    for (j = 0; j < n; j++) {
        paragraph_info[j].size =
            (bytes[8 + (j * 4) + 0] << 8) + bytes[8 + (j * 4) + 1];
        paragraph_info[j].attributes =
            (bytes[8 + (j * 4) + 2] << 8) + bytes[8 + (j * 4) + 3];
    }
    *nparas = n;
    return paragraph_info;
}

typedef struct {
    char *tag_name;
    gint start;
} FormatStart;

static GSList *PushFormat
    (
    GSList * current,
    char *tag_name,
    GtkTextIter * where
    )
{
    FormatStart *n = (FormatStart *) malloc (sizeof (FormatStart));

    n->tag_name = g_strdup (tag_name);
    n->start = gtk_text_iter_get_offset (where);
/*
   fprintf (stderr, "Starting format '%s' at %d\n", tag_name, n->start);
 */
    return g_slist_prepend (current, n);
}

static GSList *ApplyFormatting
    (
    GSList * current_formatting,
    char *tag_name,
    GtkTextIter * end
    )
{
    /* find the most recent format start with "tag_name", apply the named tag
       to start:point, and remove the format start from the stack */

    GSList *ptr;
    FormatStart *candidate;
    GtkTextIter start;
    GtkTextBuffer *buffer;

    for (ptr = current_formatting; ptr != NULL; ptr = ptr->next) {
        candidate = (FormatStart *) (ptr->data);
        if (strcmp (tag_name, candidate->tag_name) == 0) {
/*
   fprintf (stderr, "Applying tag '%s' to %d:%d\n", tag_name,
   candidate->start, gtk_text_iter_get_offset(end));
 */
            buffer = gtk_text_iter_get_buffer (end);
            gtk_text_buffer_get_iter_at_offset (buffer, &start,
                                                candidate->start);
            gtk_text_buffer_apply_tag_by_name (buffer, tag_name, &start,
                                               end);
            g_free (candidate->tag_name);
            free (candidate);
            return g_slist_remove (current_formatting, candidate);
        }
    }
    g_warning ("No starting tag for tag named '%s' found!\n", tag_name);
    return current_formatting;
}

static gboolean HasTag
    (
    GSList * current_formatting,
    char *tag_name
    )
{
    GSList *ptr;

    for (ptr = current_formatting; ptr != NULL; ptr = ptr->next) {
        if (strcmp (tag_name, ((FormatStart *) (ptr->data))->tag_name) ==
            0)
            return TRUE;
    }
    return FALSE;
}

static void InsertText
    (
    GtkTextBuffer * buffer,
    GtkTextIter * point,
    char *chars,
    int charslen,
    int charset_mibenum
    )
{
    /* fprintf (stderr, "          inserting '%*.*s'\n", charslen, charslen, chars); */

    if (charset_mibenum != UTF8_CHARSET_MIBENUM) {
        gchar *newchars;
        gsize newcharslen, bytesread;
        char *charset_name;

        if (charset_mibenum == 0)
            charset_name = "ISO-8859-1";
        else
            charset_name =
                g_tree_lookup (CharsetNames,
                               GINT_TO_POINTER (charset_mibenum));
        if (charset_name == NULL) {
            fprintf (stderr,
                     "Unknown charset %d encountered.  Skipping text.\n",
                     charset_mibenum);
        }
        else {
            newchars =
                g_convert_with_fallback (chars, charslen, "UTF-8",
                                         charset_name, NULL, &bytesread,
                                         &newcharslen, NULL);
            if (newchars)
                gtk_text_buffer_insert (buffer, point, newchars,
                                        newcharslen);
            else
                fprintf (stderr,
                         "No conversion from charset '%s' to UTF-8.  Skipping text.\n",
                         charset_name);
        }
    }
    else {
        gtk_text_buffer_insert (buffer, point, chars, charslen);
    }
}

static gboolean IsWhitespaceRun
    (
    char *ptr,
    int len
    )
{
    int i;
    gboolean status = TRUE;

    for (i = 0; i < len; i++)
        if (!isspace (ptr[i])) {
            status = FALSE;
            break;
        }
    /* fprintf (stderr, "status on '%*.*s' is %s\n", len, len, ptr, status ? "TRUE" : "FALSE"); */
    return status;
}

static Page *GetImagePage
    (
    GTree * pages,
    plkr_Document * doc,
    int id,
    gdouble scale
    )
{
    Page *page;
    GtkTextBuffer *formatted_view;
    GtkTextIter point, start;
    GdkPixbuf *image;

    if (TheTagsTable == NULL)
        TheTagsTable = CreateTags ();

    page = PagelistAddRecord (pages, id);

    if (page->formatted_view == NULL) {

        image = GetImageRecord (doc, id, scale);
        if (image == NULL) {
            g_warning ("Can't get bytes of image record %d", id);
            return NULL;
        }

        page->doc = doc;

        formatted_view = gtk_text_buffer_new (TheTagsTable);
        gtk_text_buffer_get_iter_at_offset (formatted_view, &point, 0);
        gtk_text_buffer_insert_pixbuf (formatted_view, &point, image);
        gtk_text_buffer_get_bounds (formatted_view, &start, &point);
        gtk_text_buffer_apply_tag_by_name (formatted_view, "centered",
                                           &start, &point);

        page->formatted_view = formatted_view;
        g_tree_insert (page->paragraphs, GUINT_TO_POINTER (id),
                       GUINT_TO_POINTER (0));
    }

    return page;
}


static Page *GetTablePage
    (
    GTree * pages,
    plkr_Document * doc,
    int id,
    gdouble scale
    )
{
    GtkTextChildAnchor *anchor;
    GtkTextIter start_iter;
    gint table;
    GtkRequisition requisition;
    TABLE *tnode;
    Page *page;
    GtkTextBuffer *formatted_view;
    GtkTextIter point, start;

    if (TheTagsTable == NULL)
        TheTagsTable = CreateTags ();

    page = PagelistAddRecord (pages, id);

    if (page->formatted_view == NULL) {

        formatted_view = gtk_text_buffer_new (TheTagsTable);
        page->doc = doc;
        gtk_text_buffer_get_iter_at_offset (formatted_view, &point, 0);

        tnode = (TABLE *) malloc (sizeof (TABLE));
        anchor =
            gtk_text_buffer_create_child_anchor (formatted_view, &point);
        tnode->anchor = anchor;
        tnode->table = id;
        page->tables = g_slist_append (page->tables, tnode);
        InsertText (formatted_view, &point, "\n", 1, UTF8_CHARSET_MIBENUM);
        gtk_text_buffer_get_bounds (formatted_view, &start, &point);
        gtk_text_buffer_apply_tag_by_name (formatted_view, "centered",
                                           &start, &point);

        page->formatted_view = formatted_view;
        g_tree_insert (page->paragraphs, GUINT_TO_POINTER (id),
                       GUINT_TO_POINTER (0));
    }

    return page;
}


static Page *GetTextPage
    (
    GTree * pages,
    plkr_Document * doc,
    int id,
    double scale
    )
{
    plkr_DataRecordType type;
    int home_id = plkr_GetHomeRecordID (doc);
    Page *page;
    unsigned char *ptr, *run, *para_start, *start;
    int fctype, fclen, para_index, para_len, charset;
    gboolean first_record_of_page = TRUE, first_text_of_paragraph;
    int record_index;
    int nparagraphs;
    int current_font_code, current_alignment, current_color,
        current_margins;
    ParagraphInfo *paragraphs;
    GtkWidget *separator;
    GtkTextBuffer *formatted_view;
    GtkTextIter point;
    GtkTextIter buffer_start, buffer_end;
    GSList *active_formatting = NULL;
    unsigned char record_bytes[MAX_RECORD_SIZE];
    int record_bytes_len;
    char charbuf[7];
    int utflen;
    gunichar unichar;

    LinkType current_link_type;
    int current_link_start;
    char *current_link_url;
    int current_link_record, current_link_para;
    int current_link_image;
    gboolean current_link;
    int current_glyph_page = 0;

    gboolean on_new_line;
    static char *alignment_names[] = { "left-justified",
        "right-justified",
        "centered",
        "filled"
    };
    static char *style_names[] = { "normal",
        "h1",
        "h2",
        "h3",
        "h4",
        "h5",
        "h6",
        "bold",
        "tt",
        "small"
    };


    if (TheTagsTable == NULL)
        TheTagsTable = CreateTags ();

    record_index = id;

    page = PagelistAddRecord (pages, id);

    if (page->formatted_view == NULL) {

        record_bytes_len =
            plkr_CopyRecordBytes (doc, id, &record_bytes[0],
                                  sizeof (record_bytes), &type);
        if (record_bytes_len == 0) {
            g_warning ("Can't get bytes of record %d", id);
            return NULL;
        }

        page->doc = doc;
        charset = plkr_GetRecordCharset (doc, id);

        formatted_view = gtk_text_buffer_new (TheTagsTable);
        gtk_text_buffer_get_iter_at_offset (formatted_view, &point, 0);
        on_new_line = TRUE;

        paragraphs =
            ParseParagraphInfo (record_bytes, record_bytes_len,
                                &nparagraphs);
        start =
            &record_bytes[0] + 8 + ((record_bytes[2] << 8) +
                                    record_bytes[3]) * 4;

        current_link = FALSE;

        for (para_index = 0, ptr = start, run = start;
             para_index < nparagraphs; para_index++) {

            para_len = paragraphs[para_index].size;

            /* If the paragraph is the last in the record, and it consists
               of a link to the next record in the logical page, we trim off
               the paragraph and instead insert the whole page */

            if (((para_index + 1) == nparagraphs) &&
                (para_len == (sizeof ("Click here for the next part") + 5))
                && (*ptr == 0) && (ptr[1] == ((PLKR_TFC_LINK << 3) + 2))
                && (strcmp (ptr + 4, "Click here for the next part") == 0)) {

                record_index = (ptr[2] << 8) + ptr[3];
                if ((record_bytes_len =
                     plkr_CopyRecordBytes (doc, record_index,
                                           &record_bytes[0],
                                           sizeof (record_bytes),
                                           &type)) == 0) {
                    g_warning ("Can't open record %d!", record_index);
                    return FALSE;
                }
                else if (!(type == PLKR_DRTYPE_TEXT_COMPRESSED ||
                           type == PLKR_DRTYPE_TEXT)) {
                    g_warning
                        ("Bad record type %d in record linked from end of record %d",
                         type, id);
                    return FALSE;
                }
                first_record_of_page = FALSE;
                para_index = 0;
                ptr =
                    &record_bytes[0] + 8 + ((record_bytes[2] << 8) +
                                            record_bytes[3]) * 4;
                run = ptr;
                free (paragraphs);
                paragraphs =
                    ParseParagraphInfo (record_bytes, record_bytes_len,
                                        &nparagraphs);
                para_len = paragraphs[para_index].size;
                PagelistSetPageID (pages, record_index, id);
            }

            /* If a paragraph is the first in a record, and it consists
               of a link to the previous record in the logical page, we trim it off */

            if ((para_index == 0) && !first_record_of_page &&
                (*ptr == 0) && (ptr[1] == ((PLKR_TFC_LINK << 3) + 2)) &&
                (strcmp (ptr + 4, "Click here for the previous part") ==
                 0)) {
                /* throw away this inserted paragraph */
                ptr += para_len;
                run = ptr;
                continue;
            }

            /* at the beginning of a paragraph, we start with a clean graphics context */

            if (!on_new_line) {
                InsertText (formatted_view, &point, "\n", 1, charset);
                on_new_line = TRUE;
            }

            first_text_of_paragraph = TRUE;

            g_tree_insert (page->paragraphs,
                           GINT_TO_POINTER ((para_index << 16) +
                                            record_index),
                           GINT_TO_POINTER (gtk_text_iter_get_offset
                                            (&point)));

            current_font_code = 0;
            current_color = 0x0;
            active_formatting =
                PushFormat (active_formatting, "color-000000", &point);
            current_alignment = 0;
            active_formatting =
                PushFormat (active_formatting, "left-justified", &point);
            current_margins = (3 << 8) + 3;
            active_formatting =
                PushFormat (active_formatting, "margins-03-03", &point);
            active_formatting =
                PushFormat (active_formatting, "wrap", &point);

            for (para_start = ptr, run = ptr;
                 (ptr - para_start) < para_len;) {

                if (*ptr == 0) {
                    /* function code */

                    if ((ptr - run) > 0) {
                        if (!on_new_line
                            || !IsWhitespaceRun (run, ptr - run)) {
                            InsertText (formatted_view, &point, run,
                                        (ptr - run), charset);
/*
   fprintf (stderr, "para %d:  wrote text:  '%*.*s'\n", para_index,
   (ptr-run), (ptr-run),  run);
 */
                            on_new_line = FALSE;
                        }
                        run = ptr;
                    }

                    ptr++;
                    fctype = GET_FUNCTION_CODE_TYPE (*ptr);
                    fclen = GET_FUNCTION_CODE_DATALEN (*ptr);
                    ptr++;

                    if (fctype == PLKR_TFC_NEWLINE) {

                        /* skip it if it's at the end of a paragraph */

                        /* fprintf (stderr, "para %d:  newline\n", para_index); */

                        if ((ptr - para_start + fclen) < para_len) {

                            if (UseUnicode
                                && !UnicodeNewlineSeparatorBroken) {

                                InsertText (formatted_view, &point,
                                            UNICODE_LS_STRING, -1,
                                            UTF8_CHARSET_MIBENUM);

                            }
                            else {

                                if (!on_new_line) {
                                    InsertText (formatted_view, &point,
                                                "\n", 1, charset);
                                    on_new_line = TRUE;
                                }

                            }
                            on_new_line = TRUE;

                        }

                    }
                    else if (fctype == PLKR_TFC_FONT) {

                        if (current_font_code != *ptr) {
                            if (current_font_code > 0
                                && current_font_code < 10)
                                active_formatting =
                                    ApplyFormatting (active_formatting,
                                                     style_names
                                                     [current_font_code],
                                                     &point);
                            current_font_code = *ptr;
                            if (current_font_code > 0
                                && current_font_code < 10)
                                active_formatting =
                                    PushFormat (active_formatting,
                                                style_names
                                                [current_font_code],
                                                &point);
                        }

                    }
                    else if (fctype == PLKR_TFC_ALIGN) {

                        /* fprintf (stderr, "Alignment code %d\n", *ptr); */

                        if ((current_alignment != *ptr) && (*ptr < 4)) {
                            if (current_alignment < 4
                                && current_alignment >= 0)
                                active_formatting =
                                    ApplyFormatting (active_formatting,
                                                     alignment_names
                                                     [current_alignment],
                                                     &point);
                            current_alignment = *ptr;
                            if (current_alignment < 4
                                && current_alignment >= 0)
                                active_formatting =
                                    PushFormat (active_formatting,
                                                alignment_names
                                                [current_alignment],
                                                &point);
                        }

                    }
                    else if (fctype == PLKR_TFC_BITALIC) {

                        /* fprintf (stderr, "para %d:  start_italics\n", para_index); */
                        active_formatting =
                            PushFormat (active_formatting, "italic",
                                        &point);

                    }
                    else if (fctype == PLKR_TFC_EITALIC) {

                        /* fprintf (stderr, "para %d:  end_italics\n", para_index); */
                        active_formatting =
                            ApplyFormatting (active_formatting, "italic",
                                             &point);

                    }
                    else if (fctype == PLKR_TFC_BULINE) {

                        active_formatting =
                            PushFormat (active_formatting, "underline",
                                        &point);

                    }
                    else if (fctype == PLKR_TFC_EULINE) {

                        active_formatting =
                            ApplyFormatting (active_formatting,
                                             "underline", &point);

                    }
                    else if (fctype == PLKR_TFC_BSTRIKE) {

                        active_formatting =
                            PushFormat (active_formatting, "strikethrough",
                                        &point);

                    }
                    else if (fctype == PLKR_TFC_ESTRIKE) {

                        active_formatting =
                            ApplyFormatting (active_formatting,
                                             "strikethrough", &point);

                    }
                    else if (fctype == PLKR_TFC_UCHAR) {

                        if (UseUnicode) {

                            if (fclen == 3)
                                unichar = READ_BIGENDIAN_SHORT (ptr + 1);
                            else if (fclen == 5)
                                unichar = READ_BIGENDIAN_LONG (ptr + 1);

                            utflen = g_unichar_to_utf8 (unichar, charbuf);
                            charbuf[utflen] = 0;
                            InsertText (formatted_view, &point, charbuf,
                                        -1, UTF8_CHARSET_MIBENUM);
                            on_new_line = FALSE;
                            first_text_of_paragraph = FALSE;

                            /* skip over alternate text */
                            ptr += ptr[0];

                        }

                    }
                    else if (fctype == PLKR_TFC_MARGINS) {

                        int margins;
                        char margins_spec[16];
                        GtkTextTag *margins_tag;

                        margins = (ptr[0] << 8) + ptr[1];
                        if (current_margins != margins) {
                            sprintf (margins_spec, "margins-%02x-%02x",
                                     current_margins >> 8,
                                     current_margins & 0xFF);
                            active_formatting =
                                ApplyFormatting (active_formatting,
                                                 margins_spec, &point);
                            sprintf (margins_spec, "margins-%02x-%02x",
                                     ptr[0], ptr[1]);
                            margins_tag =
                                gtk_text_tag_table_lookup (TheTagsTable,
                                                           margins_spec);
                            if (margins_tag == NULL) {
                                margins_tag =
                                    gtk_text_tag_new (margins_spec);
                                g_object_set (margins_tag, "left-margin",
                                              ptr[0], "right-margin",
                                              ptr[1], NULL);
                                gtk_text_tag_table_add (TheTagsTable,
                                                        margins_tag);
                            }
                            current_margins = margins;
                            active_formatting =
                                PushFormat (active_formatting,
                                            margins_spec, &point);
                        }

                    }
                    else if (fctype == PLKR_TFC_COLOR) {

                        char color_spec[16];
                        GtkTextTag *color_tag;
                        int color;

                        color = (ptr[0] << 16) + (ptr[1] << 8) + ptr[2];
                        if (color != current_color) {
                            sprintf (color_spec, "color-%06x",
                                     current_color);
                            active_formatting =
                                ApplyFormatting (active_formatting,
                                                 color_spec, &point);
                            sprintf (color_spec, "color-%06x", color);
                            color_tag =
                                gtk_text_tag_table_lookup (TheTagsTable,
                                                           color_spec);
                            if (color_tag == NULL) {
                                color_tag = gtk_text_tag_new (color_spec);
                                sprintf (color_spec, "#%06x", color);
                                g_object_set (color_tag, "foreground",
                                              color_spec, NULL);
                                gtk_text_tag_table_add (TheTagsTable,
                                                        color_tag);
                                sprintf (color_spec, "color-%06x", color);
                            }
                            current_color = color;
                            active_formatting =
                                PushFormat (active_formatting, color_spec,
                                            &point);
                        }

                    }
                    else if (fctype == PLKR_TFC_LINK) {

                        char link_end_spec[20];
                        gboolean real_record;
                        int record_id, paragraph_id, datalen;
                        plkr_DataRecordType type = 0;
                        char *url = NULL;
                        GtkTextIter link_start_iter;

                        if (fclen == 0) {
                            if (current_link) {
                                gtk_text_buffer_get_iter_at_offset
                                    (formatted_view, &link_start_iter,
                                     current_link_start);
                                gtk_text_buffer_apply_tag_by_name
                                    (formatted_view,
                                     link_type_names[current_link_type],
                                     &link_start_iter, &point);
                                PagelistAddLink (pages, id,
                                                   current_link_type,
                                                   current_link_start,
                                                   gtk_text_iter_get_offset
                                                   (&point),
                                                   current_link_url,
                                                   current_link_record,
                                                   current_link_para,
                                                   current_link_image);
                                current_link = FALSE;
                            }
                            else {
                                g_warning
                                    ("Link end code encounted while no link is active!");
                            }
                        }
                        else if (current_link) {

                            g_warning
                                ("New link encounted while old link is still active!");

                        }
                        else {

                            current_link = TRUE;
                            current_link_record =
                                READ_BIGENDIAN_SHORT (ptr);
                            current_link_image = 0;
                            current_link_start =
                                gtk_text_iter_get_offset (&point);
                            real_record =
                                plkr_HasRecordWithID (doc,
                                                      current_link_record);

                            if (!real_record) {
                                /* must be a URL that wasn't plucked */
                                current_link_url =
                                    plkr_GetRecordURL (doc,
                                                       current_link_record);
                                current_link_type = UrlLink;

                            }
                            else if (fclen == 2) {
                                plkr_DataRecordType t =
                                    plkr_GetRecordType (doc,
                                                        current_link_record);
                                if (t == PLKR_DRTYPE_MAILTO)
                                    current_link_type = MailtoLink;
                                else
                                    current_link_type = RecordLink;

                            }
                            else if (fclen == 4) {
                                current_link_para =
                                    READ_BIGENDIAN_SHORT (ptr + 2);
                                current_link_type = RecordParaLink;

                            }
                            else {
                                g_warning
                                    ("Bad length in LINK function code encountered (%d)",
                                     fclen);
                            }
                        }

                    }
                    else if (fctype == PLKR_TFC_IMAGE) {

                        GdkPixbuf *image;

                        image = GetImageRecord (doc, READ_BIGENDIAN_SHORT (ptr),
                                                scale);
                        gtk_text_buffer_insert_pixbuf (formatted_view, &point,
                                                       image);
                        on_new_line = FALSE;
                        first_text_of_paragraph = FALSE;

                    }
                    else if (fctype == PLKR_TFC_HRULE) {

                        GtkTextChildAnchor *anchor;
                        int start_pos;
                        GtkTextIter start_iter;

                        if (!on_new_line)
                            InsertText (formatted_view, &point, "\n", 1,
                                        charset);
                        if (current_alignment != 2)
                            active_formatting =
                                ApplyFormatting (active_formatting,
                                                 alignment_names
                                                 [current_alignment],
                                                 &point);
                        start_pos = gtk_text_iter_get_offset (&point);
                        anchor =
                            gtk_text_buffer_create_child_anchor
                            (formatted_view, &point);
                        gtk_text_buffer_get_iter_at_offset (formatted_view,
                                                            &start_iter,
                                                            start_pos);
                        gtk_text_buffer_apply_tag_by_name (formatted_view,
                                                           "centered",
                                                           &start_iter,
                                                           &point);
                        if (current_alignment != 2)
                            active_formatting =
                                PushFormat (active_formatting,
                                            alignment_names
                                            [current_alignment], &point);
                        page->hrules =
                            g_slist_prepend (page->hrules, anchor);
                        InsertText (formatted_view, &point, "\n", 1,
                                    charset);
                        on_new_line = TRUE;

                    }
                    else if (fctype == PLKR_TFC_GLYPH) {

                        if (fclen == 0) {

                            /* end of glyph run */
                            /* fprintf (stderr, "finished with glyph page %d\n", current_glyph_page); */
                            current_glyph_page = 0;

                        }
                        else if (fclen == 2) {

                            /* shift to different glyph page, possible start of glyph run */
                            int val = READ_BIGENDIAN_SHORT (ptr);
                            /* fprintf (stderr, "finished with glyph page %d, starting with %d\n", current_glyph_page, val); */
                            current_glyph_page = val;

                        }
                        else if (fclen == 4) {

                            /* adjust page position -- we'll ignore this for now */

                        }
                        else if (fclen == 6) {

                            /* start of glyph run */
                            current_glyph_page =
                                READ_BIGENDIAN_SHORT (ptr);
                            /* fprintf (stderr, "starting glyph page %d\n", current_glyph_page); */
                        }

                    }
                    else if (fctype == PLKR_TFC_IMAGE2) {

                        GdkPixbuf *image;

                        if (current_link) {

                            image =
                                GetImageRecord (doc,
                                                READ_BIGENDIAN_SHORT (ptr +
                                                                      2),
                                                scale);
                            gtk_text_buffer_insert_pixbuf (formatted_view,
                                                           &point, image);
                            InsertText (formatted_view, &point, " ", 1,
                                        charset);
                            on_new_line = FALSE;
                            first_text_of_paragraph = FALSE;
                            current_link_type = RecordLink;
                            current_link_image =
                                READ_BIGENDIAN_SHORT (ptr);

                        }
                        else {

                            GtkTextIter start_iter;
                            LinkType link_type;
                            int link_start, image_record;

                            link_type = RecordLink;
                            image_record = READ_BIGENDIAN_SHORT (ptr);
                            link_start = gtk_text_iter_get_offset (&point);
                            image =
                                GetImageRecord (doc,
                                                READ_BIGENDIAN_SHORT (ptr +
                                                                      2),
                                                scale);
                            gtk_text_buffer_insert_pixbuf (formatted_view,
                                                           &point, image);
                            on_new_line = FALSE;
                            first_text_of_paragraph = FALSE;
                            InsertText (formatted_view, &point, " ", 1,
                                        charset);
                            gtk_text_buffer_get_iter_at_offset
                                (formatted_view, &start_iter, link_start);
                            gtk_text_buffer_apply_tag_by_name
                                (formatted_view,
                                 link_type_names[link_type], &start_iter,
                                 &point);
                            PagelistAddLink (pages, id, link_type,
                                               link_start,
                                               gtk_text_iter_get_offset
                                               (&point), NULL,
                                               image_record, 0, 0);

                        }

                    }
                    else if (fctype == PLKR_TFC_TABLE) {

                        GdkPixbuf *image;
                        GtkTextChildAnchor *anchor;
                        GtkTextIter start_iter;
                        GtkWidget *table;
                        GtkRequisition requisition;
                        TABLE *tnode;

                        table =
                            RenderTable (doc, READ_BIGENDIAN_SHORT (ptr));
                        gtk_widget_show_all (table);
                        gtk_widget_size_request (table, &requisition);
//                              printf("Table width = %d\n", requisition.width);

                        if (requisition.width < 300) {
                            tnode = (TABLE *) malloc (sizeof (TABLE));
                            anchor =
                                gtk_text_buffer_create_child_anchor
                                (formatted_view, &point);
                            tnode->anchor = anchor;
                            tnode->table = READ_BIGENDIAN_SHORT (ptr);
                            page->tables =
                                g_slist_append (page->tables, tnode);
                        }
                        else {

                            GtkTextIter start_iter;
                            LinkType link_type;
                            int link_start, image_record;

                            link_type = RecordLink;
                            image_record = READ_BIGENDIAN_SHORT (ptr);
                            link_start = gtk_text_iter_get_offset (&point);
                            image =
                                gdk_pixbuf_from_pixdata
                                (&table_icon_pixdata, TRUE, NULL);
                            gtk_text_buffer_insert_pixbuf (formatted_view,
                                                           &point, image);
                            InsertText (formatted_view, &point, " ", 1,
                                        charset);
                            gtk_text_buffer_get_iter_at_offset
                                (formatted_view, &start_iter, link_start);
                            gtk_text_buffer_apply_tag_by_name
                                (formatted_view,
                                 link_type_names[link_type], &start_iter,
                                 &point);
                            PagelistAddLink (pages, id, link_type,
                                               link_start,
                                               gtk_text_iter_get_offset
                                               (&point), NULL,
                                               image_record, 0, 0);

                        }
                        on_new_line = FALSE;
                        first_text_of_paragraph = FALSE;

                    }
                    else {

                        fprintf (stderr,
                                 "unhandled function code %d encountered\n",
                                 fctype);
                    }

                    ptr += fclen;
                    run = ptr;

                }
                else if (current_glyph_page != 0) {

                    GdkPixbuf *image;

                    if (*ptr < 16 && *ptr > 0) {
                        /* vspace */
                        int width = (*ptr * ScreenDPI) / 75;
                        if (width >= 1) {
                            image = GetSpacerPixbuf (width);
                            if (!image)
                                g_warning
                                    ("Couldn't get %d-wide spacer pixmap",
                                     width);
                            else {
                                gtk_text_buffer_insert_pixbuf
                                    (formatted_view, &point, image);
                                on_new_line = FALSE;
                                first_text_of_paragraph = FALSE;
                            }
                        }
                    }
                    else if (*ptr > 15 && *ptr < 32) {
                        /* vadjust for following image -- no way to implement this yet */
                    }
                    else {
                        image =
                            GetGlyph (doc, current_glyph_page, (*ptr) - 32,
                                      ScreenDPI);
                        if (!image)
                            g_warning ("Couldn't get glyph %d/%d",
                                       current_glyph_page, (*ptr) - 32);
                        else {
                            gtk_text_buffer_insert_pixbuf (formatted_view,
                                                           &point, image);
                            on_new_line = FALSE;
                            first_text_of_paragraph = FALSE;
                        }
                    }
                    ptr++;
                    run = ptr;

                }
                else if (first_text_of_paragraph && isspace (*ptr)) {

                    /* leading whitespace -- just advance pointer */
                    ptr++;
                    run = ptr;

                }
                else {

                    /* regular text character */
                    ptr++;
                    first_text_of_paragraph = FALSE;

                }

            }

            /* at the end of the paragraph */
            if ((ptr - run) > 0) {

                if (!IsWhitespaceRun (run, (ptr - run))) {
                    InsertText (formatted_view, &point, run, (ptr - run),
                                charset);
                    /* fprintf (stderr, "para %d:  wrote text:  '%*.*s'\n", para_index,
                       (ptr-run), (ptr-run),  run);
                     */
                    on_new_line = FALSE;
                }

                run = ptr;
            }

            if (current_glyph_page > 0) {
                /* end of glyph run */
                /* fprintf (stderr, "finished with glyph page %d\n", current_glyph_page); */
                current_glyph_page = 0;
            }

            /* finish up any active formatting */
            while (active_formatting != NULL) {
                active_formatting = ApplyFormatting (active_formatting,
                                                     ((FormatStart *)
                                                      (active_formatting->
                                                       data))->tag_name,
                                                     &point);
            }

            /* put in a paragraph separator */
            if (UseUnicode) {

                InsertText (formatted_view, &point, UNICODE_PS_STRING, -1,
                            UTF8_CHARSET_MIBENUM);

            }
            else {

                if (!on_new_line) {
                    InsertText (formatted_view, &point, "\n", 1, charset);
                    on_new_line = TRUE;
                }

            }

        }

        gtk_text_buffer_get_bounds (formatted_view, &buffer_start,
                                    &buffer_end);
        gtk_text_buffer_apply_tag_by_name (formatted_view,
                                           "overall_attributes",
                                           &buffer_start, &buffer_end);

        page->formatted_view = formatted_view;
    }

    return page;
}


static void AddHruleToView
    (
    gpointer data,
    gpointer data2
    )
{
    GtkTextView *textview = (GtkTextView *) data2;
    GtkTextChildAnchor *anchor = (GtkTextChildAnchor *) data;
    GtkWidget *separator;

    separator = gtk_hseparator_new ();
    gtk_text_view_add_child_at_anchor (textview, separator, anchor);
    gtk_widget_set_size_request (separator, 70, -1);
    gtk_widget_show_all (separator);
}

static gboolean ShowRecord
    (
    plkr_Document * doc,
    int record_index,
    UI * ui,
    int offset
    );

static void TableCallback
    (
    GtkWidget * button,
    GdkEventButton * event,
    gpointer * user_data
    )
{
    guint link = GPOINTER_TO_INT
                     (g_object_get_data (G_OBJECT (button), "link"));
    UI *ui = (UI *) user_data;

    if (link > 0xFFFF) {
        GtkWidget *popup;
        GtkResponseType answer;

        popup =
            gtk_message_dialog_new (GTK_WINDOW (ui->window), 0,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    "View Large Image (instead of link)?");
        answer = gtk_dialog_run (GTK_DIALOG (popup));
        gtk_widget_destroy (popup);
        if (GTK_RESPONSE_YES == answer)
            ShowRecord (ui->doc, link >> 16, ui, 0);
        else
            ShowRecord (ui->doc, link & 0xFFFF, ui, 0);
    }

    else {
        ShowRecord (ui->doc, link, ui, 0);
    }
}


static void TableAddLinks
    (
    gpointer data,
    gpointer data2
    )
{
    GtkWidget *cell = (GtkWidget *) data;
    GList *list;
    int i, len, link;

    if (!GTK_IS_CONTAINER (cell))
        return;

    list = gtk_container_get_children (GTK_CONTAINER (cell));
    len = g_list_length (list);
    for (i = 0; i < len; i++) {
        if (GTK_IS_CONTAINER (g_list_nth_data (list, i))) {
            GList *list2;
            int i2, len2;
            list2 = gtk_container_get_children (GTK_CONTAINER
                                                (g_list_nth_data (list, i)));
            len2 = g_list_length (list2);
            for (i2 = 0; i2 < len2; i2++) {
                if (GTK_IS_CONTAINER (g_list_nth_data (list2, i2))) {
                    GList *list3;
                    int i3, len3;
                    list3 = gtk_container_get_children (GTK_CONTAINER
                                               (g_list_nth_data (list2, i2)));
                    len3 = g_list_length (list3);
                    for (i3 = 0; i3 < len3; i3++) {
                        GtkWidget *widget = g_list_nth_data (list3, i3);

                        if (GTK_IS_ALIGNMENT (widget))
                            widget = gtk_bin_get_child (GTK_BIN(widget));

                        link = GPOINTER_TO_UINT (g_object_get_data
                                                 (G_OBJECT (widget), "link"));

                        if (link) {
                            gtk_widget_add_events (widget, GDK_BUTTON_PRESS);
                            g_signal_connect (widget, "button_press_event",
                                              GTK_SIGNAL_FUNC (TableCallback),
                                              data2);
                        }
                    }
                }
            }
        }
    }
}


static void AddTableToView
    (
    gpointer data,
    gpointer data2
    )
{
    GtkTextView *textview = (GtkTextView *) ((UI *) data2)->textview;
    GtkTextChildAnchor *anchor =
        (GtkTextChildAnchor *) ((TABLE *) data)->anchor;
    plkr_Document *doc = ((UI *) data2)->doc;
    gint id = ((TABLE *) data)->table;
    GtkWidget *table = RenderTable (doc, id);

    gtk_container_foreach (GTK_CONTAINER (table),
                           (GtkCallback) TableAddLinks, data2);
    gtk_text_view_add_child_at_anchor (textview, table, anchor);
    gtk_widget_show_all (table);
}


static void AddHistoryNode
    (
    UI * ui,
    Page * page,
    int offset
    )
{
    HistoryNode *hnode;

    hnode = (HistoryNode *) malloc (sizeof (HistoryNode));
    hnode->page = page;
    hnode->offset = offset;
    ui->history = g_list_append (ui->history, hnode);
    ui->current_node = g_list_last (ui->history);
}

static gboolean ShowPage
    (
    UI * ui,
    Page * page,
    int offset
    )
{
    GtkWidget *separator;
    HistoryNode *hnode;
    GtkTextIter offset_iter;
    GtkTextMark *offset_mark;

    /* fprintf (stderr, "show page:  %d, %d\n", page->index, offset); */

    gtk_text_view_set_buffer (GTK_TEXT_VIEW (ui->textview),
                              page->formatted_view);

    g_slist_foreach (page->hrules, AddHruleToView, ui->textview);
    g_slist_foreach (page->tables, AddTableToView, ui);

    /* we want to do this right, so we need to use a GtkTextMark instead of
       a simple GtkTextIter, because using a mark apparently forces the calculation
       of actual line heights, etc. */
    gtk_text_buffer_get_iter_at_offset (page->formatted_view, &offset_iter,
                                        offset);
    offset_mark =
        gtk_text_buffer_create_mark (page->formatted_view, NULL,
                                     &offset_iter, TRUE);
    gtk_text_view_scroll_to_mark (ui->textview, offset_mark, 0.0, TRUE,
                                  0.0, 0.0);
    gtk_text_buffer_delete_mark (page->formatted_view, offset_mark);

    gtk_text_view_set_cursor_visible (ui->textview, FALSE);

    return TRUE;
}

static gboolean ShowRecord
    (
    plkr_Document * doc,
    int record_index,
    UI * ui,
    int offset
    )
{
    plkr_DataRecordType type;
    unsigned char *data;
    int data_len;
    gboolean status = TRUE;
    Page *page;

    type = plkr_GetRecordType (doc, record_index);

    if (type == PLKR_DRTYPE_TEXT_COMPRESSED || type == PLKR_DRTYPE_TEXT) {

        status = (page =
                  GetTextPage (ui->pages, doc, record_index, ui->scale))
            && ShowPage (ui, page, offset);
        if (status)
            AddHistoryNode (ui, page, offset);

    }
    else if (type == PLKR_DRTYPE_IMAGE_COMPRESSED
             || type == PLKR_DRTYPE_IMAGE
             || type == PLKR_DRTYPE_MULTIIMAGE) {

        status = (page =
                  GetImagePage (ui->pages, doc, record_index, ui->scale))
            && ShowPage (ui, page, offset);
        if (status)
            AddHistoryNode (ui, page, offset);

    }
    else if (type == PLKR_DRTYPE_TABLE_COMPRESSED
             || type == PLKR_DRTYPE_TABLE) {

        status = (page =
                  GetTablePage (ui->pages, doc, record_index, ui->scale))
            && ShowPage (ui, page, offset);
        if (status)
            AddHistoryNode (ui, page, offset);

    }
    else {

        fprintf (stderr, "Invalid record type %d for record %d\n", type,
                 record_index);
        status = FALSE;

    }

    return status;
}

static char *UrlEscapeText
    (
    char *text_to_escape
    )
{
    char *retval = g_strdup (text_to_escape);
    int i, j, inlen, outlen;
    char *ret;

    for (i = 0, j = 0, inlen = strlen (text_to_escape), outlen = inlen;
         i < inlen; i++) {
        if (g_ascii_isalnum (text_to_escape[i])) {
            retval[j] = text_to_escape[i];
            j += 1;
        }
        else {
            ret = (char *) malloc (outlen + 2 + 1);
            strncpy (ret, retval, j);
            sprintf (ret + j, "%%%02x", text_to_escape[i]);
            j += 3;
            free (retval);
            retval = ret;
            outlen += 2;
        }
    }
    retval[j] = 0;
    return retval;
}

static char *GetMailtoLinkAsUrl
    (
    plkr_Document * doc,
    int record_number
    )
{
    int to_offset, cc_offset, subject_offset, body_offset, url_size,
        record_bytes_len;
    unsigned char record_bytes[MAX_RECORD_SIZE];
    char *url, *old_url, *escaped_text;
    unsigned char *bytes;
    plkr_DataRecordType type;
    gboolean first = TRUE;


    record_bytes_len =
        plkr_CopyRecordBytes (doc, record_number, &record_bytes[0],
                              sizeof (record_bytes), &type);
    if (type != PLKR_DRTYPE_MAILTO)
        return NULL;

    bytes = &record_bytes[0] + 8;

    to_offset = (bytes[0] << 8) + bytes[1];
    cc_offset = (bytes[2] << 8) + bytes[3];
    subject_offset = (bytes[4] << 8) + bytes[5];
    body_offset = (bytes[6] << 8) + bytes[7];

    url = g_strdup ("mailto:");

    if (to_offset != 0) {
        old_url = url;
        escaped_text = UrlEscapeText (bytes + to_offset);
        url = g_strdup_printf ("%s%s", url, escaped_text);
        free (old_url);
        free (escaped_text);
    }

    if (cc_offset != 0) {
        old_url = url;
        escaped_text = UrlEscapeText (bytes + cc_offset);
        url =
            g_strdup_printf ("%s%scc=%s", url, (first ? "?" : "&"),
                             escaped_text);
        free (old_url);
        free (escaped_text);
        first = FALSE;
    }

    if (subject_offset != 0) {
        old_url = url;
        escaped_text = UrlEscapeText (bytes + subject_offset);
        url =
            g_strdup_printf ("%s%ssubject=%s", url, (first ? "?" : "&"),
                             escaped_text);
        free (old_url);
        free (escaped_text);
        first = FALSE;
    }

    if (body_offset != 0) {
        old_url = url;
        escaped_text = UrlEscapeText (bytes + body_offset);
        url =
            g_strdup_printf ("%s%sbody=%s", url, (first ? "?" : "&"),
                             escaped_text);
        free (old_url);
        free (escaped_text);
        first = FALSE;
    }

    return url;
}


static void FollowLink
    (
    Link * link,
    UI * ui
    )
{
    int id, para_offset;
    Page *page;
    plkr_Document *doc;
    GtkTextIter where;
    int buffer_x, buffer_y;
    GList *freednode;


    doc = GLIST_TO_HISTORY_NODE (ui->current_node)->page->doc;
    if (link->type == RecordLink || link->type == RecordParaLink) {
        /* free tail of history list -- taking a new branch */
        while ((freednode = ui->current_node->next) != NULL) {
            free (GLIST_TO_HISTORY_NODE (freednode));
            ui->history = g_list_remove_link (ui->history, freednode);
            g_list_free_1 (freednode);
        }
    }

    if (link->image) {
        GtkWidget *popup;
        GtkResponseType answer;

        popup =
            gtk_message_dialog_new (GTK_WINDOW (ui->window), 0,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    "View Large Image (instead of link)?");
        answer = gtk_dialog_run (GTK_DIALOG (popup));
        gtk_widget_destroy (popup);
        if (GTK_RESPONSE_YES == answer) {
            ShowRecord (doc, link->image, ui, 0);
            return;
        }
    }

    if (link->type == RecordLink) {

        /* fprintf (stderr, "Following link to record %d\n", link->record); */

        gtk_text_view_window_to_buffer_coords (ui->textview,
                                               GTK_TEXT_WINDOW_WIDGET, 0,
                                               0, &buffer_x, &buffer_y);
        gtk_text_view_get_iter_at_location (ui->textview, &where, buffer_x,
                                            buffer_y);
        GLIST_TO_HISTORY_NODE (ui->current_node)->offset =
            gtk_text_iter_get_offset (&where);
        ShowRecord (doc, link->record, ui, 0);

    }
    else if (link->type == RecordParaLink) {

        id = PagelistGetPageID (ui->pages, link->record);

        page = GetTextPage (ui->pages, doc, id, ui->scale);
        para_offset = GPOINTER_TO_UINT (g_tree_lookup (page->paragraphs,
                                                       GINT_TO_POINTER ((link->para << 16) + link->record)));

        /* fprintf (stderr, "Following link to record %d (%d), para %d; offset is %d\n", link->record, id, link->para, para_offset); */

        gtk_text_view_window_to_buffer_coords (ui->textview,
                                               GTK_TEXT_WINDOW_WIDGET, 0,
                                               0, &buffer_x, &buffer_y);
        gtk_text_view_get_iter_at_location (ui->textview, &where, buffer_x,
                                            buffer_y);
        GLIST_TO_HISTORY_NODE (ui->current_node)->offset =
            gtk_text_iter_get_offset (&where);

        ShowRecord (doc, id, ui, para_offset);

    }
    else if (link->type == UrlLink) {

        GtkWidget *popup;
        char *urltext;

        if (link->url)
            popup =
                gtk_message_dialog_new (GTK_WINDOW (ui->window), 0,
                                        GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                        "URL not included:\n%s",
                                        link->url);
        else
            popup =
                gtk_message_dialog_new (GTK_WINDOW (ui->window), 0,
                                        GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                        "No such record in document:  %d",
                                        link->record);
        (void) gtk_dialog_run (GTK_DIALOG (popup));
        gtk_widget_destroy (popup);

    }
    else if (link->type == MailtoLink) {

        GtkWidget *popup;
        char *urltext;

        urltext =
            GetMailtoLinkAsUrl (GLIST_TO_HISTORY_NODE
                                    (ui->current_node)->page->doc,
                                    link->record);
        popup =
            gtk_message_dialog_new (GTK_WINDOW (ui->window), 0,
                                    GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                    "'mailto' link to:\n%s", urltext);
        (void) gtk_dialog_run (GTK_DIALOG (popup));
        gtk_widget_destroy (popup);
        free (urltext);
    }

}

static void BackCallback
    (
    gpointer user_data
    )
{
    GList *previous;
    GtkTextIter where;
    int buffer_x, buffer_y;

    UI *ui = (UI *) user_data;

    if (previous = g_list_previous (ui->current_node)) {
        gtk_text_view_window_to_buffer_coords (ui->textview,
                                               GTK_TEXT_WINDOW_WIDGET, 0,
                                               0, &buffer_x, &buffer_y);
        gtk_text_view_get_iter_at_location (ui->textview, &where, buffer_x,
                                            buffer_y);
        GLIST_TO_HISTORY_NODE (ui->current_node)->offset =
            gtk_text_iter_get_offset (&where);
        ui->current_node = previous;
        ShowPage (ui, GLIST_TO_HISTORY_NODE (previous)->page,
                   GLIST_TO_HISTORY_NODE (previous)->offset);
    }
}

static void HomeCallback
    (
    gpointer user_data
    )
{
    UI *ui = (UI *) user_data;
    GtkTextIter where;
    int buffer_x, buffer_y;
    plkr_Document *doc;

    gtk_text_view_window_to_buffer_coords (ui->textview,
                                           GTK_TEXT_WINDOW_WIDGET, 0, 0,
                                           &buffer_x, &buffer_y);
    gtk_text_view_get_iter_at_location (ui->textview, &where, buffer_x,
                                        buffer_y);
    GLIST_TO_HISTORY_NODE (ui->current_node)->offset =
        gtk_text_iter_get_offset (&where);
    doc = GLIST_TO_HISTORY_NODE (ui->current_node)->page->doc;

    ShowRecord (doc, plkr_GetHomeRecordID (doc), ui, 0);
}

static void NextCallback
    (
    gpointer user_data
    )
{
    GList *next;
    GtkTextIter where;
    int buffer_x, buffer_y;
    UI *ui = (UI *) user_data;

    if (next = g_list_next (ui->current_node)) {
        gtk_text_view_window_to_buffer_coords (ui->textview,
                                               GTK_TEXT_WINDOW_WIDGET, 0,
                                               0, &buffer_x, &buffer_y);
        gtk_text_view_get_iter_at_location (ui->textview, &where, buffer_x,
                                            buffer_y);
        GLIST_TO_HISTORY_NODE (ui->current_node)->offset =
            gtk_text_iter_get_offset (&where);
        ui->current_node = next;
        ShowPage (ui, GLIST_TO_HISTORY_NODE (next)->page,
                   GLIST_TO_HISTORY_NODE (next)->offset);
    }
}


static void ReloadCallback
    (
    GtkButton * button,
    gpointer * user_data
    )
{
    UI *ui = (UI *) user_data;
    fprintf (stderr, "Reload:  %p\n", user_data);
}


static void QuitCallback
    (
    GtkButton * button,
    gpointer * user_data
    )
{
    /* fprintf (stderr, "Quit:  %p\n", user_data); */
    exit (0);
}


static void ScaleCallback
    (
    GtkAdjustment * adjustment,
    gpointer * user_data
    )
{
    UI *ui = (UI *) user_data;

    ui->scale = gtk_adjustment_get_value (adjustment);
}

static gboolean MouseClickCallback
    (
    GtkWidget * widget,
    GdkEventButton * event,
    gpointer user_data
    )
{
    gint buffer_x, buffer_y;
    GtkTextBuffer *buffer;
    GtkTextIter where, start, end;
    GSList *tags, *ptr;
    GtkTextTag *tag;
    int offset;
    Page *rnode;
    Link *link;
    UI *ui;

    /*
       fprintf (stderr, "State is %d, type is %d, button is %d, x is %f, y is %f\n",
       event->state, event->type, event->button, event->x, event->y);
     */

    ui = (UI *) user_data;
    rnode = GLIST_TO_HISTORY_NODE (ui->current_node)->page;
    gtk_text_view_window_to_buffer_coords (GTK_TEXT_VIEW (widget),
                                           GTK_TEXT_WINDOW_WIDGET,
                                           (gint) (event->x),
                                           (gint) (event->y), &buffer_x,
                                           &buffer_y);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
    gtk_text_view_get_iter_at_location (GTK_TEXT_VIEW (widget), &where,
                                        buffer_x, buffer_y);
    offset = gtk_text_iter_get_offset (&where);
/*
   fprintf (stderr, "Buffer 0x%p coords are %d, %d; offset is %d\n", buffer,
   buffer_x, buffer_y, offset);
 */
    link =
        (Link *) g_tree_search (rnode->links, LinkSearch,
                                GINT_TO_POINTER (offset));
    if (rnode->current_link == NULL && link
        && event->type == GDK_BUTTON_PRESS) {
        /* fprintf (stderr, "** Link of type %s, from %d to %d\n", link_type_names[link->type], link->start, link->end); */
        gtk_text_buffer_get_iter_at_offset (rnode->formatted_view, &start,
                                            link->start);
        gtk_text_buffer_get_iter_at_offset (rnode->formatted_view, &end,
                                            link->end);
        gtk_text_buffer_apply_tag_by_name (rnode->formatted_view,
                                           "selected-link", &start, &end);
        gtk_widget_queue_draw (widget);
        rnode->current_link = link;
    }
    else if (link && rnode->current_link == link
             && event->type == GDK_BUTTON_RELEASE) {
        /* fprintf (stderr, "== Invoke link ==\n"); */
        FollowLink (link, ui);
    }
    if (event->type == GDK_BUTTON_RELEASE && rnode->current_link) {
        gtk_text_buffer_get_iter_at_offset (rnode->formatted_view, &start,
                                            rnode->current_link->start);
        gtk_text_buffer_get_iter_at_offset (rnode->formatted_view, &end,
                                            rnode->current_link->end);
        gtk_text_buffer_remove_tag_by_name (rnode->formatted_view,
                                            "selected-link", &start, &end);
        rnode->current_link = NULL;
    }
    return FALSE;
}

static void UiFreeDocument
    (
    UI * ui
    )
{
}


static void UiShowDocument
    (
    UI * ui,
    char *docpath
    )
{
    plkr_Document *doc;
    GTree *tree;
    int i;

    if (ui->current_node)
        UiFreeDocument (ui);

    doc = plkr_OpenDBFile (docpath);
    if (!doc) {
        fprintf (stderr, "Error opening document %s\n", docpath);
        return;
    }

    i = plkr_GetHomeRecordID (doc);
    tree = g_tree_new (ComparePointerInts);
    PagelistAddRecord (tree, i);
    ui->pages = tree;
    ui->doc = doc;

    gtk_window_set_title (GTK_WINDOW (ui->window), plkr_GetName (doc));

    (void) ShowRecord (doc, i, ui, 0);
}


extern GtkWidget *CreateImageButton
    (
    char *,
    void (*)(gpointer),
    gpointer
    );

static gboolean AddLibraryElement
    (
    char *docname,
    DocInfo * info,
    GtkListStore * list
    )
{
    GtkTreeIter iter;

    gtk_list_store_append (list, &iter);
    gtk_list_store_set (list, &iter, 0,
                        (info->title ? info->title : docname), 1,
                        info->path, -1);

    return FALSE;
}

static void LibraryCallback
    (
    gpointer
    );
static UI *UiCreate ();

static void DocSelectionCallback
    (
    GtkTreeSelection * selection,
    gpointer user_data
    )
{
    UI *newui;
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *name, *path;

    if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
        gtk_tree_model_get (model, &iter, 0, &name, 1, &path, -1);
        /* fprintf (stderr, "Selection changed to %s, %s\n", name, path); */

        if (LibraryFirstSelection) {
            /* ignore first selection, which is automatic */
            LibraryFirstSelection = FALSE;
        }
        else {
            newui = UiCreate ();
            UiShowDocument (newui, path);
        }

        g_free (name);
        g_free (path);
    }
}

static void CloseLibrary
    (
    gpointer user_data
    )
{
    GtkWidget *window = (GtkWidget *) user_data;

    gtk_widget_hide_all (window);
}

static void LibraryCallback
    (
    gpointer user_data
    )
{
    UI *ui = (UI *) user_data;
    static GtkWidget *window = NULL;
    GtkScrolledWindow *sw;
    GtkListStore *list;
    GtkTreeIter iter;
    GtkTreeView *view;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkTreeSelection *select;
    static GtkWidget *vpaned, *buttons, *close_button, *quit_button;

    if (TheLibrary == NULL) {

        InitializeLibrary ();
        LibraryFirstSelection = TRUE;

        window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_window_set_default_size (GTK_WINDOW (window), 320, 480);
        gtk_window_set_title (GTK_WINDOW (window), "Document Library");

        g_signal_connect (window, "destroy",
                          G_CALLBACK (CloseLibrary), window);

        gtk_widget_set_name (window, "PluckerLibrary");
        gtk_container_set_border_width (GTK_CONTAINER (window), 0);

        /* the top bar is for buttons; the bottom is the view */
        vpaned = gtk_vpaned_new ();
        gtk_container_set_border_width (GTK_CONTAINER (vpaned), 5);
        gtk_container_add (GTK_CONTAINER (window), vpaned);

        buttons = gtk_hbox_new (FALSE, 4);

        quit_button = gtk_button_new_with_label ("Quit");
        g_signal_connect (GTK_OBJECT (quit_button), "clicked",
                          GTK_SIGNAL_FUNC (QuitCallback), ui);
        gtk_box_pack_end (GTK_BOX (buttons), quit_button, FALSE, TRUE, 0);

        close_button = gtk_button_new_with_label ("Close");
        g_signal_connect_swapped (GTK_OBJECT (close_button), "clicked",
                                  G_CALLBACK (CloseLibrary), window);
        gtk_box_pack_end (GTK_BOX (buttons), close_button, FALSE, TRUE, 0);

        list = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_STRING);
        EnumerateLibrary ((GTraverseFunc) AddLibraryElement, list);
        view = (GtkTreeView *)
            gtk_tree_view_new_with_model (GTK_TREE_MODEL (list));

        renderer = gtk_cell_renderer_text_new ();
        column = gtk_tree_view_column_new_with_attributes ("Document",
                                                           renderer,
                                                           "text", 0,
                                                           NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);

        renderer = gtk_cell_renderer_text_new ();
        column = gtk_tree_view_column_new_with_attributes ("Path",
                                                           renderer,
                                                           "text", 1,
                                                           NULL);
        gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);

        select = gtk_tree_view_get_selection (GTK_TREE_VIEW (view));
        gtk_tree_selection_set_mode (select, GTK_SELECTION_BROWSE);
        /* gtk_tree_selection_unselect_all (select); */
        g_signal_connect (G_OBJECT (select), "changed",
                          G_CALLBACK (DocSelectionCallback), ui);

        sw = GTK_SCROLLED_WINDOW (gtk_scrolled_window_new (NULL, NULL));
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);

        gtk_container_add (GTK_CONTAINER (sw), GTK_WIDGET (view));
        gtk_paned_add1 (GTK_PANED (vpaned), buttons);
        gtk_paned_add2 (GTK_PANED (vpaned), GTK_WIDGET (sw));
        gtk_widget_show_all (GTK_WIDGET (vpaned));

        if (!GTK_WIDGET_VISIBLE (window))
            gtk_widget_show (window);

        TheLibrary = window;

    }
    else {
        if (!GTK_WIDGET_VISIBLE (window))
            gtk_widget_show_all (window);

        /* fprintf (stderr, "Calling present on the library window"); */
        gtk_window_present (GTK_WINDOW (TheLibrary));

    }
}

static UI *UiCreate ()
{
    static GtkWidget *window = NULL;
    GtkWidget *vpaned;
    GtkWidget *view;
    GtkWidget *sw;
    GtkWidget *buttons, *minibuffer, *frame;
    GtkWidget *back_button, *quit_button, *next_button, *home_button,
        *reload_button, *library_button, *close_button;
    GtkTextTagTable *tags;
    GtkObject *adjustment;
    GtkWidget *hscale;
    UI *ui;

    InitializeIcons ();

    ui = (UI *) malloc (sizeof (UI));

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), 320, 480);

    g_signal_connect (window, "destroy",
                      G_CALLBACK (gtk_widget_destroyed), &window);

    gtk_widget_set_name (window, "PluckerView");
    gtk_container_set_border_width (GTK_CONTAINER (window), 0);

    /* the top bar is for buttons; the bottom is the view of the document */
    vpaned = gtk_vpaned_new ();
    gtk_container_set_border_width (GTK_CONTAINER (vpaned), 5);
    gtk_container_add (GTK_CONTAINER (window), vpaned);

    /* buttons = gtk_hbutton_box_new(); */
    buttons = gtk_hbox_new (FALSE, 4);

    back_button =
        CreateImageButton ("plucker-left-icon", &BackCallback, ui);
    gtk_box_pack_start (GTK_BOX (buttons), back_button, FALSE, TRUE, 0);
    /* gtk_container_add (GTK_CONTAINER (buttons), back_button); */

    home_button =
        CreateImageButton ("plucker-home-icon", &HomeCallback, ui);
    gtk_box_pack_start (GTK_BOX (buttons), home_button, FALSE, TRUE, 0);

    next_button =
        CreateImageButton ("plucker-right-icon", &NextCallback, ui);
    gtk_box_pack_start (GTK_BOX (buttons), next_button, FALSE, TRUE, 0);

    library_button =
        CreateImageButton ("plucker-library-icon", &LibraryCallback,
                             ui);
    gtk_box_pack_start (GTK_BOX (buttons), library_button, FALSE, TRUE,
                        10);

/*
   minibuffer = gtk_entry_new ();
   frame = gtk_frame_new(NULL);
   gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
   gtk_container_add (GTK_CONTAINER(frame), minibuffer);
   gtk_box_pack_start (GTK_BOX(buttons), frame, TRUE, TRUE, 0);
*/
    ui->scale = TheScale;
/*
   adjustment = gtk_adjustment_new (1.0, 0.0, 4.0, 0.1, 1.0, 0.0);
   gtk_signal_connect (GTK_OBJECT(adjustment), "value-changed",
   GTK_SIGNAL_FUNC(scale_callback), ui);
   hscale = gtk_hscale_new(GTK_ADJUSTMENT(adjustment));
   gtk_container_add (GTK_CONTAINER (buttons), hscale);

   reload_button = gtk_button_new_with_label("Reload");
   gtk_signal_connect (GTK_OBJECT(reload_button), "clicked",
   GTK_SIGNAL_FUNC(reload_callback), ui);
   gtk_container_add (GTK_CONTAINER (buttons), reload_button);
 */

    quit_button = gtk_button_new_with_label ("Quit");
    g_signal_connect (GTK_OBJECT (quit_button), "clicked",
                      GTK_SIGNAL_FUNC (QuitCallback), ui);
    gtk_box_pack_end (GTK_BOX (buttons), quit_button, FALSE, TRUE, 0);

    close_button = gtk_button_new_with_label ("Close");
    g_signal_connect_swapped (GTK_OBJECT (close_button), "clicked",
                              G_CALLBACK (gtk_widget_destroy), window);
    gtk_box_pack_end (GTK_BOX (buttons), close_button, FALSE, TRUE, 0);

    gtk_paned_add1 (GTK_PANED (vpaned), buttons);

    view = gtk_text_view_new ();

    sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                    GTK_POLICY_AUTOMATIC,
                                    GTK_POLICY_AUTOMATIC);
    gtk_paned_add2 (GTK_PANED (vpaned), sw);

    gtk_container_add (GTK_CONTAINER (sw), view);

    gtk_widget_show_all (vpaned);

    if (!GTK_WIDGET_VISIBLE (window))
        gtk_widget_show (window);

    ui->window = GTK_WINDOW (window);
    ui->textview = GTK_TEXT_VIEW (view);
    ui->current_node = NULL;
    ui->history = NULL;

    g_signal_connect (G_OBJECT (view), "button-press-event",
                      G_CALLBACK (MouseClickCallback), ui);
    g_signal_connect (G_OBJECT (view), "button-release-event",
                      G_CALLBACK (MouseClickCallback), ui);

    return ui;
}

static void InitializeCharsetNames ()
{
    /* The names here are from the Linux I18N work; see
       http://www.li18nux.org/docs/html/CodesetAliasTable-V10.html */

    CharsetNames = g_tree_new (ComparePointerInts);

    g_tree_insert (CharsetNames, GINT_TO_POINTER (3), "ISO-646-US");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (4), "ISO-8859-1");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (5), "ISO-8859-2");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (6), "ISO-8859-3");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (7), "ISO-8859-4");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (8), "ISO-8859-5");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (9), "ISO-8859-6");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (10), "ISO-8859-7");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (11), "ISO-8859-8");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (12), "ISO-8859-9");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (13), "ISO-8859-10");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (14), "ISO_6937-2-add");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (15), "JIS_X0201");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (16), "JIS_Encoding");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (17), "Shift_JIS");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (18), "EUC-JP");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (19),
                   "Extended_UNIX_Code_Fixed_Width_for_Japanese");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (20), "BS_4730");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (21), "SEN_850200_C");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (22), "IT");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (23), "ES");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (24), "DIN_66003");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (25), "NS_4551-1");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (26), "NF_Z_62-010");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (27), "ISO-10646-UTF-1");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (28),
                   "ISO_646.basic:1983");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (29), "INVARIANT");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (30), "ISO_646.irv:1983");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (31), "NATS-SEFI");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (32), "NATS-SEFI-ADD");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (33), "NATS-DANO");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (34), "NATS-DANO-ADD");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (35), "SEN_850200_B");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (36), "KS_C_5601-1987");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (37), "ISO-2022-KR");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (38), "EUC-KR");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (39), "ISO-2022-JP");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (40), "ISO-2022-JP-2");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (41),
                   "JIS_C6220-1969-jp");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (42),
                   "JIS_C6220-1969-ro");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (43), "PT");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (44), "greek7-old");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (45), "latin-greek");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (46),
                   "NF_Z_62-010_(1973)");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (47), "Latin-greek-1");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (48), "ISO_5427");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (49), "JIS_C6226-1978");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (50), "BS_viewdata");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (51), "INIS");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (52), "INIS-8");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (53), "INIS-cyrillic");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (54), "ISO_5427:1981");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (55), "ISO_5428:1980");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (56), "GB_1988-80");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (57), "GB_2312-80");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (58), "NS_4551-2");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (59), "videotex-suppl");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (60), "PT2");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (61), "ES2");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (62), "MSZ_7795.3");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (63), "JIS_C6226-1983");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (64), "greek7");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (65), "ASMO_449");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (66), "iso-ir-90");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (67), "JIS_C6229-1984-a");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (68), "JIS_C6229-1984-b");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (69),
                   "JIS_C6229-1984-b-add");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (70),
                   "JIS_C6229-1984-hand");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (71),
                   "JIS_C6229-1984-hand-add");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (72),
                   "JIS_C6229-1984-kana");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (73), "ISO_2033-1983");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (74), "ANSI_X3.110-1983");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (75), "T.61-7bit");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (76), "T.61-8bit");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (77), "ECMA-cyrillic");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (78),
                   "CSA_Z243.4-1985-1");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (79),
                   "CSA_Z243.4-1985-2");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (80),
                   "CSA_Z243.4-1985-gr");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (81), "ISO-8859-6-E");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (82), "ISO-8859-6-I");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (83), "T.101-G2");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (84), "ISO-8859-8-E");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (85), "ISO-8859-8-I");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (86), "CSN_369103");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (87), "JUS_I.B1.002");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (88), "IEC_P27-1");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (89),
                   "JUS_I.B1.003-serb");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (90), "JUS_I.B1.003-mac");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (91), "greek-ccitt");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (92), "NC_NC00-10:81");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (93), "ISO_6937-2-25");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (94), "GOST_19768-74");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (95), "ISO_8859-supp");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (96), "ISO_10367-box");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (97), "latin-lap");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (98), "JIS_X0212-1990");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (99), "DS_2089");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (100), "us-dk");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (101), "dk-us");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (102), "KSC5636");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (103),
                   "UNICODE-1-1-UTF-7");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (104), "ISO-2022-CN");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (105), "ISO-2022-CN-EXT");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (106), "UTF-8");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (109), "ISO-8859-13");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (110), "ISO-8859-14");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (111), "ISO-8859-15");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (112), "ISO-8859-16");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1000),
                   "ISO-10646-UCS-2");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1001),
                   "ISO-10646-UCS-4");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1002),
                   "ISO-10646-UCS-Basic");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1003),
                   "ISO-10646-Unicode-Latin1");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1005),
                   "ISO-Unicode-IBM-1261");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1006),
                   "ISO-Unicode-IBM-1268");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1007),
                   "ISO-Unicode-IBM-1276");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1008),
                   "ISO-Unicode-IBM-1264");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1009),
                   "ISO-Unicode-IBM-1265");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1010), "UNICODE-1-1");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1011), "SCSU");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1012), "UTF-7");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1013), "UTF-16BE");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1014), "UTF-16LE");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (1015), "UTF-16");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2000),
                   "ISO-8859-1-Windows-3.0-Latin-1");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2001),
                   "ISO-8859-1-Windows-3.1-Latin-1");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2002),
                   "ISO-8859-2-Windows-Latin-2");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2003),
                   "ISO-8859-9-Windows-Latin-5");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2004), "hp-roman8");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2005),
                   "Adobe-Standard-Encoding");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2006), "Ventura-US");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2007),
                   "Ventura-International");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2008), "DEC-MCS");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2009), "IBM850");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2010), "IBM852");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2011), "IBM437");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2012),
                   "PC8-Danish-Norwegian");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2013), "IBM862");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2014), "PC8-Turkish");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2015), "IBM-Symbols");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2016), "IBM-Thai");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2017), "HP-Legal");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2018), "HP-Pi-font");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2019), "HP-Math8");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2020),
                   "Adobe-Symbol-Encoding");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2021), "HP-DeskTop");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2022), "Ventura-Math");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2023),
                   "Microsoft-Publishing");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2024), "Windows-31J");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2025), "GB2312");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2026), "Big5");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2027), "macintosh");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2028), "IBM037");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2029), "IBM038");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2030), "IBM273");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2031), "IBM274");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2032), "IBM275");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2033), "IBM277");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2034), "IBM278");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2035), "IBM280");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2036), "IBM281");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2037), "IBM284");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2038), "IBM285");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2039), "IBM290");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2040), "IBM297");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2041), "IBM420");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2042), "IBM423");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2043), "IBM424");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2044), "IBM500");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2045), "IBM851");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2046), "IBM855");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2047), "IBM857");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2048), "IBM860");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2049), "IBM861");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2050), "IBM863");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2051), "IBM864");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2052), "IBM865");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2053), "IBM868");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2054), "IBM869");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2055), "IBM870");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2056), "IBM871");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2057), "IBM880");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2058), "IBM891");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2059), "IBM903");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2060), "IBM904");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2061), "IBM905");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2062), "IBM918");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2063), "IBM1026");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2064), "EBCDIC-AT-DE");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2065), "EBCDIC-AT-DE-A");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2066), "EBCDIC-CA-FR");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2067), "EBCDIC-DK-NO");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2068), "EBCDIC-DK-NO-A");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2069), "EBCDIC-FI-SE");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2070), "EBCDIC-FI-SE-A");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2071), "EBCDIC-FR");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2072), "EBCDIC-IT");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2073), "EBCDIC-PT");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2074), "EBCDIC-ES");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2075), "EBCDIC-ES-A");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2076), "EBCDIC-ES-S");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2077), "EBCDIC-UK");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2078), "EBCDIC-US");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2079), "UNKNOWN-8BIT");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2080), "MNEMONIC");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2081), "MNEM");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2082), "VISCII");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2083), "VIQR");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2084), "KOI8-R");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2085), "HZ-GB-2312");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2086), "IBM866");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2087), "IBM775");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2088), "KOI8-U");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2089), "IBM00858");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2090), "IBM00924");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2091), "IBM01140");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2092), "IBM01141");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2093), "IBM01142");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2094), "IBM01143");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2095), "IBM01144");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2096), "IBM01145");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2097), "IBM01146");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2098), "IBM01147");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2099), "IBM01148");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2100), "IBM01149");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2101), "Big5-HKSCS");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2250), "windows-1250");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2251), "windows-1251");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2252), "windows-1252");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2253), "windows-1253");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2254), "windows-1254");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2255), "windows-1255");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2256), "windows-1256");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2257), "windows-1257");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2258), "windows-1258");
    g_tree_insert (CharsetNames, GINT_TO_POINTER (2259), "TIS-620");
}


static void InitializeProperties ()
{
    if (Verbose)
        plkr_ShowMessages (1);

    if (ScreenDPI == 0) {
        ScreenDPI = gdk_screen_width () / (gdk_screen_width_mm () / 25.4);
    }
    /* fprintf (stderr, "dpi is %d\n", ScreenDPI); */
}


int main
    (
    int ac,
    char **av,
    char **envp
    )
{
    int i;
    struct stat buf;
    char *document_path = NULL;
    gboolean status;
    UI *ui;
    char *usage_format =
        "Usage:  %s [--unicode] [--dpi=N] [--verbose] [--scale=SCALE] [DOCUMENT-FILE]\n";

    TheScale = 1.0;
    UseUnicode = FALSE;

    gtk_init (&ac, &av);

    for (i = 1; i < ac; i++) {
        if (strncmp (av[i], "--scale=", 8) == 0) {
            TheScale = atof (av[i] + 8);
        }
        else if (strncmp (av[i], "--unicode", 12) == 0) {
            UseUnicode = TRUE;
        }
        else if (strncmp (av[i], "--verbose", 9) == 0) {
            Verbose = TRUE;
        }
        else if (strncmp (av[i], "--dpi=", 6) == 0) {
            ScreenDPI = atoi (av[i] + 6);
        }
        else if (strncmp (av[i], "--", 2) == 0) {
            fprintf (stderr, usage_format, av[0]);
            return 1;
        }
        else {
            break;
        }
    }

    if ((ac - i) == 1) {
        document_path = av[i];

        if (stat (document_path, &buf) != 0) {
            fprintf (stderr, "Can't access document %s\n", document_path);
            return 1;
        }
        else if (!S_ISREG (buf.st_mode)) {
            fprintf (stderr, "Document file %s must be a regular file.\n",
                     document_path);
            return 1;
        }
    }

    InitializeCharsetNames ();
    InitializeProperties ();


    if (document_path) {
        ui = UiCreate ();
        UiShowDocument (ui, document_path);
    }
    else
        LibraryCallback (ui);

    if (TheMainloop == NULL)
        TheMainloop = g_main_loop_new (NULL, TRUE);
    g_main_loop_run (TheMainloop);

    return 0;
}
