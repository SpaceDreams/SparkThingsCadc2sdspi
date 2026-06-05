#include "initADC.h"
#include "initSD.h"
#include "format_wav.h"

static const char *TAG = "ADC_rec_example";
#define BYTE_RATE init_ADC_SAMPLING_FREQ*init_ADC_BIT_WIDTH/8

void record_wav(uint32_t rec_time)
{

    //SD setup
    // Use POSIX and C standard library functions to work with files.
    int flash_wr_size = 0;
    ESP_LOGI(TAG, "Opening file");

    uint32_t flash_rec_time = BYTE_RATE * rec_time;
    const wav_header_t wav_header =
        WAV_HEADER_PCM_DEFAULT(flash_rec_time, 16, CONFIG_INIT_SAMPLE_RATE, 1);

    // First check if file exists before creating a new file.
    struct stat st;
    if (stat(SD_MOUNT_POINT"/record.wav", &st) == 0) {
        // Delete it if it exists
        unlink(SD_MOUNT_POINT"/record.wav");
    }

    // Create new WAV file
    FILE *f = fopen(SD_MOUNT_POINT"/record.wav", "a");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    // Write the header to the WAV file
    fwrite(&wav_header, sizeof(wav_header), 1, f);
    
    //ADC setup
    esp_err_t ret;
    uint32_t ret_num = 0;
    uint8_t result[init_READ_LEN] = {0};
    memset(result, 0xcc, init_READ_LEN);
    adc_continuous_handle_t handle = NULL;
    s_task_handle = xTaskGetCurrentTaskHandle();
    continuous_adc_init(channel, sizeof(channel) / sizeof(adc_channel_t), &handle);
    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));
    ESP_ERROR_CHECK(adc_continuous_start(handle));

    // Start recording
    while (flash_wr_size < flash_rec_time) {
        /**
         * This is to show you the way to use the ADC continuous mode driver event callback.
         * This `ulTaskNotifyTake` will block when the data processing in the task is fast.
         * However in this example, the data processing (print) is slow, so you barely block here.
         *
         * Without using this event callback (to notify this task), you can still just call
         * `adc_continuous_read()` here in a loop, with/without a certain block timeout.
         */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // Read the RAW samples from the microphone
        ret = adc_continuous_read(handle, result, init_READ_LEN, &ret_num, 0);
        while(ret==ESP_OK){
            adc_continuous_data_t parsed_data[ret_num / SOC_ADC_DIGI_RESULT_BYTES];
            uint32_t num_parsed_samples = 0;
            esp_err_t parse_ret = adc_continuous_parse_data(handle, result, ret_num, parsed_data, &num_parsed_samples);
            if(parse_ret==ESP_OK){
                uint16_t buff[num_parsed_samples];
                for (int i = 0; i < num_parsed_samples; i++) {
                        if (parsed_data[i].valid) {
                            //
                            buff[i]=parsed_data[i].raw_data;
                        } else {
                            ESP_LOGW(TAG, "Invalid data [ADC%d_Ch%d_%"PRIu32"]",
                                     parsed_data[i].unit + 1,
                                     parsed_data[i].channel,
                                     parsed_data[i].raw_data);
                        }
                    }
                    // Write the samples to the WAV file
                    fwrite(buff, sizeof(buff), 1, f);
                    flash_wr_size += sizeof(buff);
                    fflush(f);
            } else{
                ESP_LOGE(TAG, "Data parsing failed: %s", esp_err_to_name(parse_ret));
            }
        }
        if(ret == ESP_ERR_TIMEOUT){
            ESP_LOGE(TAG, "ADC Timeout Error");
            //We try to read `init_READ_LEN` until API returns timeout, which means there's no available data
            break;
        }
    }
    ESP_LOGI(TAG, "Recording done!");
    fclose(f);
    ESP_LOGI(TAG, "File written on SDCard");
    // All done, Turn off ADC
    ESP_ERROR_CHECK(adc_continuous_stop(handle));
    ESP_ERROR_CHECK(adc_continuous_deinit(handle));
    // All done, unmount partition and disable SPI peripheral
    esp_vfs_fat_sdcard_unmount(SD_MOUNT_POINT, card);
    ESP_LOGI(TAG, "Card unmounted");
    // Deinitialize the bus after all devices are removed
    spi_bus_free(host.slot);
}

void app_main(void)
{
    printf("ADC recording example start\n--------------------------------------\n");
    // Mount the SDCard for recording the audio file
    mount_sdcard();
    ESP_LOGI(TAG, "Starting the recording for %d seconds!", CONFIG_REC_TIME);
    // Start Recording
    record_wav(CONFIG_REC_TIME);
}