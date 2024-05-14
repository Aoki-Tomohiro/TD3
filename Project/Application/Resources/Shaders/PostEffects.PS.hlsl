#include "PostEffects.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct LensDistortion
{
    int32_t isEnable;
    float tightness;
    float strength;
};

struct Vignette
{
    int32_t isEnable;
    float intensity;
};

struct GlitchNoise
{
    int32_t isEnable;
    float32_t blockSize;
    float32_t time;
    float32_t noiseSpeed;
    float32_t glitchIntensity;
};

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t4> gRenderedEffectsTexture : register(t1);
SamplerState gSampler : register(s0);

ConstantBuffer<LensDistortion> gLensDistortionParameter : register(b0);
ConstantBuffer<Vignette> gVignetteParameter : register(b1);
ConstantBuffer<GlitchNoise> gGlitchNoiseParameter : register(b2);

float32_t random(float32_t2 seeds)
{
    return frac(sin(dot(seeds, float2(12.9898, 78.233))) * 43758.5453);
}

float32_t blockNoise(float32_t2 seeds)
{
    return random(floor(seeds));
}

float32_t noiseRandom(float32_t2 seeds)
{
    return -1.0f + 2.0f * blockNoise(seeds);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    //通常テクスチャ
    float32_t4 originalColor = gTexture.Sample(gSampler, input.texcoord);
    //ポストエフェクトをかけたテクスチャ
    float32_t4 effectsColor = gRenderedEffectsTexture.Sample(gSampler, input.texcoord);
    //ポストエフェクトをかけたテクスチャが真っ黒の場合はoriginalColorを使う
    if (effectsColor.r == 0 && effectsColor.g == 0 && effectsColor.b == 0 && effectsColor.a == 0)
    {
        output.color = originalColor;
    }
    else
    {
        output.color = effectsColor;

    }
    
    //レンズディストーション
    if (gLensDistortionParameter.isEnable)
    {
        const float2 uvNormalized = input.texcoord * 2 - 1;
        const float distortionMagnitude = abs(uvNormalized.x * uvNormalized.y);
        const float smoothDistortionMagnitude = pow(distortionMagnitude, gLensDistortionParameter.tightness);
		//const float smoothDistortionMagnitude = 1 - sqrt(1 - pow(distortionMagnitude, gLensDistortionParameter.tightness));
		//const float smoothDistortionMagnitude = pow(sin(1.57079632679f * distortionMagnitude), gLensDistortionParameter.tightness);
        float2 uvDistorted = input.texcoord + uvNormalized * smoothDistortionMagnitude * gLensDistortionParameter.strength;
        if (uvDistorted[0] < 0 || uvDistorted[0] > 1 || uvDistorted[1] < 0 || uvDistorted[1] > 1)
        {
            originalColor = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
            effectsColor = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
        }
        else
        {
            originalColor = gTexture.Sample(gSampler, uvDistorted);
            effectsColor = gRenderedEffectsTexture.Sample(gSampler, uvDistorted);
            //ポストエフェクトをかけたテクスチャが真っ黒の場合はoriginalColorを使う
            if (effectsColor.r == 0 && effectsColor.g == 0 && effectsColor.b == 0 && effectsColor.a == 0)
            {
                output.color = originalColor;
            }
            else
            {
                output.color = effectsColor;

            }
        }
    }
    
    //ビネット
    if (gVignetteParameter.isEnable)
    {
        float2 uv = input.texcoord;
        uv = gVignetteParameter.intensity * uv - gVignetteParameter.intensity / 2;
        output.color *= 1.0 - dot(uv, uv);
    }
    
    if(gGlitchNoiseParameter.isEnable)
    {
        float32_t2 gv = input.texcoord;
        float32_t noise = blockNoise(input.texcoord.y * gGlitchNoiseParameter.blockSize);
        noise += random(input.texcoord.x) * 0.3f;
        float32_t2 randomValue = noiseRandom(float32_t2(input.texcoord.y, gGlitchNoiseParameter.time * gGlitchNoiseParameter.noiseSpeed));
        gv += randomValue * sin(sin(gGlitchNoiseParameter.glitchIntensity) * 0.5f) * sin(-sin(noise) * 0.2f) * frac(gGlitchNoiseParameter.time);
        output.color.r = gTexture.Sample(gSampler, gv + float32_t2(0.006f, 0.0f)).r;
        output.color.g = gTexture.Sample(gSampler, gv).g;
        output.color.b = gTexture.Sample(gSampler, gv - float32_t2(0.008f, 0.0f)).b;
        
        //static const float division = 768;
        //static const float blackinterval = 6;
        //static const float blackheight = 0.1f;
        //static const float noisewidth = 0.001;
        
        ////画面Y座標を分割
        //int divisionIndex = input.texcoord.y * division;
        
        ////一定間隔で横に区切ったブロックを作る
        //int noiseIndex = divisionIndex / blackinterval;
        
        ////ブロックごとに横にずらす座標を決める前処理
        ////時間による乱数のシード値（timeに互いに素っぽい数をかけたのを複数用意するといい感じになる）
        //float32_t2 timenoise = float32_t2(int(gGlitchNoiseParameter.time * 61.0f), int(gGlitchNoiseParameter.time * 83.0f));
        ////ときどき大きくずらす（時間のシード値が変更されるたびに5%の確率でノイズが10倍になる）
        //float noiserate = random(timenoise) < 0.05f ? 10.0f : 1.0f;
        
        ////横にずらす大きさを乱数で決める（0~1）（時間的にも位置的にもランダムになるように位置によるシード値と時間によるシード値を別次元で与える）
        //float xnoise = random(timenoise);
        ////ずれを2乗して0.5引く（2乗しないと乱れすぎる気がした）
        //xnoise = xnoise * xnoise - 0.5f;
        ////ずれにスケールをかける
        //xnoise = xnoise * noisewidth * noiserate;
        ////ラスタースキャンっぽいノイズ
        //xnoise = xnoise + (abs((int(gGlitchNoiseParameter.time * 2000) % int(division / blackinterval)) - noiseIndex) < 5 ? 0.005 : 0);
       
        //float32_t2 uv = input.texcoord + float32_t2(xnoise, 0.0f);
        
        ////ぼやけさせる
        //float32_t4 color1 = gTexture.Sample(gSampler, uv);
        //float32_t4 color2 = gTexture.Sample(gSampler, uv + float32_t2(0.005f, 0.0f));
        //float32_t4 color3 = gTexture.Sample(gSampler, uv + float32_t2(-0.005f, 0.0f));
        //float32_t4 color4 = gTexture.Sample(gSampler, uv + float32_t2(0.0f, 0.005f));
        //float32_t4 color5 = gTexture.Sample(gSampler, uv + float32_t2(0.0f, -0.005f));
        //output.color = (color1 * 4.0f + color2 + color3 + color4 + color5) / 8.0f;
        //output.color.rgb = divisionIndex % blackinterval < blackheight ? float32_t3(0.0f, 0.0f, 0.0f) : output.color.rgb;
    }
    
    return output;
}