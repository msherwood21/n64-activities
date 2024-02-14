/**
 * Provides logging functionality for all subsystems
 */
#ifndef LOG_H
#define LOG_H

//-
//- Definitions
//-

//-
//- Public functions
//-

//- Must be initialized before all other subsystems
void LogInit(void);

//- Logs to the console, SD card or is a noop depending on build configuration
void Log(char * const msg, ...);

#endif
