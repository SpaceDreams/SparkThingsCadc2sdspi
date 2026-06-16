idf_component_register(SRCS "ADC2sdspi.c"
                    REQUIRES fatfs initSD initADC
                    PRIV_REQUIRES esp_adc
                    INCLUDE_DIRS "." "include")
