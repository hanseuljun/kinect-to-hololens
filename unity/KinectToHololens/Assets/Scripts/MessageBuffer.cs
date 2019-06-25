using System;
using System.Net.Sockets;

// A buffer class for receiving messages over TCP.
public class MessageBuffer
{
    // The bytes for the actual message's size.
    private byte[] sizeBytes;
    private int sizeCursor;
    // The bytes of the actual message.
    private byte[] messageBytes;
    private int messageCursor;

    public MessageBuffer()
    {
        sizeBytes = new byte[4];
        sizeCursor = 0;
        messageBytes = null;
        messageCursor = 0;
    }

    // Try receiving a message from the tcpSocket.
    // Return the message if when succeeded to receive a whole message.
    // Return null if not.
    public byte[] Receive(TcpSocket tcpSocket)
    {
        // Try receiving the size of the actual message.
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

        // Try receiving the bytes of the actual message.
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