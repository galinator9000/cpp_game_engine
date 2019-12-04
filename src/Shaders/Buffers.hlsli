cbuffer ShadowBoxSConstantBuffer : register(b3) {
	ShadowBox shadowBoxes[MAX_SHADOWBOX_COUNT];
};

cbuffer GraphicsSettingsStruct : register(b13) {
	int PCFLevel;
};