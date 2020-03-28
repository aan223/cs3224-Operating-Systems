//name: Abdel Ngouloure
//Project4_Rise_of_The_AI
//The program creates a game with enemies with different properties

#include "Entity.h"

Entity::Entity() {
    position = glm::vec3(0);
    speed = 0;
    displacement = glm::vec3(0);
    
    modelMatrix = glm::mat4(1.0f);
    
    velocity = glm::vec3(0);
    acceleration = glm::vec3(0);
}

void Entity::Update(float deltaTime, Entity *player, Entity *platform, int platformCount, Entity *enemies, int enemyCount) {
    if (victory || failure) { //WHEN GAME ENDS
        return;
    }
    
    if(isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    if(entityType==ENEMY) { //call the AI function if its an enemy
        AI(player);
    }
    
    //if (entityType == PLAYER || entityType == ENEMY) {
    if (animIndices != NULL) {
        if (glm::length(displacement) == 0) {
            //idle = true;
            animTime += deltaTime;
        }
        /*
        else {
            idle = false;
        }
         */
        animTime += deltaTime;

        if (animTime >= 0.25f) {

            animTime = 0.0f;
            animIndex++;

            if (animIndex >= animFrames) {
                animIndex = 0;
            }
        }
        else {
            animIndex = 0;
        }
    }
    
    if(jump) {
        jump = false;
        
        velocity.y += jumpPower;
    }

    velocity.x = displacement.x * speed;
    velocity += acceleration * deltaTime;

    if(entityType == PLAYER) {
        CheckCollisionsX(enemies,enemyCount);
        CheckCollisionsY(enemies,enemyCount);
    }

    position.x += velocity.x * deltaTime;
    CheckCollisionsX(platform, platformCount);
    
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(platform, platformCount);
    //}
    //Renders everything
    modelMatrix = glm::mat4(1.0f);
    
    modelMatrix = glm::translate(modelMatrix, position);
    
}

void Entity::AIJumper() {
    if(collidedBottom) {
        jump = true;
    }
}

void Entity::AIWalker() {
    displacement = glm::vec3(-1, 0, 0);
    //position.x += speed;
}

void Entity::AIWaitAndGo(Entity *player) {
    switch(aiState){
        case IDLE:
            if(glm::distance(position, player->position) < 3.0f){
                aiState = WALKING;
            }
            break;
            
        case WALKING:
            if(player->position.x < position.x) {
                //displacement = glm::vec3(-1, 0, 0);
                position.x -= 0.02;
            }else {
                //displacement = glm::vec3(1, 0, 0);
                position.x += 0.02;
            }
            break;

    }
}

void Entity::AI(Entity *player) {
    switch(aiType) {
        case WALKER:
            AIWalker();
            break;
            
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
            
        case JUMPER:
            AIJumper();
            break;
    }
}

bool Entity::CheckCollision(Entity* other) {
    if(isActive == false || other->isActive == false) return false;
    
    float yDist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float xDist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);

    if (xDist < 0 && yDist < 0) {
        return true;
    }

    return false;
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        if (CheckCollision(object)) {
            if (velocity.x < 0) { //Checking left collision
                velocity.x = 0;
                collidedLeft = true;
                object->collidedRight = true;
                if (entityType == PLAYER && object->entityType == ENEMY) {
                    failure = true;
                    isActive = false;
                }
                //else if (entityType == PLAYER && object->entityType == ENEMY) {
                //    failure = true;
                //}
            }
            
            else if (velocity.x > 0) {//CHecking right collision
                velocity.x = 0;
                collidedRight = true;
                object->collidedLeft = true;
                if (entityType == PLAYER && object->entityType == ENEMY ){
                    failure = true;
                    isActive = false;
                }
                //else if (entityType == PLAYER && object->entityType == ENEMY) {
                //    failure = true;
                //}
                
            }
        }
        
    }
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount) {
    Entity* prev = NULL;
    float penetrationY = 0;
    for (int i = 0; i < objectCount; i++) { //plan to check is get previous block to see if they are the same or different
        Entity *object = &objects[i];
        if (CheckCollision(object)) { //Fixing penetration issues
            //float xDist = fabs(position.x - object->position.x);
            float yDist = fabs(position.y - object->position.y);
            
            float temp = fabs(yDist - (height / 2.0f) - (object->height / 2.0f));
            if (penetrationY < temp) {
                penetrationY = temp;
            }
            
            if (velocity.y < 0) {
                collidedBottom = true;
                object->collidedTop = true;
                if (entityType == PLAYER && object->entityType == ENEMY) {
                    if(position.y >= object->position.y) {
                        object->isActive = false;
                    }
                }
                /*
                if (entityType == PLAYER && object->entityType == ENEMY) {
                    victory = true;
                }
                 */
            }

            else if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                object->collidedBottom = true;
                if (entityType == PLAYER && object->entityType == ENEMY) {
                    if(position.y >= object->position.y) {
                        object->isActive = false;
                    }
                    else {
                        failure = true;
                        isActive = false;
                    }
                }
            }
        }
        
        prev = object;
    }
    
    /*
    if(failure && victory){
        Entity* surface = NULL;
        Entity* surface1 = NULL;
        
        for (int i = 0; i < objectCount; i++) {
            Entity* temp = &objects[i];
            if (temp->collidedBottom) {
                if(temp->entityType == PLATFORM){
                    surface = temp;
                }
                
                else if (temp->entityType == ENEMY) {
                    surface1 = temp;
                }
            }
        }
    }
     */
  
    if (collidedBottom) {
        position.y += penetrationY;
        velocity.y = 0;
    }
}

/*
void Entity::CheckCollisionsY(Entity *objects, int objectCount) {
    Entity* prev = NULL;
    //float penetrationY = 0;
    for (int i = 0; i < objectCount; i++) { //plan to check is get previous block to see if they are the same or different
        Entity *object = &objects[i];
        if (CheckCollision(object)) { //Fixing penetration issues
            //float xDist = fabs(position.x - object->position.x);
            float yDist = fabs(position.y - object->position.y);
            
            float penetrationY = fabs(yDist - (height / 2.0f) - (object->height / 2.0f));
            //if (penetrationY < temp) {
             //   penetrationY = temp;
            //}
            
            if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                object->collidedBottom = true;
                
                if (entityType == PLAYER && object->entityType == ENEMY) {
                    object->isActive = false;
                }
                
                if (entityType == PLAYER && object->entityType == ENEMY) {
                    victory = true;

                }
                 
            }
            
            else if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                if (entityType == PLAYER && object->entityType == ENEMY) {
                    failure = true;
                }
            }
        }
        
        prev = object;
    }
    
    
    if(failure && victory){
        Entity* surface = NULL;
        Entity* surface1 = NULL;
        
        for (int i = 0; i < objectCount; i++) {
            Entity* temp = &objects[i];
            if (temp->collidedBottom) {
                if(temp->entityType == PLATFORM){
                    surface = temp;
                }
                
                else if (temp->entityType == ENEMY) {
                    surface1 = temp;
                }
            }
        }
    }
     
  
    if (collidedBottom) {
        position.y += penetrationY;
        velocity.y = 0;
    }
   
}
*/

//ReDraws the Entities
void Entity::DrawSpriteFromTexture(ShaderProgram *program, GLuint textureID, int index) {

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

//Renders Entity
void Entity::Render(ShaderProgram* program) {
    if(isActive == false) return;
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
            DrawSpriteFromTexture(program, textureID, animIndices[animIndex]);
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
