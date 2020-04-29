
#ifndef Wire_h
#define Wire_h

#include <cstdint>
#include <optional>
#include "BoardData.hxx"
#include "Stream.h"

class TwoWire : public Stream
{
  
public:
    void begin();
    void begin(uint8_t a);
    void begin(int a);
    size_t requestFrom(uint8_t address, size_t size, bool stop);
    uint8_t requestFrom(uint8_t, uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
    uint8_t requestFrom(int, int);
    uint8_t requestFrom(int, int, int);
    void beginTransmission(uint8_t);
    void beginTransmission(int);
    uint8_t endTransmission(void);
    uint8_t endTransmission(uint8_t);
    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t*, size_t);
    virtual int available(void);
    virtual int read(void);
    void setClock(uint32_t);
    void onReceive(void (*)(int));
    void onRequest(void (*)());

private:
    bool begun;
    uint8_t slaveAddress;
    uint8_t transmitted;
    uint8_t rxBufferIndex;
    uint8_t txBufferIndex;
    std::uint16_t bus_id;
    std::optional<uint8_t>transmissionAddress;
    void (*user_onRequest)(void);
    void (*user_onReceive)(int);
};

#endif // wire_h