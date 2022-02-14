template<typename F>
std::size_t TransSocket::recvMessage(char *data, size_t size, F scanForEnd)
{
    if (getSocketFD() == 0) {
        throw std::logic_error(buildErrorMessage("TransSocket::", __func__, ": accept called on a bad socket object (this object was moved)"));
    }

    std::size_t dataRead  = 0;
    while(dataRead < size)
    {
        // The inner loop handles interactions with the socket.
        std::size_t get = recv(getSocketFD(), data + dataRead, size - dataRead, 0);
        if (get == static_cast<std::size_t>(-1)) {
            switch(errno) {
                case EBADF:
                case EFAULT:
                case EINVAL:
                case ENXIO:
                {
                    // Fatal error. Programming bug
                    throw std::domain_error(buildErrorMessage("TransSocket::", __func__, ": read: critical error: ", strerror(errno)));
                }
                case EIO:
                case ENOBUFS:
                case ENOMEM:
                {
                   // Resource acquisition failure or device error
                    throw std::runtime_error(buildErrorMessage("TransSocket::", __func__, ": read: resource failure: ", strerror(errno)));
                }
                case EINTR:
                    // TODO: Check for user interrupt flags.
                    //       Beyond the scope of this project
                    //       so continue normal operations.
                case ETIMEDOUT:
                case EAGAIN:
                {
                    // Temporary error.
                    // Simply retry the read.
                    continue;
                }
                case ECONNRESET:
                case ENOTCONN:
                {
                    // Connection broken.
                    // Return the data we have available and exit
                    // as if the connection was closed correctly.
                    get = 0;
                    break;
                }
                default:
                {
                    throw std::runtime_error(buildErrorMessage("TransSocket::", __func__, ": read: returned -1: ", strerror(errno)));
                }
            }
        }
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

