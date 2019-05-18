using System;
using System.Net;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.UI;

public class ReceiverDemoManager : MonoBehaviour
{
    public InputField ipAddressInputField;
    public InputField portInputField;
    public Button connectButton;
    public Receiver receiver;

    public bool UiVisibility
    {
        set
        {
            ipAddressInputField.gameObject.SetActive(value);
            portInputField.gameObject.SetActive(value);
            connectButton.gameObject.SetActive(value);
        }
    }

    private void Awake()
    {
        print("interface: " + KinectToHololensPlugin.has_unity_interfaces());
    }

    void Update()
    {
        if (receiver == null)
        {
            return;
        }

        byte[] message;
        try
        {
            message = receiver.Receive();
        }
        catch(Exception e)
        {
            Debug.Log(e.Message);
            receiver = null;
            return;
        }

        if (message == null)
        {
            return;
        }

        if (message[0] == 0)
        {
            Debug.Log("Received intrinsics.");
        }
        else if (message[0] == 1)
        {
            int cursor = 1;
            int frameId = BitConverter.ToInt32(message, cursor);
            cursor += 4;

            int vp8FrameSize = BitConverter.ToInt32(message, cursor);
            cursor += 4;

            IntPtr vp8FrameBytes = Marshal.AllocHGlobal(vp8FrameSize);
            Marshal.Copy(message, cursor, vp8FrameBytes, vp8FrameSize);
            //var colorByteFrame = NativeByteFrame.Create(vp8FrameBytes, vp8FrameSize);
            Marshal.FreeHGlobal(vp8FrameBytes);
            cursor += vp8FrameSize;

            int rvlFrameSize = BitConverter.ToInt32(message, cursor);
            cursor += 4;

            IntPtr rvlFrameBytes = Marshal.AllocHGlobal(rvlFrameSize);
            Marshal.Copy(message, cursor, rvlFrameBytes, rvlFrameSize);
            //var depthByteFrame = NativeByteFrame.Create(rvlFrameBytes, rvlFrameSize);
            Marshal.FreeHGlobal(rvlFrameBytes);
            cursor += rvlFrameSize;

            if (frameId % 100 == 0)
            {
                Debug.LogFormat("Received frame {0} (vp8FrameSize: {1}, rvlFrameSize: {2}).", frameId, vp8FrameSize, rvlFrameSize);
            }
        }
    }

    public async void OnConnectButtonClicked()
    {
        UiVisibility = false;

        string ipAddress = ipAddressInputField.text;
        if (ipAddress.Length == 0)
            ipAddress = "127.0.0.1";

        string portString = portInputField.text;
        int port = portString.Length != 0 ? int.Parse(portString) : 7777;

        Debug.LogFormat("Try connecting to {0}:{1}.", ipAddress, port);
        var receiver = new Receiver();
        if (await receiver.ConnectAsync(new IPEndPoint(IPAddress.Parse(ipAddress), port)))
        {
            this.receiver = receiver;
        }
        else
        {
            UiVisibility = true;
        }
    }
}
