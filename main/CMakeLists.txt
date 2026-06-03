idf_component_register(SRCS "ADC2sdspi.c"
                    PRIV_REQUIRES esp_adc fatfs
                    INCLUDE_DIRS "." "include")
