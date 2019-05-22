using System;
using System.Net;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.XR.WSA.Input;

public class HololensDemoManager : MonoBehaviour
{
    private enum InputState
    {
        IpAddress, Port
    }

    public Transform cameraTransform;
    public Transform scenceRootTransform;
    public TextMesh ipAddressText;
    public TextMesh ipAddressInputField;
    public TextMesh portText;
    public TextMesh portInputField;
    public TextMesh instructionText;
    public Material screenMaterial;
    public ScreenRenderer screenRenderer;

    private GestureRecognizer gestureRecognizer;
    private InputState inputState;
    private bool textureCreated;
    private Receiver receiver;
    private Vp8Decoder decoder;

    public TextMesh ActiveInputField
    {
        get
        {
            return inputState == InputState.IpAddress ? ipAddressInputField : portInputField;
        }
    }

    public bool UiVisibility
    {
        set
        {
            ipAddressText.gameObject.SetActive(value);
            ipAddressInputField.gameObject.SetActive(value);
            portText.gameObject.SetActive(value);
            portInputField.gameObject.SetActive(value);
            instructionText.gameObject.SetActive(value);
        }
    }

    void Awake()
    {
        gestureRecognizer = new GestureRecognizer();
        textureCreated = false;
        UiVisibility = true;
        SetInputState(InputState.IpAddress);
        PluginHelper.InitTextureGroup();

        gestureRecognizer.Tapped += OnTapped;
        gestureRecognizer.StartCapturingGestures();
    }

    void Update()
    {
        if(Input.GetKeyDown(KeyCode.Space))
        {
            ResetView();
        }

        AbsorbInput();

        if (!textureCreated)
        {
            if (Plugin.texture_group_get_y_texture_view().ToInt64() == 0)
                return;

            var textureGroup = new TextureGroup();
            screenMaterial.SetTexture("_YTex", textureGroup.YTexture);
            screenMaterial.SetTexture("_UTex", textureGroup.UTexture);
            screenMaterial.SetTexture("_VTex", textureGroup.VTexture);
            screenMaterial.SetTexture("_DepthTex", textureGroup.DepthTexture);
            textureCreated = true;
        }

        if (receiver == null)
            return;

        byte[] message;
        try
        {
            message = receiver.Receive();
        }
        catch (Exception e)
        {
            Debug.Log(e.Message);
            receiver = null;
            return;
        }

        if (message == null)
            return;

        if (message[0] == 0)
        {
            int cursor = 1;

            KinectColorIntrinsics colorIntrinsics;
            {
                float fx = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float fy = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float cx = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float cy = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float shiftD = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float shiftM = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float mxX3y0 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float mxX0y3 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float mxX2y1 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float mxX1y2 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float mxX2y0 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float mxX0y2 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float mxX1y1 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float mxX1y0 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float mxX0y1 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float mxX0y0 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float myX3y0 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float myX0y3 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float myX2y1 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float myX1y2 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float myX2y0 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float myX0y2 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float myX1y1 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float myX1y0 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float myX0y1 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float myX0y0 = BitConverter.ToSingle(message, cursor);
                cursor += 4;

                colorIntrinsics = new KinectColorIntrinsics(fx: fx,
                                                            fy: fy,
                                                            cx: cx,
                                                            cy: cy,
                                                            shiftD: shiftD,
                                                            shiftM: shiftM,
                                                            mxX3y0: mxX3y0,
                                                            mxX0y3: mxX0y3,
                                                            mxX2y1: mxX2y1,
                                                            mxX1y2: mxX1y2,
                                                            mxX2y0: mxX2y0,
                                                            mxX0y2: mxX0y2,
                                                            mxX1y1: mxX1y1,
                                                            mxX1y0: mxX1y0,
                                                            mxX0y1: mxX0y1,
                                                            mxX0y0: mxX0y0,
                                                            myX3y0: myX3y0,
                                                            myX0y3: myX0y3,
                                                            myX2y1: myX2y1,
                                                            myX1y2: myX1y2,
                                                            myX2y0: myX2y0,
                                                            myX0y2: myX0y2,
                                                            myX1y1: myX1y1,
                                                            myX1y0: myX1y0,
                                                            myX0y1: myX0y1,
                                                            myX0y0: myX0y0);
            }

            KinectIrIntrinsics irIntrinsics;
            {
                float fx = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float fy = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float cx = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float cy = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float k1 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float k2 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float k3 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float p1 = BitConverter.ToSingle(message, cursor);
                cursor += 4;
                float p2 = BitConverter.ToSingle(message, cursor);

                irIntrinsics = new KinectIrIntrinsics(fx: fx,
                                                      fy: fy,
                                                      cx: cx,
                                                      cy: cy,
                                                      k1: k1,
                                                      k2: k2,
                                                      k3: k3,
                                                      p1: p1,
                                                      p2: p2);
            }

            var kinectScreen = new KinectScreen(colorIntrinsics, irIntrinsics);
            screenRenderer.SetKinectScreen(kinectScreen);
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

    private void OnTapped(TappedEventArgs args)
    {
        ResetView();
    }

    private void ResetView()
    {
        scenceRootTransform.localPosition = cameraTransform.localPosition;
        scenceRootTransform.localRotation = cameraTransform.localRotation;
    }

    private void AbsorbInput()
    {
        if (Input.GetKeyDown(KeyCode.UpArrow) || Input.GetKeyDown(KeyCode.DownArrow) || Input.GetKeyDown(KeyCode.Tab))
        {
            SetInputState(inputState != InputState.IpAddress ? InputState.IpAddress : InputState.Port);
        }
        AbsorbKeyCode(KeyCode.Alpha0, '0');
        AbsorbKeyCode(KeyCode.Keypad0, '0');
        AbsorbKeyCode(KeyCode.Alpha1, '1');
        AbsorbKeyCode(KeyCode.Keypad1, '1');
        AbsorbKeyCode(KeyCode.Alpha2, '2');
        AbsorbKeyCode(KeyCode.Keypad2, '2');
        AbsorbKeyCode(KeyCode.Alpha3, '3');
        AbsorbKeyCode(KeyCode.Keypad3, '3');
        AbsorbKeyCode(KeyCode.Alpha4, '4');
        AbsorbKeyCode(KeyCode.Keypad4, '4');
        AbsorbKeyCode(KeyCode.Alpha5, '5');
        AbsorbKeyCode(KeyCode.Keypad5, '5');
        AbsorbKeyCode(KeyCode.Alpha6, '6');
        AbsorbKeyCode(KeyCode.Keypad6, '6');
        AbsorbKeyCode(KeyCode.Alpha7, '7');
        AbsorbKeyCode(KeyCode.Keypad7, '7');
        AbsorbKeyCode(KeyCode.Alpha8, '8');
        AbsorbKeyCode(KeyCode.Keypad8, '8');
        AbsorbKeyCode(KeyCode.Alpha9, '9');
        AbsorbKeyCode(KeyCode.Keypad9, '9');
        if (inputState == InputState.IpAddress)
        {
            AbsorbKeyCode(KeyCode.Period, '.');
            AbsorbKeyCode(KeyCode.KeypadPeriod, '.');
        }
        if(Input.GetKeyDown(KeyCode.Backspace))
        {
            var text = ActiveInputField.text;
            if(text.Length > 0)
            {
                ActiveInputField.text = text.Substring(0, text.Length - 1);
            }
        }
        if(Input.GetKeyDown(KeyCode.Return) || Input.GetKeyDown(KeyCode.KeypadEnter) || Input.GetKeyDown("enter"))
        {
            Connect();
        }
    }

    private void AbsorbKeyCode(KeyCode keyCode, char c)
    {
        if(Input.GetKeyDown(keyCode))
        {
            ActiveInputField.text += c;
        }
    }

    private async void Connect()
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
            //QuadVisibility = true;
            this.receiver = receiver;
            decoder = new Vp8Decoder();
        }
        else
        {
            UiVisibility = true;
        }
    }

    private void SetInputState(InputState inputState)
    {
        if(inputState == InputState.IpAddress)
        {
            ipAddressText.color = Color.yellow;
            portText.color = Color.white;
        }
        else
        {
            ipAddressText.color = Color.white;
            portText.color = Color.yellow;
        }

        this.inputState = inputState;
    }
}
