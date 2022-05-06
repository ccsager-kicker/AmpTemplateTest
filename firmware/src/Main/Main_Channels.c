/**
 * @company
 *  Stillwater Designs & Audio, Inc.
 * 
 * @filename
 *  Main_Channels.c
 * 
 * @author
 *  Luke Lau
 *  Ian Oswald
 */

/* Section: Included Files */
#include "Main.h"
#include <math.h>
#include "AmpDefs.h"

#define IS_ODD      1
/**
 * Initializes an array of channel objects (an amplifier) to their default
 * settings. All channels are assigned a channelType of FULL_RANGE unless
 * the 'channelCount' is odd and it is the last channel in the array, 
 * at which point it is assigned SUBWOOFER.
 * @param channels
 *  The array of channels to be initialized.
 * @param channelCount
 *  The number of channels to be initialized in the 'channels' array.
 */
void Main_ChannelInit(channel_t *channels, uint8_t channelCount) {
    
    uint8_t chNum;
    for (chNum = 0; chNum < channelCount; chNum++) {
        
        channels[chNum].channelNumber = chNum;
        /* If we're on the last channel and the number of channels is odd, make it a sub. */
        if (chNum == (channelCount - 1) && channelCount % 2 == IS_ODD) {
            channels[chNum].channelType = SUBWOOFER;
        }
        else {
            channels[chNum].channelType = FULL_RANGE;
        }
        /* Since the full range channels do not have independent settings,
            the ampIndex is the same as the channelNumber */
        channels[chNum].ampIndex = channels[chNum].channelNumber;
        switch(channels[chNum].channelType) {
            case FULL_RANGE:
                channels[chNum].channelMode = STEREO_LR;
                channels[chNum].gain.input = DEFAULT_GAINTRIM;
                channels[chNum].gain.output = DEFAULT_GAINTRIM;

                channels[chNum].highPass.xOverMode = HIGH_PASS;
                channels[chNum].highPass.frequency = DEFAULT_FR_HIPASS_FREQ;
                channels[chNum].highPass.alignment = LINKWITZ_RILEY;
                channels[chNum].highPass.slope = _24dB;
                channels[chNum].highPass.isEnabled = false;

                channels[chNum].lowPass.xOverMode = LOW_PASS;
                channels[chNum].lowPass.frequency = DEFAULT_FR_LOPASS_FREQ;
                channels[chNum].lowPass.alignment = LINKWITZ_RILEY;
                channels[chNum].lowPass.slope = _24dB;
                channels[chNum].lowPass.isEnabled = false;

                channels[chNum].delay.timeMs = DEFAULT_TIME_DELAY_MS;
                channels[chNum].delay.defeat = true;
                break;
            case SUBWOOFER:
                channels[chNum].channelMode = MONO_L_PLUS_R;
                channels[chNum].gain.input = DEFAULT_GAINTRIM;
                channels[chNum].gain.output = DEFAULT_GAINTRIM;

                channels[chNum].highPass.xOverMode = HIGH_PASS;
                channels[chNum].highPass.frequency = DEFAULT_SUB_HIPASS_FREQ;
                channels[chNum].highPass.alignment = LINKWITZ_RILEY;
                channels[chNum].highPass.slope = _24dB;
                channels[chNum].highPass.isEnabled = true;

                channels[chNum].lowPass.xOverMode = LOW_PASS;
                channels[chNum].lowPass.frequency = DEFAULT_SUB_LOPASS_FREQ;
                channels[chNum].lowPass.alignment = LINKWITZ_RILEY;
                channels[chNum].lowPass.slope = _24dB;
                channels[chNum].lowPass.isEnabled = true;

                channels[chNum].kickerEq.boost.frequency = DEFAULT_KICKEQ_FREQ;
                channels[chNum].kickerEq.boost.bandwidth = DEFAULT_KICKEQ_BW;
                channels[chNum].kickerEq.boost.gain = DEFAULT_KICKEQ_GAIN;
                channels[chNum].superBass.intensity = DEFAULT_KICKEQ_ENHANCEMENT;
                channels[chNum].delay.timeMs = DEFAULT_TIME_DELAY_MS;
                channels[chNum].delay.defeat = true;

                channels[chNum].outputPolarity = POLARITY_NORMAL;
            default:
                break;
        }
    }
}