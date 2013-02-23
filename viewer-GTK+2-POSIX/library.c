/* -*- mode: c; indent-tabs-mode: nil; c-indent-level: 4; -*-

 * $Id: library.c,v 1.3 2003/12/28 20:58:20 chrish Exp $
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

#include <stdlib.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <unpluck.h>

#include "gtkviewer.h"

#define PLKR_TIMEADJUST   2082848400    /* difference in seconds between
                                           Palm timebase of 12 AM Jan 1, 1904,
                                           and UNIX timebase of 12 AM Jan 1, 1970 */

#define PLUCKER_ID_STAMP        "DataPlkr"

#define READ_BIGENDIAN_SHORT(p) (((p)[0] << 8)|((p)[1]))
#define READ_BIGENDIAN_LONG(p)  (((p)[0] << 24)|((p)[1] << 16)|((p)[2] << 8)|((p)[3]))

static GTree *PluckerDocs = NULL;

static gboolean HasSuffix
    (
    char *main_string,
    char *potential_suffix
    )
{
    int l1 = strlen (main_string);
    int l2 = strlen (potential_suffix);

    return ((l1 >= l2)
            && (strncmp (main_string + (l1 - l2), potential_suffix, l2) ==
                0));
}

static char *IsPlucker
    (
    int fp
    )
{
    char filebuf[80];
    char *retval = NULL;

    if (fp >= 0) {
        if (read (fp, filebuf, 72) == 72) {
            if (strncmp (&filebuf[60], PLUCKER_ID_STAMP, 8) == 0) {
                retval = g_strdup (filebuf);
            }
        }
    }
    return retval;
}

void EnumerateLibrary
    (
    GTraverseFunc func,
    gpointer user_data
    )
{
    g_tree_foreach (PluckerDocs, func, user_data);
}

typedef enum {
    PLKR_HOME_NAME = 0,
    PLKR_URLS_INDEX_NAME = 2,
    PLKR_DEFAULT_CATEGORY_NAME = 3,
    PLKR_METADATA_NAME = 4,
} ReservedRecordName;

void GetMetadata
    (
    int fp,
    int *nrecords,
    GSList ** categories_out,
    int *charset_out,
    char **title_out,
    char **author_out,
    time_t * pubtime_out
    )
{
    unsigned char databuf[128];
    int nrecs, rec_offset, nreserved, i, type, id, nread, j,
        metadata_record_index = 0, categories_record_index =
        0, nrecord_headers, rec_length;
    unsigned long ctime;
    unsigned char record_offsets_data[6 * 8];
    GSList *categories = NULL;

    /* seek to the start of the record list to find the index record's offset */
    lseek (fp, 0, SEEK_SET);
    read (fp, databuf, 78);
    ctime = READ_BIGENDIAN_LONG (databuf + 36) - PLKR_TIMEADJUST;
    nrecs = READ_BIGENDIAN_SHORT (databuf + 76);
    nrecord_headers = MIN (6, nrecs);
    read (fp, record_offsets_data, nrecord_headers * 8);

    /* seek to the index record */
    rec_offset = READ_BIGENDIAN_LONG (record_offsets_data);
    lseek (fp, rec_offset, 0);
    read (fp, databuf, 128);
    nreserved = READ_BIGENDIAN_SHORT (databuf + 4);
    for (id = 0, i = 0; i < nreserved; i++) {
        type = READ_BIGENDIAN_SHORT (databuf + 8 + (i * 4));
        if (type == PLKR_DEFAULT_CATEGORY_NAME) {
            categories_record_index =
                READ_BIGENDIAN_SHORT (databuf + 6 + (i * 4));
        }
        else if (type == PLKR_METADATA_NAME) {
            metadata_record_index =
                READ_BIGENDIAN_SHORT (databuf + 6 + (i * 4));
        }
    }

    *nrecords = nrecs;
    *categories_out = NULL;
    *title_out = NULL;
    *author_out = NULL;
    *charset_out = 0;
    *pubtime_out = 0;

    /* seek to the default categories */
    if (categories_record_index > 0 || metadata_record_index > 0) {
        for (i = 1; i < MIN (5, nrecord_headers); i++) {
            rec_offset =
                READ_BIGENDIAN_LONG (record_offsets_data + (8 * i));
            rec_length =
                READ_BIGENDIAN_LONG (record_offsets_data + (8 * (i + 1))) -
                rec_offset;
            lseek (fp, rec_offset, 0);
            read (fp, databuf, 10);
            id = READ_BIGENDIAN_SHORT (databuf);
            type = *(databuf + 6);
            if (id == categories_record_index
                && type == PLKR_DRTYPE_CATEGORY) {
                char *ptr, *temp;
                j = rec_length - 8;
                temp = malloc (j);
                strncpy (temp, databuf + 8, 2);
                read (fp, temp + 2, j - 2);
                for (ptr = temp; ptr < (temp + j);) {
                    categories =
                        g_slist_prepend (categories, g_strdup (ptr));
                    ptr += strlen (ptr) + 1;
                }
                *categories_out = categories;
                free (temp);
                categories_record_index = 0;
                if (metadata_record_index == 0)
                    break;
            }
            else if (id == metadata_record_index
                     && type == PLKR_DRTYPE_METADATA) {
                int npieces = READ_BIGENDIAN_SHORT (databuf + 8);
                int datalen;
                for (j = 0; j < npieces; j++) {
                    read (fp, databuf, 4);
                    type = READ_BIGENDIAN_SHORT (databuf);
                    datalen = READ_BIGENDIAN_SHORT (databuf + 2) * 2;
                    if (type == 1) {    /* charset */
                        read (fp, databuf, 2);
                        *charset_out = READ_BIGENDIAN_SHORT (databuf);
                    }
                    else if (type == 4) {       /* author */
                        *author_out = malloc (datalen);
                        read (fp, *author_out, datalen);
                    }
                    else if (type == 5) {
                        *title_out = malloc (datalen);
                        read (fp, *title_out, datalen);
                    }
                    else if (type == 6) {
                        read (fp, databuf, 4);
                        *pubtime_out =
                            READ_BIGENDIAN_LONG (databuf) -
                            PLKR_TIMEADJUST;
                    }
                    else {
                        lseek (fp, datalen, SEEK_CUR);
                    }
                }
                metadata_record_index = 0;
                if (categories_record_index == 0)
                    break;
            }
        }
    }
}

void InitializeLibrary ()
{
    char *p;
    gchar **dirs;
    GDir *dir;
    gchar *fname, *path, *docname, *dirname, *homedir;
    int i, fp;
    DocInfo *docinfo;

    if (PluckerDocs != NULL)
        return;

    homedir = (char *) g_getenv ("HOME");

    PluckerDocs = g_tree_new ((GCompareFunc) strcmp);

    p = (char *) g_getenv ("PLUCKERPATH");
    if (p == NULL)
        p = plkr_GetConfigString (NULL, "pluckerpath", NULL);
    if (p == NULL)
        p = ".";

    if (p != NULL) {

        dirs = g_strsplit (p, ":", 0);

        for (i = 0; dirs[i] != NULL; i++) {

            dirname = dirs[i];

            /* handle ~ characters */
            if (dirname[0] == '~' && dirname[1] == G_DIR_SEPARATOR
                && homedir) {
                dirname =
                    g_strjoin (G_DIR_SEPARATOR_S, homedir, dirs[i] + 2,
                               NULL);
            }

            if (!g_file_test (dirname, G_FILE_TEST_EXISTS)) {
                fprintf (stderr,
                         "Non-existent path/file on PLUCKERPATH:  %s\n",
                         dirname);
                continue;
            }
            else if (g_file_test (dirname, G_FILE_TEST_IS_DIR)) {

                if (dir = g_dir_open (dirname, 0, NULL)) {

                    /* fprintf (stderr, "Looking in directory %s...\n", dirname); */

                    while ((fname =
                            (char *) g_dir_read_name (dir)) != NULL) {

                        if (HasSuffix (fname, ".plkr") ||
                            HasSuffix (fname, ".pdb")) {

                            path =
                                g_strjoin (G_DIR_SEPARATOR_S, dirname,
                                           fname, NULL);
                            fp = open (path, O_RDONLY);
                            if ((docname = IsPlucker (fp)) != NULL) {
                                /* OK, it's a plucker document, index it by document name */
                                /* fprintf (stderr, "    adding '%s' (%s)\n", filebuf, fname); */
                                docinfo =
                                    (DocInfo *) malloc (sizeof (DocInfo));
                                docinfo->name = docname;
                                docinfo->path = path;
                                GetMetadata (fp, &docinfo->nrecords,
                                              &docinfo->categories,
                                              &docinfo->charset,
                                              &docinfo->title,
                                              &docinfo->author,
                                              &docinfo->pubtime);
                                g_tree_insert (PluckerDocs, docinfo->name,
                                               docinfo);
                                path = NULL;
                            }
                            close (fp);
                            if (path)
                                free (path);
                        }
                    }

                    g_dir_close (dir);
                }
            }
            else if (g_file_test (dirname, G_FILE_TEST_EXISTS)) {
                fp = open (dirname, O_RDONLY);
                if ((docname = IsPlucker (fp)) != NULL) {
                    /* OK, it's a plucker document, index it by document name */
                    /* fprintf (stderr, "    adding '%s' (%s)\n", filebuf, fname); */
                    docinfo = (DocInfo *) malloc (sizeof (DocInfo));
                    docinfo->name = docname;
                    docinfo->path = g_strdup (dirname);
                    GetMetadata (fp, &docinfo->nrecords,
                                  &docinfo->categories,
                                  &docinfo->charset,
                                  &docinfo->title,
                                  &docinfo->author, &docinfo->pubtime);
                    g_tree_insert (PluckerDocs, docinfo->name, docinfo);
                }
                close (fp);
            }
            else {
                fprintf (stderr,
                         "Non-Plucker, non-directory element on PLUCKERPATH:  %s\n",
                         dirname);
            }

            if (dirname != dirs[i])
                free (dirname);
        }

        g_strfreev (dirs);
    }
}
