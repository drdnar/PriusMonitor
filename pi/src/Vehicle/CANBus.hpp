#ifndef CAN_BUS_INTERFACE_H
#define CAN_BUS_INTERFACE_H

#include <string>

namespace Vehicle
{

/**
 * Wraps a frame of CAN bus traffic.
 */
struct CANFrame
{
    static const unsigned int MaxLength = 8;
    union
    {
        /**
         * Data bytes
         */
        uint8_t Data[MaxLength];
        /**
         * Data, but as a single item.
         * BEWARE OF ENDIANNESS.
         */
        uint64_t Data64;
    };
    /**
     * CAN ID
     */
    uint_fast32_t ID;
    /**
     * Number of valid bytes of payload.
     */
    uint_fast8_t Length;
    /**
     * Extended frame format.
     * False for standard frame format.
     */
    bool ExtendedFormat;
    /**
     * Remote transmission request.
     */
    bool RemoteRequest;
    /**
     * Error frame.
     */
    bool Error;
};


/**
 * Wraps system calls for CAN bus access.
 */
class CANBus
{
    public:
        /**
         * Attempt to connect to can0.
         * If that fails, attempt to connect to vcan0.
         * If that fails, an exception is thrown.
         */
        CANBus();
        /**
         * Connect to a specific CAN interface.
         */
        CANBus(const char* name);
        ~CANBus();
        /**
         * Attempts to get a frame.
         * @param target Reference to a CANFrame to dump the data read into.
         * @return true on success, false if no data read (target is left in undefined state)
         */
        bool Receive(CANFrame& target);
        /**
         * Sends a frame.
         * @param data Reference to data to send.
         */
        bool Send(CANFrame& data);
    private:
        void attach(const char* name);
        /**
         * RAII for a socket.  The only role of this is to close the socket correctly.
         */
        struct Socket
        {
            /**
             * Socket number, garbage if created is false.
             */
            int soc;
            /**
             * Set to true once the socket is created.
             * If false, no socket exists yet!
             */
            bool created = false;
            /**
             * Create a socket.
             */
            void Create(int domain, int type, int protocol);
            ~Socket();
            /**
             * Get the actual wrapped socket value for system calls.
             */
            operator int() const { return soc; }
        } can_socket;
};

} /* namespace Vehicle */
#endif /* CAN_BUS_INTERFACE_H */
