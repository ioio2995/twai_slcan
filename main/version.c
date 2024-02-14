#include "sdkconfig.h"

/** \brief Software version string.
 *
 *  This variable holds the software version information, and it is set to the Git reference.
 */
const char * software_version_str = GIT_REFERENCE;

/** \brief Hardware version string.
 *
 *  This variable holds the hardware version information, and it is set to the IDF target configuration.
 */
const char * hardware_version_str = CONFIG_IDF_TARGET;
