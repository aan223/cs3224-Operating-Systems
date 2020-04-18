//name: Abdel Ngouloure
//Project5_Platformer

#include "Scene.h"

class Home : public Scene {
public:
     void Initialize() override;
     void Update(float deltaTime) override;
     void Render(ShaderProgram *program) override;
};
