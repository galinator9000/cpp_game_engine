cbuffer ShadowMapSConstantBuffer : register(b3) {
	ShadowMap shadowMaps[MAX_SHADOW_CASTER_COUNT];
};

cbuffer GraphicsSettingsStruct : register(b13) {
	int PCFLevel;
};