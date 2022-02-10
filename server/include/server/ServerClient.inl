template<class ProtocolC>
template<class CliProtocolC>
Server<ProtocolC>::Client<CliProtocolC>::Client(TransSocket&& sock) :
    _socket(std::move(sock)),
        _proto(_socket)
{
    _socket.EnableKeepAlive();
}

template<class ProtocolC>
template<class CliProtocolC>
Server<ProtocolC>::Client<CliProtocolC>::~Client()
{
    this->disconnect();
}

template<class ProtocolC>
template<class CliProtocolC>
void
Server<ProtocolC>::Client<CliProtocolC>::sendMessage(const std::string& data)
{
    _proto.sendMessage(data);
}

template<class ProtocolC>
template<class CliProtocolC>
void
Server<ProtocolC>::Client<CliProtocolC>::recvMessage(std::string& data)
{
    _proto.recvMessage(data);

    if (data.size() <= 0) {
        disconnect();
    }
}

template<class ProtocolC>
template<class CliProtocolC>
int
Server<ProtocolC>::Client<CliProtocolC>::disconnect()
{
    _status = ClientStatus::disconnected;
    if (!_socket.isValid()) {
        return _status;
    }
    _socket.Close();
    return _status;
}
