using System;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using UnityEngine;

public class TcpSocket
{
    public bool Disposed { get; private set; }
    private Socket socket;

    public TcpSocket()
    {
        Disposed = false;
        socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp)
        {   
            Blocking = false
        };
    }

    public TcpSocket(Socket socket)
    {
        this.socket = socket;
        socket.Blocking = false;
    }

    public void Dispose()
    {
        if (!Disposed)
        {
            Disposed = true;
            try
            {
                socket.Shutdown(SocketShutdown.Both);
            }
            catch (SocketException e)
            {
                if (e.SocketErrorCode == SocketError.NotConnected)
                {
                    // When the socket is not connected, calling socket.Dispose results in a C++ exception in IL2CPP
                    // which can't be caught in a try-catch statement in C#.
                    // In brief, calling socket.Dispose() should be avoided.
                    Debug.Log("Failed to shutdown a TcpSocket since it was not connected...");
                    return;
                }
                else
                {
                    Debug.Log("SocketException from TcpSocket.Dispose calling Shutdown: " + e.SocketErrorCode);
                    throw e;
                }
            }
            // Removed socket.Dispose() since I it will be called sometime in the future (since Socket is an IDisposable),
            // and calling this led to a bug that crashes this application in HoloLens.
            //socket.Dispose();
        }
    }

    public TcpSocket Accept()
    {
        try
        {
            return new TcpSocket(socket.Accept());
        }
        catch (SocketException e)
        {
            if (e.SocketErrorCode == SocketError.WouldBlock)
            {
                return null;
            }
            else
            {
                throw e;
            }
        }
    }

    public Task<bool> ConnectAsync(IPAddress address, int port)
    {
        return ConnectAsync(new IPEndPoint(address, port));
    }

    // Returns true if connection succeeded.
    // Synchronous connection has a problem of always returning false.
    // It comes from the property of Socket.Connect when the socket is non-blocking.
    // reference: https://docs.microsoft.com/en-us/dotnet/api/system.net.sockets.socket.connect?view=netframework-4.7.2
    public async Task<bool> ConnectAsync(IPEndPoint ipEndPoint)
    {
        return await Task.Run(() => {
            var t = new TaskCompletionSource<bool>();
            SocketAsyncEventArgs args = new SocketAsyncEventArgs();
            args.RemoteEndPoint = ipEndPoint;
            args.Completed += (sender, e) => t.SetResult(args.ConnectSocket.Connected);
            try
            {
                socket.ConnectAsync(args);
            }
            catch (ObjectDisposedException e)
            {
                Debug.Log("Socket was disposed while ConnectAsync: " + e.Message);
            }
            return t.Task;
        });
    }

    public void Listen(int port)
    {
        socket.Bind(new IPEndPoint(IPAddress.Any, port));
        socket.Listen(10);
    }

    public Tuple<int, SocketError> Receive(byte[] buffer)
    {
        return Receive(buffer, 0, buffer.Length);
    }

    public Tuple<int, SocketError> Receive(byte[] buffer, int offset, int length)
    {
        SocketError socketError;
        int size = socket.Receive(buffer, offset, length, SocketFlags.None, out socketError);
        return new Tuple<int, SocketError>(size, socketError);
    }

    public Tuple<int, SocketError> Send(byte[] buffer)
    {
        SocketError socketError;
        int size = socket.Send(buffer, 0, buffer.Length, SocketFlags.None, out socketError);
        return new Tuple<int, SocketError>(size, socketError);
    }
}