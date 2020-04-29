#include "Wire.h"
#include "Arduino.h"
#include "BoardDataDef.hxx"

void TwoWire::begin()
{
    begun = true;
}

void TwoWire::begin(uint8_t address)
{
    slaveAddress = address;
    begun = true;
}
void TwoWire::begin(int address)
{
    slaveAddress = address;
    begun = true;
}

// requestFrom = REQUEST_SEND
size_t TwoWire::requestFrom(uint8_t address, size_t size, bool stop)
{
    std::lock_guard lock{board_data->i2c_buses[bus_id].tx_mutex};

    ProtocolRequest req = ProtocolRequest{ REQUEST_SEND, size };
    transmissionAddress = address;
    board_data->i2c_buses[bus_id].requests[address].push_back(req);
    return req.bytes;
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t size)
{
    return requestFrom(address, static_cast<size_t>(size), true);
}

uint8_t TwoWire::requestFrom(uint8_t address, uint8_t size, uint8_t stop)
{
    return requestFrom(address, static_cast<size_t>(size), static_cast<bool>(stop));
}

uint8_t TwoWire::requestFrom(int address, int size)
{
    return requestFrom(static_cast<uint8_t>(address), static_cast<size_t>(size), true);
}

uint8_t TwoWire::requestFrom(int address, int size, int stop)
{
    return requestFrom(static_cast<uint8_t>(address), static_cast<size_t>(size),static_cast<bool>(stop));
}

void TwoWire::beginTransmission(uint8_t adress)
{
    txBufferIndex = 0;
    // set address of targeted slave    
    transmissionAddress = adress;
}
void TwoWire::beginTransmission(int adress)
{   
    beginTransmission((uint8_t)(adress));  
}

uint8_t TwoWire::endTransmission()
{
    return endTransmission(true); 
}
uint8_t TwoWire::endTransmission(uint8_t)
{
    if (transmitted > 0)
    {
        transmissionAddress.reset();
        txBufferIndex = 0;
        //0 = sucess transmission
        return 0;
    }
    transmissionAddress.reset();
    //Error at transmission
    return 1;
};

// REQUEST_RECEIVE
size_t TwoWire::write(uint8_t value)
{
    std::lock_guard lock{board_data->i2c_buses[bus_id].tx_mutex};
    std::lock_guard lock{board_data->i2c_buses[transmissionAddress.value()].tx_mutex};

    if (transmissionAddress)
    { 
        ProtocolRequest req = ProtocolRequest{ REQUEST_RECEIVE, value };
        board_data->i2c_buses[bus_id].requests[transmissionAddress.value()].push_back(req);

        // put byte in tx buffer
        board_data->i2c_buses[transmissionAddress.value()].tx[txBufferIndex] = ((std::byte)value); 
        txBufferIndex++;
        transmitted = value;    
        // update amount in buffer   
        board_data->i2c_buses[transmissionAddress.value()].tx_used_size = txBufferIndex;
        return transmitted;
    }   
    return 0;
}

size_t TwoWire::write(const uint8_t* data, size_t quantity)
{
    std::lock_guard lock{board_data->i2c_buses[bus_id].tx_mutex};
    std::lock_guard lock{board_data->i2c_buses[transmissionAddress.value()].tx_mutex};

    size_t bytes;

    if (transmissionAddress)
    {
        ProtocolRequest protoReq = ProtocolRequest{ REQUEST_RECEIVE, quantity };
        board_data->i2c_buses[bus_id].requests[transmissionAddress.value()].push_back(protoReq);
        bytes = protoReq.bytes;
        board_data->i2c_buses[transmissionAddress.value()].tx[txBufferIndex] = (std::byte)bytes;
        txBufferIndex++;
        return quantity;    
    }
    return 0;
}

int TwoWire::available()
{ 
    std::lock_guard lock{board_data->i2c_buses[transmissionAddress.value()].tx_mutex};
    return board_data->i2c_buses[transmissionAddress.value()].tx_used_size;   
}

int TwoWire::read()
{
    std::lock_guard lock{board_data->i2c_buses[transmissionAddress.value()].tx_mutex};
    std::byte read;

    size_t bufferLength = board_data->i2c_buses[transmissionAddress.value()].tx_used_size;
    if (txBufferIndex < bufferLength)
    {
        read = board_data->i2c_buses[transmissionAddress.value()].tx[txBufferIndex];
        txBufferIndex++;
    }
   return (int)read;
}

void setClock(uint32_t);    

//called when data is received
void TwoWire::onReceive(void (*function)(int))  
{
    TwoWire::user_onReceive = function;
}

//called when data is requested
void TwoWire::onRequest(void (*function)())
{
    TwoWire::user_onRequest = function;
}
