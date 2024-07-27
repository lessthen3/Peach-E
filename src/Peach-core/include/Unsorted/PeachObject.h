#pragma once

#include <string>

using namespace std;

namespace PeachCore {

    //Base Node class
        class PeachObject 
        {
        public:
            virtual ~PeachObject();

            PeachObject() = default;

            //virtual void 
            //    Update(float fp_TimeSinceLastFrame) = 0; not sure if update functions are neccessary here because they can just inherit peachnode

            //virtual void 
            //    ConstantUpdate(float fp_TimeSinceLastFrame) = 0;

            string m_Name;

        protected:
            PeachObject(const string& fp_Name) : m_Name(fp_Name) {}
    };

}