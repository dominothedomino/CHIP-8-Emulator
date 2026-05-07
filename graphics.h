#include <SDL3/SDL.h>
#include <cmath>

class Platform{
    public:

    static void AudioCallBack(void* userdata, SDL_AudioStream* stream, int moreAmm, int totalAmm){
        int samples = moreAmm / sizeof(int16_t);
        int16_t* buffer = new int16_t[samples];
        static int sampleIndex = 0;

        for(int i = 0; i < samples; i++){
            float t = (float)sampleIndex / 44100.0f;
            buffer[i] = (int16_t)(sinf(2.0f * 3.1519f * 440.0f * t) * 3000);
            sampleIndex++;
            if(sampleIndex >= 44100) sampleIndex = 0;
        }

        SDL_PutAudioStreamData(stream, buffer, moreAmm);
        delete[] buffer;
    }

    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight){
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        window = SDL_CreateWindow(title, windowWidth, windowHeight, 0);
        if(!window){
            SDL_Log("Failed to create window: %s", SDL_GetError());
        }

        renderer = SDL_CreateRenderer(window, nullptr);
        if(!renderer){
            SDL_Log("Failed to create renderer: %s", SDL_GetError());
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
        if(!texture){
            SDL_Log("Failed to create texture: %s", SDL_GetError());
        }

        SDL_AudioSpec spec;
        spec.freq = 44100;
        spec.format = SDL_AUDIO_S16;
        spec.channels = 1;
        audiostream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, AudioCallBack, nullptr);
        if(!audiostream){
            SDL_Log("Failed to create audio stream: %s", SDL_GetError());
        }
    }

    ~Platform(){
        SDL_DestroyAudioStream(audiostream);
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Update(void const* buffer, int pitch){
        SDL_UpdateTexture(texture, nullptr, buffer, pitch);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    bool ProcessInput(uint8_t* keys){
        bool quit = false;
        SDL_Event event;
        while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                quit = true;
                break;

            case SDL_EVENT_KEY_DOWN:
                switch (event.key.scancode)
                {
                    case SDL_SCANCODE_ESCAPE: quit = true; break;
                    case SDL_SCANCODE_X: keys[0] = 1; break;
                    case SDL_SCANCODE_1: keys[1] = 1; break;
                    case SDL_SCANCODE_2: keys[2] = 1; break;
                    case SDL_SCANCODE_3: keys[3] = 1; break;
                    case SDL_SCANCODE_Q: keys[4] = 1; break;
                    case SDL_SCANCODE_W: keys[5] = 1; break;
                    case SDL_SCANCODE_E: keys[6] = 1; break;
                    case SDL_SCANCODE_A: keys[7] = 1; break;
                    case SDL_SCANCODE_S: keys[8] = 1; break;
                    case SDL_SCANCODE_D: keys[9] = 1; break;
                    case SDL_SCANCODE_Z: keys[0xA] = 1; break;
                    case SDL_SCANCODE_C: keys[0xB] = 1; break;
                    case SDL_SCANCODE_4: keys[0xC] = 1; break;
                    case SDL_SCANCODE_R: keys[0xD] = 1; break;
                    case SDL_SCANCODE_F: keys[0xE] = 1; break;
                    case SDL_SCANCODE_V: keys[0xF] = 1; break;
                }
                break;

            case SDL_EVENT_KEY_UP:
                switch (event.key.scancode)
                {
                    case SDL_SCANCODE_X: keys[0] = 0; break;
                    case SDL_SCANCODE_1: keys[1] = 0; break;
                    case SDL_SCANCODE_2: keys[2] = 0; break;
                    case SDL_SCANCODE_3: keys[3] = 0; break;
                    case SDL_SCANCODE_Q: keys[4] = 0; break;
                    case SDL_SCANCODE_W: keys[5] = 0; break;
                    case SDL_SCANCODE_E: keys[6] = 0; break;
                    case SDL_SCANCODE_A: keys[7] = 0; break;
                    case SDL_SCANCODE_S: keys[8] = 0; break;
                    case SDL_SCANCODE_D: keys[9] = 0; break;
                    case SDL_SCANCODE_Z: keys[0xA] = 0; break;
                    case SDL_SCANCODE_C: keys[0xB] = 0; break;
                    case SDL_SCANCODE_4: keys[0xC] = 0; break;
                    case SDL_SCANCODE_R: keys[0xD] = 0; break;
                    case SDL_SCANCODE_F: keys[0xE] = 0; break;
                    case SDL_SCANCODE_V: keys[0xF] = 0; break;
                }
                break;
        }
    }

    return quit;
    }

    void startSound(){
        if(!audioPlaying){
            SDL_ResumeAudioStreamDevice(audiostream);
            audioPlaying = true;
        }
    }

    void stopSound(){
        if(audioPlaying){
            SDL_PauseAudioStreamDevice(audiostream);
            audioPlaying = false;
        }
    }


    private:
    SDL_Window* window{};   
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
    SDL_AudioStream* audiostream{};
    bool audioPlaying = false;


};