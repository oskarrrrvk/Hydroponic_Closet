idf_component_register(SRCS "main.c" 
                            "actuators/WaterFlow/water_flow.c" "actuators/leds/leds.c"
                            "Protocols/I2C_protocol.c" 
                            "sensors/Humidity/humidity.c" "sensors/Temperature/temperature.c"
                    INCLUDE_DIRS ".")