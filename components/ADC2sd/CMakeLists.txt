idf_component_register(SRCS "ADC2sd.c"
                    REQUIRES initSDspi initADC
                    PRIV_REQUIRES esp_adc
                    INCLUDE_DIRS "." "include")
