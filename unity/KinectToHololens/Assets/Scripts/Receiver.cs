using System.Net;
using System.Threading.Tasks;

public class Receiver
{
    private TcpSocket socket;
    private MessageBuffer messageBuffer;

    public Receiver()
    {
        socket = new TcpSocket();
        messageBuffer = new MessageBuffer();
    }

    public Task<bool> ConnectAsync(IPEndPoint ipEndPoint)
    {
        return socket.ConnectAsync(ipEndPoint);
    }

    public byte[] Receive()
    {
        return messageBuffer.Receive(socket);
    }
}
