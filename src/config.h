#ifndef ARXCLOCK_CONFIG_H
#define ARXCLOCK_CONFIG_H

#define ARXCLOCK_VERSION "1.1.9"

/* Phonon doesn't support static compilation, but it can be hacked.
 * This is one of the hacks. */
#ifdef ARXCLOCK_STATIC_PLUGINS
#  define PHONON_EXPORT
#endif

#ifdef _MSC_VER
#  define OVERRIDE override
#else
#  define OVERRIDE
#endif

#endif // ARXCLOCK_CONFIG_H
