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

            string m_Name;

        protected:
            PeachObject(const string& fp_Name) : m_Name(fp_Name) {}
    };

}