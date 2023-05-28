#include "src/MFRC522.h"

#if defined(_WIN32)
#define MAKE_WIN_TEST
#endif

#ifndef MAKE_WIN_TEST
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include <iostream>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char* s)
{
	perror(s);
	abort();
}

static const char* device = "/dev/spidev0.0";
static uint8_t mode = 0;//[srg]
static uint8_t bits = 8;
static uint32_t speed = MFRC522_SPICLOCK;
static uint16_t delay_us = 0;//[srg]

int fd = 0;

MFRC522 mfrc522(SS_PIN);   // Create MFRC522 instance.
//MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

byte SPITransfer(byte val)
{
	uint8_t tx[1]{};
	uint8_t rx[1]{};

	tx[0] = val;
	//rx[0] = 0;
	//int ret;
	//uint8_t tx[] = {
	//	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	//	0x40, 0x00, 0x00, 0x00, 0x00, 0x95,
	//	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	//	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	//	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	//	0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD,
	//	0xF0, 0x0D,
	//};
	//uint8_t rx[ARRAY_SIZE(tx)] = { 0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.speed_hz = speed,
		.delay_usecs = delay_us,
		.bits_per_word = bits,
	};

	int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	//for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
	//	if (!(ret % 6))
	//		puts("");
	//	printf("%.2X ", rx[ret]);
	//}
	//puts("");

	return rx[0];
}

void loop()
{
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if (!mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}

int main()
{
	g_time_start = std::chrono::steady_clock::now();

	//parse_opts(argc, argv);

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	int ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);

	mfrc522.PCD_Init(); // Init MFRC522 card

	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

	while (true)
		loop();

	close(fd);

	return 0;
}
#else // MAKE_WIN_TEST
byte SPITransfer(byte val)
{
	return 0;
}

int main()
{
	g_time_start = std::chrono::steady_clock::now();
	delay(10);
	auto time = millis();

	return 0;
}
#endif // MAKE_WIN_TEST