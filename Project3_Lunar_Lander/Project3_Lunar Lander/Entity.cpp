//name: Abdel Ngouloure
//Project3_Lunar_Lander
//The program creates a Lundar Lander game simulation

#include "Entity.h"

Entity::Entity() {
    position = glm::vec3(0);
    speed = 0;
    displacement = glm::vec3(0);
    
    modelMatrix = glm::mat4(1.0f);
    
    velocity = glm::vec3(0);
    acceleration = glm::vec3(0);
}

void Entity::Update(float deltaTime, std::vector<Entity>surface, int surfaceCount) {
    if (victory || failure) { //WHEN GAME ENDS
        return;
    }

    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    if (entityType == PLAYER) {
        if (animIndices != NULL) {
            if (glm::length(displacement) == 0) {
                idle = true;
            }
            else {
                idle = false;
            }

            animTime += deltaTime;

            if (animTime >= 0.25f) {

                animTime = 0.0f;
                animIndex++;

                if (animIndex >= animFrames) {
                    animIndex = 0;
                }
            }
        }

        velocity.x = displacement.x * speed;
        velocity += acceleration * deltaTime;

        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        CheckCollisionsX(surface, surfaceCount);
        CheckCollisionsY(surface, surfaceCount);
    }
    //Renders everything
    modelMatrix = glm::mat4(1.0f);
    
    modelMatrix = glm::translate(modelMatrix, position);
    
}

bool Entity::CheckCollision(Entity* other) {
    float yDist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    float xDist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);

    if (xDist < 0 && yDist < 0) {
        return true;
    }

    return false;
}

void Entity::CheckCollisionsX(std::vector<Entity> objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        if (CheckCollision(object)) {
            if (velocity.x < 0) { //Checking left collision
                velocity.x = 0;
                collidedLeft = true;
                if (entityType == PLAYER && object->entityType == SURFACE) {
                    failure = true;
                }
                else if (entityType == PLAYER && object->entityType == VICTORY_BLOCK) {
                    failure = true;
                }
            }
            
            else if (velocity.x > 0) {//CHecking right collision
                velocity.x = 0;
                collidedRight = true;
                if (entityType == PLAYER && object->entityType == SURFACE ){
                    failure = true;
                }
                else if (entityType == PLAYER && object->entityType == VICTORY_BLOCK) {
                    failure = true;
                }
                
            }
        }
        
    }
}

void Entity::CheckCollisionsY(std::vector<Entity> objects, int objectCount) {
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
                object->collidedBottom = true;
                if (entityType == PLAYER && object->entityType == SURFACE) {
                    failure = true;
                }
                if (entityType == PLAYER && object->entityType == VICTORY_BLOCK) {
                    victory = true;

                }
            }
            
            else if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                if (entityType == PLAYER) {
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
                if(temp->entityType == SURFACE){
                    surface = temp;
                }
                
                else if (temp->entityType == VICTORY_BLOCK) {
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
