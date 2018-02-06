#include "spibus.h"

#include <cerrno>               // errno
#include <fcntl.h>              // ::open() O_RDWR
#include <sys/ioctl.h>          // ioctl()
#include <linux/spi/spidev.h>   // spi_ioc_transfer SPI_IOC_MESSAGE
#include <stdexcept>            // std::runtime_error() <string>
#include <cstring>              // memset() strerror()



using namespace amex;


#define SPI_BASE_PATH "/dev/spidev" /* The general path to an SPI device */

spi::spi()
    : bus()
{

}

void spi::open(
        unsigned int bus, unsigned int device,
        mode_t mode /*= idle_high_cedge_rising*/,
        bit_justification_t bj /*= msb_first*/,
		uint32_t speed /*= 500000*/,
		uint8_t bpw /*= 8*/)
{
    m_bus = bus;
    m_device = device;
    std::string str = SPI_BASE_PATH + std::to_string(m_bus) + "." + std::to_string(m_device);

    if ( (m_file = ::open(str.c_str(), O_RDWR)) < 0 ) {
        std::string str("Failed to open SPI bus: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
	}

	this->setMode(mode);
	this->setBitJustification(bj);
	this->setSpeed(speed);
	this->setBitsPerWord(bpw);
}

void spi::setMode(mode_t mode)
{
    if (ioctl(m_file, SPI_IOC_WR_MODE, &mode) < 0 ) {
        std::string str("Failed to set SPI bus mode: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
	}

}

spi::mode_t spi::mode( void ) const
{
	mode_t mode;
    if (ioctl(m_file, SPI_IOC_RD_MODE, &mode) < 0 ) {
        std::string str("Failed to read SPI bus mode: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
	}
	return mode;
}

void spi::setBitJustification(bit_justification_t bj)
{
    uint8_t val = static_cast<uint8_t>(bj);
    if (ioctl(m_file, SPI_IOC_WR_LSB_FIRST, &val) < 0 ) {
        std::string str("Failed to set SPI bus bit justification: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
	}
}
spi::bit_justification_t spi::bitJustification( void ) const
{
	uint32_t val;
    if (ioctl(m_file, SPI_IOC_RD_LSB_FIRST, &val) < 0 ) {
        std::string str("Failed to read SPI bus bit justification: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
	}
    return val ? bit_justification_t::lsb_first : bit_justification_t::msb_first;
}

void spi::setSpeed(uint32_t speed) {
    if (ioctl(m_file, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0 ) {
        std::string str("Failed to set SPI bus max speed");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
	}
}

uint32_t spi::speed( void ) const {
	uint32_t speed;
    if (ioctl(m_file, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0 ) {
        std::string str("Failed to read SPI bus max speed: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
	}
	return speed;
}


void spi::setBitsPerWord(uint8_t bpw)
{
    if (ioctl(m_file, SPI_IOC_WR_BITS_PER_WORD, &bpw) < 0 ) {
        std::string str("Failed to set SPI bus bits per word: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
	}
}


uint8_t spi::bitsPerWord( void ) const
{
	uint8_t bpw;
    if (ioctl(m_file, SPI_IOC_RD_BITS_PER_WORD, &bpw) < 0 ) {
        std::string str("Failed to read SPI bus bits per word: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
	}
	return bpw;
}

void spi::transfer(uint8_t* send, uint8_t* receive, size_t length)
{

    struct spi_ioc_transfer transfer = {

        .tx_buf = reinterpret_cast<unsigned long>(send),
        .rx_buf = reinterpret_cast<unsigned long>(receive),

        .len = static_cast<__u32>(length),
        .speed_hz = this->speed(),

        /*
         * If nonzero, how long to delay after the last bit transfer before optionally
         * deselecting the device before the next transfer.
         */
        .delay_usecs = 0,
        /*
         * Temporary override of the device's wordsize.
         */
        .bits_per_word = this->bitsPerWord(),
        /*
         * True to deselect device before starting the next transfer.
         */
        .cs_change = 1,

        .tx_nbits = 0,
        .rx_nbits = 0,
        .pad = 0
    };

    if (ioctl(m_file, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        std::string str("Failed to transfer data to/from SPI bus: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
    }
}

void spi::transfer(uint16_t *send, uint16_t *receive, size_t length)
{
    struct spi_ioc_transfer transfer = {

        .tx_buf = reinterpret_cast<unsigned long>(send),
        .rx_buf = reinterpret_cast<unsigned long>(receive),

        .len = static_cast<__u32>(length),
        .speed_hz = this->speed(),

        /*
         * If nonzero, how long to delay after the last bit transfer before optionally
         * deselecting the device before the next transfer.
         */
        .delay_usecs = 0,
        /*
         * Temporary override of the device's wordsize.
         */
        .bits_per_word = this->bitsPerWord(),
        /*
         * True to deselect device before starting the next transfer.
         */
        .cs_change = 1,

        .tx_nbits = 0,
        .rx_nbits = 0,
        .pad = 0
    };

    if (ioctl(m_file, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        std::string str("Failed to transfer data to/from SPI bus: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
    }
}

void spi::transfer(uint32_t *send, uint32_t *receive, size_t length)
{
    struct spi_ioc_transfer transfer = {

        .tx_buf = reinterpret_cast<unsigned long>(send),
        .rx_buf = reinterpret_cast<unsigned long>(receive),

        .len = static_cast<__u32>(length),
        .speed_hz = this->speed(),

        /*
         * If nonzero, how long to delay after the last bit transfer before optionally
         * deselecting the device before the next transfer.
         */
        .delay_usecs = 0,
        /*
         * Temporary override of the device's wordsize.
         */
        .bits_per_word = this->bitsPerWord(),
        /*
         * True to deselect device before starting the next transfer.
         */
        .cs_change = 1,

        .tx_nbits = 0,
        .rx_nbits = 0,
        .pad = 0
    };
    if (ioctl(m_file, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        std::string str("Failed to transfer data to/from SPI bus: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
    }
}

uint8_t spi::transferByte(uint8_t send)
{
    uint8_t receive = 0;
    this->transfer(&send, &receive, 1);
    return receive;
}

uint16_t spi::transferHalfword(uint16_t send)
{
    uint16_t receive = 0;
    this->transfer(&send, &receive, 1);
    return receive;
}

uint32_t spi::transferWord(uint32_t send)
{
    uint32_t receive = 0;
    this->transfer(&send, &receive, 1);
    return receive;
}
