using System;
using System.IO;
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

    public void Send(int frameId)
    {
        int packetSize = 5;
        var ms = new MemoryStream();
        ms.Write(BitConverter.GetBytes(packetSize), 0, 4);
        ms.WriteByte(0);
        ms.Write(BitConverter.GetBytes(frameId), 0, 4);
        socket.Send(ms.ToArray());
    }
}
