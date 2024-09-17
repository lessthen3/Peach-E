#pragma once

#include "../../include/Peach-Core/Peach-Core.hpp"

using namespace std;

namespace PeachCore {

    class SerializationManager
    {
    public:
        static SerializationManager& Serializer() 
        {
            static SerializationManager serializer;
            return serializer;
        }
        ~SerializationManager();

    private:
        SerializationManager();
        SerializationManager(const SerializationManager&) = delete;
        SerializationManager& operator=(const SerializationManager&) = delete;


    public:

        PeachCamera2D* DeserializePeachCamera2DFromJSON(const string& fp_JSONData)
        {

        }

        Sprite2D* DeserializeSprite2DFromJSON(const string& fp_JSONData)
        {

        }

        TileSet* DeserializeTileSetFromJSON(const string& fp_JSONData)
        {

        }

        TileMap* DeserializeTileMapFromJSON(const string& fp_JSONData)
        {

        }

        CollisionShape2D* DeserializeCollisionShape2DFromJSON(const string& fp_JSONData)
        {

        }

        CollisionPolygon2D* DeserializeCollisionPolygon2DFromJSON(const string& fp_JSONData)
        {

        }

    };

}