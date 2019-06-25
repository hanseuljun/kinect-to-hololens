using System;
using System.Net;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.XR.WSA.Input;

// The main script for the HololensDemo scene.
public class HololensDemoManager : MonoBehaviour
{
    private enum InputState
    {
        IpAddress, Port
    }

    // The main camera's Transform.
    public Transform cameraTransform;
    // The TextMesh placed above user's head.
    public TextMesh statusText;
    // The root of the scene that includes everything else except the main camera.
    // This provides a convenient way to place everything in front of the camera.
    public Transform scenceRootTransform;
    // TextMeshes for the UI.
    public TextMesh ipAddressText;
    public TextMesh ipAddressInputField;
    public TextMesh portText;
    public TextMesh portInputField;
    public TextMesh instructionText;
    // For rendering the Kinect pixels in 3D.
    public Material screenMaterial;
    public ScreenRenderer screenRenderer;

    // To recognize when the user taps.
    private GestureRecognizer gestureRecognizer;
    // Varaibles that represent states of the scene.
    private InputState inputState;
    private bool textureCreated;
    // The Receiver which receives Kinect data over the network.
    private Receiver receiver;
    // Decodes Kinect frames that were encoded before being sent over the network.
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

        // Prepare a GestureRecognizer to recognize taps.
        gestureRecognizer.Tapped += OnTapped;
        gestureRecognizer.StartCapturingGestures();

        statusText.text = "Waiting for user input.";
    }

    void Update()
    {
        // Space key resets the scene to be placed in front of the camera.
        if(Input.GetKeyDown(KeyCode.Space))
        {
            ResetView();
        }

        // Sends virtual keyboards strokes to the TextMeshes for the IP address and the port.
        AbsorbInput();

        // If texture is not created, create and assign them to quads.
        if(!textureCreated)
        {
            // Check whether the native plugin has Direct3D textures that
            // can be connected to Unity textures.
            if(Plugin.texture_group_get_y_texture_view().ToInt64() == 0)
                return;

            // TextureGroup includes Y, U, V, and a depth texture.
            var textureGroup = new TextureGroup();
            screenMaterial.SetTexture("_YTex", textureGroup.YTexture);
            screenMaterial.SetTexture("_UTex", textureGroup.UTexture);
            screenMaterial.SetTexture("_VTex", textureGroup.VTexture);
            screenMaterial.SetTexture("_DepthTex", textureGroup.DepthTexture);
            textureCreated = true;
        }

        // Do not continue if there is no Receiever connected to a Sender.
        if(receiver == null)
            return;

        // Try receiving a message.
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

        // Continue only if there is a message.
        if(message == null)
            return;

        // Prepare the ScreenRenderer with the received KinectIntrinsics.
        if(message[0] == 0)
        {
            var kinectScreen = CreateKinectScreenFromIntrinsicsMessage(message);
            screenRenderer.SetKinectScreen(kinectScreen);
        }
        // When a Kinect frame got received.
        else if (message[0] == 1)
        {
            int cursor = 1;
            int frameId = BitConverter.ToInt32(message, cursor);
            cursor += 4;

            // Notice the Sender that the frame was received through the Receiver.
            receiver.Send(frameId);

            int vp8FrameSize = BitConverter.ToInt32(message, cursor);
            cursor += 4;

            // Marshal.AllocHGlobal, Marshal.Copy, and Marshal.FreeHGlobal are like
            // malloc, memcpy, and free of C.
            // This is required since vp8FrameBytes gets sent to a Vp8Decoder
            // inside the native plugin.
            IntPtr vp8FrameBytes = Marshal.AllocHGlobal(vp8FrameSize);
            Marshal.Copy(message, cursor, vp8FrameBytes, vp8FrameSize);
            var ffmpegFrame = decoder.Decode(vp8FrameBytes, vp8FrameSize);
            Plugin.texture_group_set_ffmpeg_frame(ffmpegFrame.Ptr);
            Marshal.FreeHGlobal(vp8FrameBytes);
            cursor += vp8FrameSize;

            int rvlFrameSize = BitConverter.ToInt32(message, cursor);
            cursor += 4;

            // Marshal.AllocHGlobal, Marshal.Copy, and Marshal.FreeHGlobal are like
            // malloc, memcpy, and free of C.
            // This is required since rvlFrameBytes gets sent to the native plugin.
            IntPtr rvlFrameBytes = Marshal.AllocHGlobal(rvlFrameSize);
            Marshal.Copy(message, cursor, rvlFrameBytes, rvlFrameSize);
            Plugin.texture_group_set_rvl_frame(rvlFrameBytes, rvlFrameSize);
            Marshal.FreeHGlobal(rvlFrameBytes);

            if (frameId % 100 == 0)
            {
                string logString = string.Format("Received frame {0} (vp8FrameSize: {1}, rvlFrameSize: {2}).", frameId, vp8FrameSize, rvlFrameSize);
                Debug.Log(logString);
                statusText.text = logString;
            }

            // Invokes a function to be called in a render thread.
            PluginHelper.UpdateTextureGroup();
        }
    }

    private void OnTapped(TappedEventArgs args)
    {
        // Place the scene in front of the camera when the user taps.
        ResetView();
    }

    // Places everything in front of the camera by positing and turning a root transform for
    // everything else except the camera.
    private void ResetView()
    {
        scenceRootTransform.localPosition = cameraTransform.localPosition;
        scenceRootTransform.localRotation = cameraTransform.localRotation;
    }

    // Sends keystrokes of the virtual keyboard to TextMeshes.
    // Try connecting the Receiver to a Sender when the user pressed the enter key.
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

    // A helper method for AbsorbInput().
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

        // The default IP address is 127.0.0.1.
        string ipAddress = ipAddressInputField.text;
        if (ipAddress.Length == 0)
            ipAddress = "127.0.0.1";

        // The default port is 7777.
        string portString = portInputField.text;
        int port = portString.Length != 0 ? int.Parse(portString) : 7777;

        string logString = string.Format("Try connecting to {0}:{1}...", ipAddress, port);
        Debug.Log(logString);
        statusText.text = logString;
        var receiver = new Receiver();
        if (await receiver.ConnectAsync(new IPEndPoint(IPAddress.Parse(ipAddress), port)))
        {
            this.receiver = receiver;
            decoder = new Vp8Decoder();
            statusText.text = string.Format("Connected to {0}:{1}!", ipAddress, port);
        }
        else
        {
            UiVisibility = true;
            statusText.text = string.Format("Failed to connect to {0}:{1}.", ipAddress, port);
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

    // Parses a message that contains a KinectIntrinsics and uses the KinectIntrinsics to build a KinectScreen
    // that gets used for properly rendering the Kinect frame pixels in 3D.
    private static KinectScreen CreateKinectScreenFromIntrinsicsMessage(byte[] message)
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

        return new KinectScreen(colorIntrinsics, irIntrinsics);
    }
}
