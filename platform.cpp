#include "platform.h"
#include "raylib.h"

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
	InitWindow(windowWidth, windowHeight, title);
    
    image = GenImageColor(textureWidth, textureHeight, (Color) {199, 240, 216, 255});
    texture = LoadTextureFromImage(image);
    texture_rect = {0, 0, float(textureWidth), float(textureHeight)};
    screen_rect = {0, 0, float(windowWidth), float(windowHeight)};

    UnloadImage(image);

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(MAX_SAMPLES_PER_UPDATE);
    stream = LoadAudioStream(44100, 16, 1);
    SetAudioStreamCallback(stream, AudioInputCallback);
    AttachAudioStreamProcessor(stream, AudioProcessEffectLPF);

    PlayAudioStream(stream);
}

Platform::~Platform()
{
    UnloadAudioStream(stream);
    CloseAudioDevice();
	CloseWindow();
}

void Platform::SetFreq(float Value)
{
    Freq = Value;
}

void Platform::AudioInputCallback(void *buffer, unsigned int frames)
{
    float incr = Freq/44100.0f;
    short *d = (short *)buffer;

    for (unsigned int i = 0; i < frames; i++)
    {
        d[i] = (short)(MAX_SAMPLE_SIZE * (beepIdx < 0.25 ? 1 : -1) * 0.2);
        beepIdx += incr;
        if (beepIdx > 1.0f) beepIdx -= 1.0f;
    }
}

void Platform::AudioProcessEffectLPF(void *buffer, unsigned int frames)
{
    static float low[2] = { 0.0f, 0.0f };
    static const float cutoff = 70.0f / 44100.0f; // 70 Hz lowpass filter
    const float k = cutoff / (cutoff + 0.1591549431f); // RC filter formula

    // Converts the buffer data before using it
    float *bufferData = (float *)buffer;
    for (unsigned int i = 0; i < frames*2; i += 2)
    {
        const float l = bufferData[i];
        const float r = bufferData[i + 1];

        low[0] += k * (l - low[0]);
        low[1] += k * (r - low[1]);
        bufferData[i] = low[0];
        bufferData[i + 1] = low[1];
    }
}

void Platform::Update(void const* buffer)
{
    ClearBackground((Color) {199, 240, 216, 255});
    UpdateTexture(texture, buffer);
    DrawTexturePro(texture, texture_rect, screen_rect, (Vector2) {0, 0}, 0, (Color) {67, 82, 61, 255});
}
