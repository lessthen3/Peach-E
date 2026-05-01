/*******************************************************************
 *                     Peach Editor v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#include "ProjectManager.h"

///STL
#include <filesystem>

///PeachCore
#include <utils/Serializer.h>

namespace PeachEditor
{

    bool
        ProjectManager::StartProjectManager()
    {

        return true;
    }

    bool
        ProjectManager::CloseProjectManager()
    {

        return true;
    }

    bool
        ProjectManager::CreateNewPeachProject
        (
            const string& fp_ProjectName,
            const string& fp_DesiredProjectPath,
            const bool fp_ShouldCreateDirectory,
            PeachProject& fp_PeachProject //IDK HOW to indicate side effects better
        )
    {
        //if (not fp_PeachProject)
        //{

        //    return false;
        //}
        //fp_PeachProject.
        const string f_FullProjectPath = fp_DesiredProjectPath + "/" + fp_ProjectName;

        //ensure there is no name clashes when a user tries to create a project w a name that is already being used by another project

        //if directory should not be created, check if the directory is valid/exists, if not inform user and stop project creation and prompt again
        if (not fp_ShouldCreateDirectory and not filesystem::exists(fp_DesiredProjectPath))
        {

            return false;
        }
        //create directories for project if directory creation UI item is checked and the user wants a new dir instead of using an existing dir
        else
        {
            error_code f_LastError;

            filesystem::create_directories(fp_DesiredProjectPath, f_LastError); //create hidden metadata folder for the project, like .vs or .godot


            filesystem::create_directories(f_FullProjectPath + "/.peach", f_LastError); //create hidden metadata folder for the project, like .vs or .godot
        }

        //write peach project settings -> JSON inside desired dir
        return PEACH_TO_JSON(fp_PeachProject, fp_ProjectName, fp_DesiredProjectPath, project_logger.get());
    }
}