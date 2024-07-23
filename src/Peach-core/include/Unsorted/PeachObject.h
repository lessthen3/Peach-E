#pragma once

#include <string>

using namespace std;

namespace PeachCore {

    //Base Node class
        class PeachObject 
        {

        public:
            virtual ~PeachObject();

            virtual void Update(float fp_TimeSinceLastFrame) = 0;
            virtual void ConstantUpdate(float fp_TimeSinceLastFrame) = 0;

            string m_Name;

        protected:
            PeachObject(const string& fp_Name) : m_Name(fp_Name) {}
    };

}