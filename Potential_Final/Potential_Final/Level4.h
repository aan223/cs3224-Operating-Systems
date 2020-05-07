#pragma once
#include "Scene.h"
class Level4 : public Scene {
public:
	void Initialize() override;
	void Update(float deltaTime) override;
	void Render(ShaderProgram *program) override;
};
#pragma once
