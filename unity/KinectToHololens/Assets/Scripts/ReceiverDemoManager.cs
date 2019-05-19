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
    public Vp8Decoder decoder;
    public Texture yTexture;
    public Texture uTexture;
    public Texture vTexture;
    public Texture depthTexture;
    public MeshRenderer yQuad;
    public MeshRenderer uQuad;
    public MeshRenderer vQuad;
    public MeshRenderer colorQuad;
    public MeshRenderer depthQuad;

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
        print("interface: " + Plugin.has_unity_interfaces());
        PluginHelper.InitTextureGroup();
    }

    void Update()
    {
        const int COLOR_WIDTH = 960;
        const int COLOR_HEIGHT = 540;
        const int DEPTH_WIDTH = 512;
        const int DEPTH_HEIGHT = 424;
        if (yTexture == null)
        {
            if(Plugin.texture_group_get_y_texture_view().ToInt64() == 0)
                return;

            yTexture = Texture2D.CreateExternalTexture(COLOR_WIDTH,
                                                       COLOR_HEIGHT,
                                                       TextureFormat.R8,
                                                       false,
                                                       false,
                                                       Plugin.texture_group_get_y_texture_view());
            yQuad.material.mainTexture = yTexture;

            uTexture = Texture2D.CreateExternalTexture(COLOR_WIDTH / 2,
                                                       COLOR_HEIGHT / 2,
                                                       TextureFormat.R8,
                                                       false,
                                                       false,
                                                       Plugin.texture_group_get_u_texture_view());
            uQuad.material.mainTexture = uTexture;

            vTexture = Texture2D.CreateExternalTexture(COLOR_WIDTH / 2,
                                                       COLOR_HEIGHT / 2,
                                                       TextureFormat.R8,
                                                       false,
                                                       false,
                                                       Plugin.texture_group_get_v_texture_view());
            vQuad.material.mainTexture = vTexture;

            colorQuad.material.SetTexture("_YTex", yTexture);
            colorQuad.material.SetTexture("_UTex", uTexture);
            colorQuad.material.SetTexture("_VTex", vTexture);

            depthTexture = Texture2D.CreateExternalTexture(DEPTH_WIDTH,
                                                           DEPTH_HEIGHT,
                                                           TextureFormat.R16,
                                                           false,
                                                           false,
                                                           Plugin.texture_group_get_depth_texture_view());
            depthQuad.material.mainTexture = depthTexture;
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
            this.receiver = receiver;
            decoder = new Vp8Decoder();
        }
        else
        {
            UiVisibility = true;
        }
    }
}
