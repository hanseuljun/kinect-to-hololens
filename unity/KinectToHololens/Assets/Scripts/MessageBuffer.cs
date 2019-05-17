using System;
using System.Net.Sockets;

public class MessageBuffer
{
    private byte[] sizeBytes;
    private int sizeCursor;
    private byte[] messageBytes;
    private int messageCursor;

    public MessageBuffer()
    {
        sizeBytes = new byte[4];
        sizeCursor = 0;
        messageBytes = null;
        messageCursor = 0;
    }

    public byte[] Receive(TcpSocket tcpSocket)
    {
        if (sizeCursor < sizeBytes.Length)
        {
            var sizeResult = tcpSocket.Receive(sizeBytes, sizeCursor, sizeBytes.Length - sizeCursor);
            var sizeError = sizeResult.Item2;
            if (!(sizeError == SocketError.Success || sizeError == SocketError.WouldBlock))
            {
                throw new Exception("Error receiving message size: " + sizeError);
            }
            else
            {
                sizeCursor += sizeResult.Item1;
            }

            if (sizeCursor == sizeBytes.Length)
            {
                int packetSize = BitConverter.ToInt32(sizeBytes, 0);
                messageBytes = new byte[packetSize];
            }
            else
            {
                return null;
            }
        }

        var messageResult = tcpSocket.Receive(messageBytes, messageCursor, messageBytes.Length - messageCursor);
        var messageError = messageResult.Item2;
        if (!(messageError == SocketError.Success || messageError == SocketError.WouldBlock))
        {
            throw new Exception("Error receiving message: " + messageError);
        }
        else
        {
            messageCursor += messageResult.Item1;
        }

        // If message wasn't not received completety, try it again next time.
        if (messageCursor < messageBytes.Length)
            return null;

        sizeCursor = 0;
        messageCursor = 0;
        return messageBytes;
    }
}