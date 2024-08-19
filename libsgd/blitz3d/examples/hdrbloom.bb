Dialect "modern"

Include "start.bb"

; Visit https://openexr.com/en/latest/ for more exr info and samples.

CreateWindow GetDesktopWidth()/2,GetDesktopHeight()/2,"スノー Blocks",WINDOW_FLAGS_CENTERED

Local mat = LoadPrelitMaterial("sgd://misc/StillLife.exr")

Set2DFillMaterial mat

Local bloom = CreateBloomEffect()

While (PollEvents() And 1) <> 1
	
	If IsKeyHit(KEY_SPACE)
		SetRenderEffectEnabled bloom, Not IsRenderEffectEnabled(bloom)
	EndIf
	
	If IsKeyHit(KEY_UP) 
		If GetBloomEffectRadius(bloom) < 31
			SetBloomEffectRadius(bloom, GetBloomEffectRadius(bloom)+1)
		EndIf
	Else If IsKeyHit(KEY_DOWN)
		If GetBloomEffectRadius(bloom) > 2
			SetBloomEffectRadius(bloom, GetBloomEffectRadius(bloom)-1)
		EndIf
	EndIf
	
	Clear2D()
	
	Draw2DRect 0,0,GetWindowWidth(),GetWindowHeight()
	Draw2DText "Bloom enabled (space): " + IsRenderEffectEnabled(bloom),0,0
	Draw2DText "Bloom Radius (up/down): " + GetBloomEffectRadius(bloom),0,16
	
	RenderScene()
	
	Present()
Wend