using UnityEngine;

public class TextureGroup
{
    public Texture YTexture { get; private set; }
    public Texture UTexture { get; private set; }
    public Texture VTexture { get; private set; }
    public Texture DepthTexture { get; private set; }

    public TextureGroup()
    {
        const int COLOR_WIDTH = 960;
        const int COLOR_HEIGHT = 540;
        const int DEPTH_WIDTH = 512;
        const int DEPTH_HEIGHT = 424;

        YTexture = Texture2D.CreateExternalTexture(COLOR_WIDTH,
                                                   COLOR_HEIGHT,
                                                   TextureFormat.R8,
                                                   false,
                                                   false,
                                                   Plugin.texture_group_get_y_texture_view());
        UTexture = Texture2D.CreateExternalTexture(COLOR_WIDTH / 2,
                                                   COLOR_HEIGHT / 2,
                                                   TextureFormat.R8,
                                                   false,
                                                   false,
                                                   Plugin.texture_group_get_u_texture_view());

        VTexture = Texture2D.CreateExternalTexture(COLOR_WIDTH / 2,
                                                   COLOR_HEIGHT / 2,
                                                   TextureFormat.R8,
                                                   false,
                                                   false,
                                                   Plugin.texture_group_get_v_texture_view());

        DepthTexture = Texture2D.CreateExternalTexture(DEPTH_WIDTH,
                                                       DEPTH_HEIGHT,
                                                       TextureFormat.R16,
                                                       false,
                                                       false,
                                                       Plugin.texture_group_get_depth_texture_view());
    }
}
