idf_component_register(SRCS "main.cpp" "led_grid.cpp" "is31fl3731.cpp"
                       PRIV_REQUIRES spi_flash esp_driver_i2c
                       INCLUDE_DIRS "")
