/* $Id: ppi.h,v 1.1 2004/01/24 16:43:32 chrish Exp $ */

#ifndef PPI_PPI_H
#define PPI_PPI_H

#include <PalmOS.h>

#include "../const.h"
#include "form.h"
#include "resourceids.h"

#define PpiAppID   'PlkP'
#define DA_RSRC_TYPE 'code'
#define DA_RSRC_ID 1000

typedef enum {
    TYPE_APP = 0,
    TYPE_DA,
    TYPE_CUSTOM
} AppType;

typedef enum {
    SELECT_WORD_NONE       = 0,
    SELECT_WORD_CUSTOM,
    SELECT_WORD_ROADLINGUA,
    SELECT_WORD_KDICDA
} LookupWordActionType;

typedef struct {
    Char       name[ dmDBNameLength ];
    AppType    type;
    UInt32     launchCmd;
    Boolean    toClipboard;
} LookupList;

#define PREFS_VERSION 2
#define PREFS_ID      0

/* PPI preferences */
typedef struct {
    LookupWordActionType lookupAction;
    Char       name[ dmDBNameLength ];
    Boolean    toClipboard;
    Boolean    isDA;
    UInt32     launchCmd;
} Preferences;

extern Preferences* Prefs ( void );
extern void WritePrefs( Preferences* prefs );

#endif
