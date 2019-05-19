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
    public MeshRenderer yQuad;
    public MeshRenderer uQuad;
    public MeshRenderer vQuad;
    public MeshRenderer colorQuad;
    public MeshRenderer depthQuad;

    private bool textureCreated;
    private Receiver receiver;
    private Vp8Decoder decoder;

    public bool UiVisibility
    {
        set
        {
            ipAddressInputField.gameObject.SetActive(value);
            portInputField.gameObject.SetActive(value);
            connectButton.gameObject.SetActive(value);
        }
    }

    public bool QuadVisibility
    {
        set
        {
            yQuad.gameObject.SetActive(value);
            uQuad.gameObject.SetActive(value);
            vQuad.gameObject.SetActive(value);
            colorQuad.gameObject.SetActive(value);
            depthQuad.gameObject.SetActive(value);
        }
    }

    void Awake()
    {
        UiVisibility = true;
        QuadVisibility = false;
        PluginHelper.InitTextureGroup();
    }

    void Update()
    {
        if (!textureCreated)
        {
            if(Plugin.texture_group_get_y_texture_view().ToInt64() == 0)
                return;

            var textureGroup = new TextureGroup();
            yQuad.material.mainTexture = textureGroup.YTexture;
            uQuad.material.mainTexture = textureGroup.UTexture;
            vQuad.material.mainTexture = textureGroup.VTexture;

            colorQuad.material.SetTexture("_YTex", textureGroup.YTexture);
            colorQuad.material.SetTexture("_UTex", textureGroup.UTexture);
            colorQuad.material.SetTexture("_VTex", textureGroup.VTexture);

            depthQuad.material.mainTexture = textureGroup.DepthTexture;
            textureCreated = true;
        }

        if (receiver == null)
            return;

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
            return;

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
            var ffmpegFrame = decoder.Decode(vp8FrameBytes, vp8FrameSize);
            Plugin.texture_group_set_ffmpeg_frame(ffmpegFrame.Ptr);
            Marshal.FreeHGlobal(vp8FrameBytes);
            cursor += vp8FrameSize;

            int rvlFrameSize = BitConverter.ToInt32(message, cursor);
            cursor += 4;

            IntPtr rvlFrameBytes = Marshal.AllocHGlobal(rvlFrameSize);
            Marshal.Copy(message, cursor, rvlFrameBytes, rvlFrameSize);
            Plugin.texture_group_set_rvl_frame(rvlFrameBytes, rvlFrameSize);
            Marshal.FreeHGlobal(rvlFrameBytes);
            cursor += rvlFrameSize;

            if (frameId % 100 == 0)
            {
                Debug.LogFormat("Received frame {0} (vp8FrameSize: {1}, rvlFrameSize: {2}).", frameId, vp8FrameSize, rvlFrameSize);
            }

            PluginHelper.UpdateTextureGroup();
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
            QuadVisibility = true;
            this.receiver = receiver;
            decoder = new Vp8Decoder();
        }
        else
        {
            UiVisibility = true;
        }
    }
}
