//name: Abdel Ngouloure
//Project5_Platformer

#include "Entity.h"
#include <SDL_mixer.h>

Mix_Chunk *bounce;

Entity::Entity() {
    position = glm::vec3(0);
    speed = 0;
    displacement = glm::vec3(0);
    
    modelMatrix = glm::mat4(1.0f);
    
    velocity = glm::vec3(0);
    acceleration = glm::vec3(0);
}

void Entity::Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Map *map) {
    //Start Audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    bounce = Mix_LoadWAV("bounce.wav");
    
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
/*
    if(entityType == PLAYER) {
        CheckCollisionsX(enemies,enemyCount);
        CheckCollisionsY(enemies,enemyCount);
    }
*/
    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);
    CheckCollisionsY(objects, objectCount); // Fix if needed

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);
    CheckCollisionsX(objects, objectCount); // Fix if needed
    
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
                        Mix_PlayChannel(-1, bounce, 0);
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
                        Mix_PlayChannel(-1, bounce, 0);
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
  
    if (collidedBottom) {
        position.y += penetrationY;
        velocity.y = 0;
    }
}

void Entity::CheckCollisionsY(Map *map) {
     // Probes for tiles
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
     }
     else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
         position.y -= penetration_y;
         velocity.y = 0;
         collidedTop = true;
     }
     else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
         position.y -= penetration_y;
         velocity.y = 0;
         collidedTop = true;
     }

     if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
         position.y += penetration_y;
         velocity.y = 0;
         collidedBottom = true;
     }
     else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
         position.y += penetration_y;
         velocity.y = 0;
         collidedBottom = true;
     }
     else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
         position.y += penetration_y;
         velocity.y = 0;
         collidedBottom = true;
     }
}

void Entity::CheckCollisionsX(Map *map) {
     // Probes for tiles
     glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
     glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);

     float penetration_x = 0;
     float penetration_y = 0;
    
     if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
         position.x += penetration_x;
         velocity.x = 0;
         collidedLeft = true;
     }

     if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
         position.x -= penetration_x;
         velocity.x = 0;
         collidedRight = true;
     }
}


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
