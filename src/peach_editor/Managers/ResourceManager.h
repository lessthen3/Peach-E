/*******************************************************************
 *                     Peach Editor v0.0.7
 *             Created by Ranyodh Mandur - 🍑 2024
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#pragma once

#include <Utils/Logger.h>

#include <vector>
#include <string>

namespace PeachEditor{

    using namespace std;

    struct ResourceManager{

        [[nodiscard]] static bool
            WriteSPIRVToFile
            (
                const vector<uint32_t>& fp_SpirvBytecode,
                const string& fp_DesiredOutputDirectory,
                const string& fp_DesiredName,
                PeachCore::Logger* logger
            );

        [[nodiscard]] static bool
            LoadRawShaderSource
            (
                string* fp_SourceCode,
                const string& fp_ShaderSourcePath,
                PeachCore::Logger* logger
            );
    };
}