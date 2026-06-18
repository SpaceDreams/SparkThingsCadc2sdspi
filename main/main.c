#include "ADC2SD.h"


static const char *TAG = "ADC_2_SD_testing";

void app_main(void)
{
    printf("ADC recording example start\n--------------------------------------\n");
    ESP_LOGI(TAG, "Starting the recording for %d seconds!", CONFIG_REC_TIME);
    const char *filename = "/record.wav";
    // Start Recording
    record_wav(CONFIG_REC_TIME, filename);
}