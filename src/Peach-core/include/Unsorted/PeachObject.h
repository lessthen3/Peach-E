#pragma once

#include <string>

namespace PeachCore {

    //Base Node class
        class PeachObject {

        public:
            PeachObject(){

            }
            virtual ~PeachObject();

            virtual void Update(float dt) = 0;
            virtual void ConstantUpdate(float dt) = 0;
            virtual void Draw() = 0;

            std::string name;

        protected:
            PeachObject(const std::string& name) : name(name) {}
    };

}