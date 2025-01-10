#ifndef DISABLE_TRACE
#include <trace.h>
#include <uart.h>
#include <printf.h>
#include <stdbool.h>
#include <stdarg.h>

static bool initialized = false;

/* Initialize Trace */
void trace_init(void){
    // TODO : Add assert initialized
    if(!initialized){
        uart_init(); // Initialize UART
        initialized = true;
    }
}

/* Trace Function Implementation */
void trace(const char *format, ...){
    if(initialized){
        /* va comes from stdarg.h, it is used to handle
         * variable no of args. */

        // Sets up args list
        va_list args;

        // This initializes the va_list object (args) to start processing the variable arguments
        va_start(args, format);

        // It uses the args list (which was set up by va_start) to retrieve the actual arguments
        vprintf(format, args);

        // This cleans up the va_list object when you're done processing the variable arguments
        va_end(args);
    }
}

#endif



