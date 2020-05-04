#include "Entity.h"
Entity::Entity() { 
	position = glm::vec3(0);   
	movement = glm::vec3(0);
	velocity = glm::vec3(0);
	speed = 0;        
	modelMatrix = glm::mat4(1.0f); 
}

bool Entity::CheckCollision(Entity* other) {
	if (isActive == false || other->isActive == false) { return false; }
	float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
	float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

	if (xdist < 0 && ydist < 0) { 
		
		return true;
	}

	return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount) {
	for (int i = 0; i < objectCount; i++) { 
		Entity *object = &objects[i];     
		if (CheckCollision(object)) {


			airborne = false;

			float ydist = fabs(position.y - object->position.y);   
			float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));     
			if (velocity.y > 0) {
				position.y -= penetrationY;              
				velocity.y = 0; 
				collidedTop = true;
			}
			else if (velocity.y < 0) { 
				position.y += penetrationY;          
				velocity.y = 0; 
				collidedBottom = true;

				if (entityType == PLAYER && object->entityType ==ENEMY && !recover ) {
					object->isActive = false;
				}
			}
		}
	}
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount) { 
	for (int i = 0; i < objectCount; i++) { Entity *object = &objects[i];    
	if (CheckCollision(object)) {
		float xdist = fabs(position.x - object->position.x);     
		float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));        
		if (velocity.x > 0) {
			position.x -= penetrationX;       
			velocity.x = 0; 
			collidedRight = true;
			} 
		else if (velocity.x < 0) { 
			position.x += penetrationX;         
			velocity.x = 0; 
			collidedLeft = true;
			} 
		}
		if (collidedRight || collidedLeft) {//logic for enemy collisions with enemy and player or projectile
			if ((entityType == PLAYER && object->entityType == ENEMY) && !recover) {
				hp--;
				recover = true;
				if (hp <= 0) {
					fail = true;
					isActive = false;
				}
				else {
					position = glm::vec3(1.0f, -5.0f, 0);
				}

			}
			else if (entityType == ENEMY && object->entityType == PLAYER && !recover) {
				object->hp--;
				object->recover = true;
				if (hp <= 0) {
					fail = true;
					isActive = false;
				}
				else {
					object->position = glm::vec3(1.0f, -5.0f, 0);
				}

			}
			else if (entityType == PROJECTILE && object->entityType == ENEMY) {
				isActive = false;
				if (object->enemyType != BOSS || object->hp <= 0) {//because of how the algorithm works
					object->isActive = false;
				}
				else {
					object->hp--;
				}
			}
		}
	}
	
}

void Entity::CheckCollisionsY(Map *map) {    // Probes for tiles 
	glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);  
	glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z); 
	glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);   
	glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);   
	glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z); 
	glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);  
	float penetration_x = 0;  
	float penetration_y = 0;
	if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) { 
		position.y -= penetration_y;      
		velocity.y = 0; 
		collidedTop = true;
		colTop = true;
	}
	else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) { 
		position.y -= penetration_y; 
		velocity.y = 0;      
		collidedTop = true; 
		colTopLeft = true;
	}
	
	else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
		position.y -= penetration_y;     
		velocity.y = 0;     
		collidedTop = true;
		colTopRight = true;
	}

	if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {      
		position.y += penetration_y;
		velocity.y = 0;
		colBot = true;
		airborne = false;
	}

	else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {   

		colBotLeft = true;
	}
	else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {  

		colBotRight = true;
	}

	if ((colBotLeft || colBotRight) && (velocity.x == 0)) {
		position.y += penetration_y;
		velocity.y = 0;
		
		airborne = false;
	}
}

void Entity::CheckCollisionsX(Map *map) {    // Probes for tiles   
	glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
	glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);     
	float penetration_x = 0;
	float penetration_y = 0;
	if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
		position.x += penetration_x;      
		velocity.x = 0;    
		colLeft = true; 
	}     
	if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) { 
		position.x -= penetration_x;     
		velocity.x = 0;
		colRight = true; 
	}
	
	if ((colLeft || colRight) && entityType == ENEMY) {
		if (colLeft) {
			goLeft = false;
		}
		else {
			goLeft = true;
		}
	}
	
}


void Entity::Shoot(Entity* player) {
	isActive = true;
	position = player->position;
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.0f));
	if (player->goLeft) {
		movement.x = -1.0f;
	}
	else {
		movement.x = 1.0f;
	}
}


void Entity::AIWalker() {
	//look at points and see wether we should turn or not
	if (!colBotRight &&colBotLeft && !colBot) {
		goLeft = true;
	}
	if (colBotRight && !colBotLeft && !colBot) {
		goLeft = false;
	}

	if (goLeft) {
		movement.x = -1.0f;
		animIndices = animLeft;
	}
	else {
		movement.x = +1.0f;
		animIndices = animRight;

	}
};
void Entity::AIBoss(Entity* player) {
	if (!colBotRight &&colBotLeft && !colBot) {
		goLeft = true;
	}
	if (colBotRight && !colBotLeft && !colBot) {
		goLeft = false;
	}
	//like walker but if the character sees the player then the penguin will shoot the player
	float xdist = position.x - player->position.x;
	if (goLeft) {
		if (!shoot) {
			movement.x = -1.0f;
			animIndices = animLeft;
		}
		if (xdist < 0) {//PLayer on the Left
			shoot = true;
		}

	}
	else {
		if (!shoot) {
			movement.x = +1.0f;
			animIndices = animRight;
		}
		if (xdist > 0) {//PLayer on the right
			shoot = true;
		}
	}
}; //goes unused in this game; not enough time :(

void Entity::AIStalker(Entity* player) {
	if (!player->success && !player->fail) {
		float ydist = fabs(position.y - player->position.y) - ((height + player->height) / 2.0f);
		if (ydist <= 0) {
			chase = true;
		}
		else {
			chase = false;
		}
		if (chase) {
			if (position.x - player->position.x > 0) { //to the right
				movement.x = -1.0f;
				animIndices = animLeft;
			}
			else if (position.x - player->position.x < 0) {
				movement.x = 1.0f;
				animIndices = animRight;
			}
		}
	}
	else {
		movement.x = 0;
	}
}void Entity::AI(Entity* player) { 
	if (entityType == ENEMY) {
		//on this level there are only walkers
		switch (enemyType) {
			case STALKER:
				AIStalker(player);
				break;
			default:
				AIWalker();
				break;
		}

	}
}
void Entity::Update(float deltaTime, Entity *player, Entity* objects, int objectCount, Map* map) { 

	//updates entity
	collidedTop = false;
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;

	colTop = false;
	colTopLeft = false;
	colTopRight = false;
	colBot = false;
	colBotLeft = false;
	colBotRight = false;
	colLeft = false;
	colRight = false;

	if (entityType ==PLAYER &&hp <= 0) {
		player->fail = true;
	}

	if (success || fail || !isActive) { //if the player has completed game/ died or if object is not active
		//end of game if success or fail so we do not want to update positions and such
		return;
	}


	if (animIndices != NULL) {
		if (glm::length(movement) == 0 && entityType == PLAYER) {
			idle = true;
		}
		else {
			idle = false;
		}

		animTime += deltaTime;

		if (animTime >= animTimer) {

			animTime = 0.0f;
			animIndex++;

			if (animIndex >= animFrames) {
				animIndex = 0;
			}

		}
	}

	if (airborne && entityType ==PLAYER) {
		switch (goLeft) {

		case (false):
			//animIndices = animRightFly;
			break;

		case (true):
			//animIndices = animLeftFly;
			break;
		}
	}

	if (entityType == BACKGROUND) {//we only need this to happen to background to make this bigger
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(7.0f, -4.0f, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(13.0f, 9.0f, 0.0f));
		return;
	}

	else if (entityType == LIVES) {//we only need this to happen to background to make this bigger
		modelMatrix = glm::mat4(1.0f);
		velocity.x = movement.x * speed;
		position.x += velocity.x * deltaTime;
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.0f));
		return;
	}

	else if (entityType == PLAYER || entityType == ENEMY) {
		//movement logic
		

		velocity.x = movement.x * speed;
		velocity.y = movement.y * speed;
		position.y += velocity.y * deltaTime;
		//CheckCollisionsY(map);
		CheckCollisionsY(objects, objectCount);
		
		position.x += velocity.x * deltaTime;
		//CheckCollisionsX(map);
		CheckCollisionsX(objects, objectCount);

		if (entityType == ENEMY) {
			AI(player);
		}
		modelMatrix = glm::mat4(1.0f);

		modelMatrix = glm::translate(modelMatrix, position);

	}
	else if (entityType == PROJECTILE) {
		if (isActive) {
			velocity.x = movement.x*speed;

			position.x += velocity.x * deltaTime;
			CheckCollisionsX(objects, objectCount);

		}
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.5f, 0.0f));
	}
	else {
		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);
	}
	
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index) {

	float u = (float)(index % animCols) / (float)animCols;
	float v = (float)(index / animCols) / (float)animRows;
	
	float width = 1.0f / (float)animCols;
	float height = 1.0f / (float)animRows;
	
	float texCoords[] = { u, v + height, u + width, v + height, u + width, v, u, v + height, u + width, v, u, v };
	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

	
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) { 
	if (!isActive) { return; }
	program->SetModelMatrix(modelMatrix);    
	if (animIndices != NULL) {
		if (animIdle != NULL ) {//idle will be bigger
			if (idle && !goLeft) {
				DrawSpriteFromTextureAtlas(program, textureID, animIdle[3]);
				return;
			}
			else if (idle && goLeft ) {
				DrawSpriteFromTextureAtlas(program, textureID, animIdle[3]);
				return;
			}

			
		}
		DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
		return;
	}
	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };   
	float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };    
	glBindTexture(GL_TEXTURE_2D, textureID);  

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);  
	glEnableVertexAttribArray(program->positionAttribute);   

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);  
	glEnableVertexAttribArray(program->texCoordAttribute);      

	glDrawArrays(GL_TRIANGLES, 0, 6);     

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute); 
}

