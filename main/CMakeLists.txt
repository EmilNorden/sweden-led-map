idf_component_register(SRCS "main.cpp" "is31fl3731.cpp" "solpos.c" "sun_position.cpp"
                       PRIV_REQUIRES spi_flash esp_driver_i2c
                       INCLUDE_DIRS "")
