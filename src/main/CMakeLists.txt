
idf_component_register(SRCS "main.c" 
                            "actuators/WaterFlow/water_flow.c" "actuators/leds/leds.c" "actuators/Fan/fan.c"
                            "protocols/I2C_protocol.c" 
                            "wifi/wifi.c"
                            "thingsBoard/things_board.c"
                            "sensors/Humidity/humidity.c" "sensors/Temperature/temperature.c"
                    INCLUDE_DIRS "." 
                    )