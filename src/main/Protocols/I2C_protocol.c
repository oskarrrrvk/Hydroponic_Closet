#include "I2C_protocol.h"

void config_i2c_channel()
{
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_config));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0,i2c_config.mode,0,0,1));
}

int receive_i2c_channel(uint8_t address, int reading_number)//no me lee del sensor
{
    uint8_t command = 0x00;
    uint8_t buffer [reading_number];
    int out = 0;
  
    i2c_master_write_read_device(I2C_NUM_0,address,&command,1,buffer,reading_number,5000/portTICK_PERIOD_MS);
    out = buffer[0];
    for(int i = 1; i < reading_number; i++)
        out = (out << 8) | buffer[i];
    ESP_LOG_BUFFER_HEX("\tbuffer",buffer,reading_number);
    ESP_LOGI("\tout","%d",out);
    return out;
}

void show_i2c_info(char *tag, float data)
{
    ESP_LOGI(tag,"%.2f",data);
}
