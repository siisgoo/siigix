template<typename F>
std::size_t TransSocket::reciveBytes(char *data, std::size_t size, F scanForEnd)
{
    if (getSocketFD() == 0) {
        throw std::logic_error(eprintf("TransSocket::", __func__, ": accept called on a bad socket object (this object was moved)"));
    }

    std::size_t dataRead  = 0;
    while(dataRead < size)
    {
        // The inner loop handles interactions with the socket.
        std::size_t get = recv(getSocketFD(), data + dataRead, size - dataRead, 0);
        get = checkReciveState(get);
        if (get == 0) {
            break;
        }
        dataRead += get;
        if (scanForEnd(dataRead)) {
            break;
        }
    }

    return dataRead;
}
