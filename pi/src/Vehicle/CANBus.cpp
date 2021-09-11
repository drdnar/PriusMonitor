#include "CANBus.hpp"
#include <unistd.h>
#include <string.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include <iostream>

using namespace Vehicle;

void CANBus::Socket::Create(int domain, int type, int protocol)
{
    soc = socket(domain, type, protocol);
    if (soc < 0)
        throw std::runtime_error("CANBus::Socket: failed to create");
    created = true;
}


CANBus::Socket::~Socket()
{
    if (created)
        close(soc);
}


CANBus::CANBus()
{
    can_socket.Create(PF_CAN, SOCK_RAW, CAN_RAW);
    try
    {
        attach("can0");
    }
    catch (std::runtime_error& ex)
    {
        attach("vcan0");
    }
}


CANBus::CANBus(const char* name)
{
    can_socket.Create(PF_CAN, SOCK_RAW, CAN_RAW);
    attach(name);
    struct can_frame x;
}


void CANBus::attach(const char* name)
{
    struct ifreq interface_request;
    if (strlen(name) >= sizeof(interface_request.ifr_name))
        throw std::runtime_error("CANBus::CANBus(const char*): interface name too long");
    strncpy(interface_request.ifr_name, name, sizeof(interface_request.ifr_name));
    if (ioctl(can_socket, SIOCGIFINDEX, &interface_request) < 0)
        throw std::runtime_error("CANBus::CANBus(const char*): ioctl failed");
    struct sockaddr_can can_address;
    can_address.can_family = AF_CAN;
    can_address.can_ifindex = interface_request.ifr_ifindex;
    //setsockopt(can_socket, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
    if (bind(can_socket, (struct sockaddr*)&can_address, sizeof(can_address)) < 0)
        throw std::runtime_error("CANBus::CANBus(const char*): bind failed");
}


bool CANBus::Receive(CANFrame& target)
{
    struct can_frame frame;
    auto r = read(can_socket, &frame, sizeof(struct can_frame));
    //if (r < 0)
    //    return false;
    if (r < sizeof(struct can_frame))
        return false;
    // Hopefully this gets turned into one or two native instructions. . . .
    target.Data64 = *((uint64_t*)&frame.data);
    target.ID = frame.can_id & CAN_ERR_MASK;
    target.Length = frame.can_dlc;
    target.Error = frame.can_id & CAN_ERR_FLAG;
    target.RemoteRequest = frame.can_id & CAN_RTR_FLAG;
    target.ExtendedFormat = frame.can_id & CAN_EFF_FLAG;
    return true;
}


bool CANBus::Send(CANFrame& data)
{
    struct can_frame frame;
    *((uint64_t*)&frame.data) = data.Data64;
    frame.can_id = data.ID
        | (data.Error ? CAN_ERR_FLAG : 0)
        | (data.RemoteRequest ? CAN_RTR_FLAG : 0)
        | (data.ExtendedFormat ? CAN_EFF_FLAG : 0);
    frame.can_dlc = data.Length;
    return write(can_socket, &frame, sizeof(struct can_frame)) == sizeof(struct can_frame);
}


CANBus::~CANBus()
{
    // do nothing probably
}
