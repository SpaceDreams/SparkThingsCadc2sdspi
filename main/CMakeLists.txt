idf_component_register(SRCS "ADC2sdspi.c"
                    REQUIRES initADC initSD
                    INCLUDE_DIRS "." "include")
