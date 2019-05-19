using UnityEngine;

public static class PluginHelper
{
    public static void InitTextureGroup()
    {
        InvokeRenderEvent(0);
    }

    public static void UpdateTextureGroup()
    {
        InvokeRenderEvent(1);
    }

    private static void InvokeRenderEvent(int renderEvent)
    {
        GL.IssuePluginEvent(Plugin.get_render_event_function_pointer(), renderEvent);
    }
}
