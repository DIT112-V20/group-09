
#ifndef Wire_h
#define Wire_h

#include <array>
#include <cstdint>
#include <optional>
#include "Stream.h"

class TwoWire : public Stream {
    using OnRecieve = void(int);
    using OnRequest = void();
    constexpr static std::uint8_t no_address = 128;

    std::uint16_t bus_id = 0;
    bool begun{};
    std::uint8_t own_address = no_address;
    std::uint8_t slave_address = no_address;
    OnRecieve* user_onrecieve{};
    OnRequest* user_onrequest{};
    std::array<std::byte, 32> write_buf{};
    std::size_t write_buf_used{};
    bool truncated_buf{};

  public:
    void begin();
    void begin(std::uint8_t with_address);
    void begin(int sda, int scl);
    void begin(int sda, int scl, std::uint8_t with_address);
    std::size_t requestFrom(std::uint8_t slave_address, std::size_t quantity, bool stop = true);
    std::uint8_t requestFrom(std::uint8_t slave_address, std::uint8_t quantity, std::uint8_t);
    void beginTransmission(std::uint8_t slave_address);
    inline void beginTransmission(int slave_address) { beginTransmission(static_cast<std::uint8_t>(slave_address)); }
    std::uint8_t endTransmission(std::uint8_t = 0);
    std::size_t write(std::uint8_t value) override;
    std::size_t write(const std::uint8_t* buf, std::size_t buf_len) override;
    int available() override;
    int peek() override;
    int read() override;
    void setClock(std::uint32_t frequency);
    void onReceive(OnRecieve* hdl) noexcept;
    void onRequest(OnRequest* hdl) noexcept;
};

extern TwoWire Wire;

#endif // Wire_h
