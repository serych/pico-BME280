
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/clocks.h"
#define PLL_SYS_MHZ 50
#define LEDPIN 25

#define MISO 	16
#define CS		17
#define SCLK	18
#define MOSI	19

#define SPI_PORT spi0

uint16_t dig_T1;
int16_t dig_T2, dig_T3;

int32_t compTemp(int32_t adc_T)
{
	int32_t var1, var2, T;

	var1 = ((((adc_T >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2)) >> 11;
	var2 = (((((adc_T >>4) - ((int32_t) dig_T1)) * ((adc_T >> 4) - ((int32_t) dig_T1))) >>12) * ((int32_t) dig_T3)) >> 14;
	T = ((var1 + var2) * 5 + 128) >>8;
	return T;
}

void read_temp_comp()
{
	uint8_t buffer[6];
	uint8_t reg;

	reg = 0x88 | 0x80;
	gpio_put(CS, 0);
	spi_write_blocking(SPI_PORT, &reg, 1);
	spi_read_blocking(SPI_PORT, 0, buffer, 6);
	gpio_put(CS, 1);

	dig_T1 = buffer[0] | (buffer[1] << 8);
	dig_T2 = buffer[2] | (buffer[3] << 8);
	dig_T3 = buffer[4] | (buffer[5] << 8);
}

int main()
{
//////////////////// INIT	
	stdio_init_all();
	const uint32_t clkhz = PLL_SYS_MHZ * 1000000L;
    set_sys_clock_khz(clkhz / 1000L, true);

    clock_configure(clk_peri, 0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                    PLL_SYS_MHZ * MHZ,
                    PLL_SYS_MHZ * MHZ); 

	spi_init(SPI_PORT, 500000);
	
	gpio_set_function(MISO, GPIO_FUNC_SPI);
	gpio_set_function(SCLK, GPIO_FUNC_SPI);
	gpio_set_function(MOSI, GPIO_FUNC_SPI);

	gpio_init(CS);
	gpio_set_dir(CS, GPIO_OUT);
	gpio_put(CS, 1);

	gpio_init(LEDPIN);
	gpio_set_dir(LEDPIN, GPIO_OUT);

	read_temp_comp();

	uint8_t data[2];
	data[0] = 0xF4 & 0x7F;
	data[1] = 0x27;

	gpio_put(CS, 0);
	spi_write_blocking(SPI_PORT, data, 2);
	gpio_put(CS, 1);

//////////////////////////////////////
	int32_t raw_temp, temperature;
	uint8_t reg, buffer[3];
	while (1)
	{
		reg = 0xFA | 0x80;
		gpio_put(CS, 0);
		spi_write_blocking(SPI_PORT, &reg, 1);
		spi_read_blocking(SPI_PORT, 0, buffer, 3);
		gpio_put(CS, 1);

		raw_temp = ((uint32_t) buffer[0] << 12) | ((uint32_t) buffer[1] << 4) | ((uint32_t) buffer[2] >> 4);
		temperature = compTemp(raw_temp);
		printf("Teplota = %.2f °C\n", temperature / 100.00);

		gpio_put(LEDPIN, 1);
		sleep_ms(100);
		gpio_put(LEDPIN, 0);
		sleep_ms(900);

	}
}
