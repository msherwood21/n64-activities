#include "log.h"

#include <stdint.h> // backtrace needs these definitions...
#include <backtrace.h>
#include <debug.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-
//- Definitions
//-

#ifdef DEBUG_EMULATE
#   define LOG_SRC DEBUG_FEATURE_LOG_ISVIEWER
#elif DEBUG_CART
#   define LOG_SRC DEBUG_FEATURE_LOG_SD
#elif NDEBUG
#   define LOG_SRC 0
#else
#   error "Debug configuration not selected. Where do I log?"
#endif

//-
//- Static data
//-

static bool Initialized = false;

//-
//- Public functions
//-

void LogInit(void) {
    if (Initialized) { return; }

    if (!debug_init(LOG_SRC) && (LOG_SRC != 0)) {
        //- Unrecoverable error. Abort.
        exit(1);
    }

    Initialized = true;
}

void Log(char * const msg, ...) {
    #ifndef NDEBUG
        //- Get function name
        //- NOTE: backtrace doesn't want to do less than three frames for some
        //      reason
        void * caller[2] = { NULL, NULL };
        char ** symbol;
        unsigned symbolSize = 0;

        int btSize = backtrace(caller, 2);
        if (btSize == 2) {
            symbol = backtrace_symbols(caller, btSize);
            if (symbol != NULL) {
                symbolSize += strlen(symbol[1]);
            }
        } else {
            debugf("btSize %d\n", btSize);
        }

        //- Paste together
        unsigned strSize = strlen(msg)
            + symbolSize
            + 1     // ' ' between the function and message
            + 1;    // null terminator
        char * concattedMsg = calloc(1, sizeof(char) * strSize);
        char * msgPos = concattedMsg;

        if (symbolSize > 0) {
            strcpy(msgPos, symbol[1]);
            msgPos += symbolSize;
            free(symbol);
        }
        memset(msgPos, ' ', 1);
        msgPos += 1;
        strcpy(msgPos, msg);

        //- Write
        va_list args;

        va_start(args, msg);
        vfprintf(stderr, concattedMsg, args);
        va_end(args);
    #endif
}
