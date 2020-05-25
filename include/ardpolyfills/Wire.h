
#ifndef Wire_h
#define Wire_h

#include <array>
#include <cstdint>
#include <optional>
#include "Stream.h"

class TwoWire : public Stream {
    using OnRecieve = void(int);
    using OnRequest = void();
    ///Default value for having no address
    constexpr static std::uint8_t no_address = 128;

    ///Holds the value for the bus_id
    std::uint16_t bus_id = 0;
    ///Hold the value if the wire is Initiated
    bool begun{};
    ///Contains the master adress 
    std::uint8_t own_address = no_address;
    ///Adress of the slave
    std::uint8_t slave_address = no_address;
    ///Contains the onRecieve parameters function
    OnRecieve* user_onrecieve{};
    ///Contains the onReqeust parameters function
    OnRequest* user_onrequest{};
    ///Contains the written bytes
    std::array<std::byte, 32> write_buf{};
    ///Contains the the used buffer size
    std::size_t write_buf_used{};
    bool truncated_buf{};

  public:

    /**
    * Initiate the wire 
    **/
    void begin();
    
    /**
    *Initiate the wire a given adress
    **/
    void begin(std::uint8_t with_address);

    /**
    * Initiate the wire and sets the bus_id
    **/
    void begin(int sda, int scl);

    /**
    * Initiate the wire with a given adress and set the bus_id
    **/
    void begin(int sda, int scl, std::uint8_t with_address);

    /**
    * Request byte from a slave 
    **/
    std::size_t requestFrom(std::uint8_t slave_address, std::size_t quantity, bool stop = true);

    /**
    * Calls requestFrom(uint8_t, size_t, bool)
    **/
    std::uint8_t requestFrom(std::uint8_t slave_address, std::uint8_t quantity, std::uint8_t);

    /**
    * Calls requestFrom(uint8_t, size_t, bool)
    **/
    std::uint8_t requestFrom(int slave_address, int quantity, int stop = 0);

    /**
    * Begins a transmission with a given adress
    **/
    void beginTransmission(std::uint8_t slave_address);

    /**
    * Calls beginTransmission(uint8_t)
    **/
    inline void beginTransmission(int slave_address) { beginTransmission(static_cast<std::uint8_t>(slave_address)); }

    /**
    * Ends the transmission
    **/
    std::uint8_t endTransmission(std::uint8_t = 0);

    /**
    * Writes byte to write_buf
    **/
    std::size_t write(std::uint8_t value) override;

    /**
    * Writes byte of a given length
    **/
    std::size_t write(const std::uint8_t* buf, std::size_t buf_len) override;

    /**
    * Returns the avaiable bytes
    **/
    int available() override;

    /**
    * Returns the next byte at the front 
    **/
    int peek() override;

    /**
    * Reads the next byte and erases it
    **/
    int read() override;

    /**
    * Gets overridden
    **/
    void setClock(std::uint32_t frequency);

    /**
    * Function pointer that stores hdl in user_onrecieve
    **/
    void onReceive(OnRecieve* hdl) noexcept;

    /**
    * Function pointer that stores hdl in user_onrequest
    **/
    void onRequest(OnRequest* hdl) noexcept;
};

extern TwoWire Wire;

#endif // Wire_h
