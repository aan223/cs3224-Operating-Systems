#include "Scene.h"
class Level1 : public Scene { 
	public:
    int enemyCount;
    //bool complete =false;
    
	void Initialize() override;   
	void Update(float deltaTime) override;  
	void Render(ShaderProgram *program) override; 
};
