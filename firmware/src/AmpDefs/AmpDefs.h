/*
 Where we fix crap that is wrong
 */

#ifndef _AMPDEFS_H    /* Guard against multiple inclusion */
#define _AMPDEFS_H

#include "ChannelStructs.h"

//TODO: Set the channel count
#define CHANNEL_COUNT                   4

/* LIMITS */
    /* KICKER EQ */
#define KICKER_EQ_MAX_GAIN              6.0
#define KICKER_EQ_MIN_GAIN              0.0
#define KICKER_EQ_MAX_FREQ              80
#define KICKER_EQ_MIN_FREQ              20
#define KICKER_EQ_MAX_BW                5.0
#define KICKER_EQ_MIN_BW                1.0
    /* BASS ENHANCEMENT */
#define SUPERBASS_MIN                   0.0
#define SUPERBASS_MAX                   3.0
    /* GAIN */
#define MAX_GAIN_FR                     3.0
#define MIN_GAIN_FR                     -9.0
#define MAX_GAIN_SUB                    6.0
#define MIN_GAIN_SUB                    -9.0
    /* CROSSOVER */
#define MAX_HIGHPASS_FREQ_FR            5000
#define MIN_HIGHPASS_FREQ_FR            20
#define MAX_LOWPASS_FREQ_FR             5000
#define MIN_LOWPASS_FREQ_FR             40

#define MAX_HIGHPASS_FREQ_SUB           80
#define MIN_HIGHPASS_FREQ_SUB           10
#define MAX_LOWPASS_FREQ_SUB            160
#define MIN_LOWPASS_FREQ_SUB            40

/* DEFAULTS */
#define DEFAULT_GAINTRIM                0.0

#define DEFAULT_CH_MODE                 STEREO_LR
#define DEFAULT_TIME_DELAY_MS           0

#define DEFAULT_FR_HIPASS_FREQ          MIN_HIGHPASS_FREQ_FR
#define DEFAULT_FR_LOPASS_FREQ          MAX_LOWPASS_FREQ_FR

#define DEFAULT_SUB_HIPASS_FREQ         MIN_HIGHPASS_FREQ_SUB
#define DEFAULT_SUB_LOPASS_FREQ         MAX_LOWPASS_FREQ_SUB

#define DEFAULT_KICKEQ_FREQ             40
#define DEFAULT_KICKEQ_BW               2.0
#define DEFAULT_KICKEQ_GAIN             0.0
#define DEFAULT_KICKEQ_ENHANCEMENT      0.0

#define DEFAULT_SUB_PHASE_INV           POLARITY_NORMAL

/* TIMING */
#define AMP_START_DELAY_MS              1904    // To meet 3 sec startup timing.
#define PROTECT_RECOVER_RATE_MS         3000
#define SHUTDOWN_DELAY_MS               500

/* PROTECTION */
#define PSU_OVER_CURRENT_PROT_VOLT      1.4
#define UNDERVOLT_PROTECT_VOLT          6.5
#define UNDERVOLT_RECOVER_VOLT          7.0
#define UNDERVOLT_WARNING_VOLT          10.5
#define OVERVOLT_PROTECT_VOLT           16.5
#define OVERVOLT_RECOVER_VOLT           16.0
#define OVERVOLT_WARNING_VOLT           16.0

#define THERMAL_PROTECT_RECOVER_C       80.0
#define THERMAL_PROTECT_ACTIVATE_C      85.0

#define ROLLBACK_ACTIVATE_C             75.0
#define ROLLBACK_RATE_DB_C              2.0     //dB per degree C

#endif /* _AMPDEFS_H */

/* *****************************************************************************
 End of File
 */
