/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:         
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>
#include <string>

#include <iostream> // only used if logger is nullptr in asserts

#include <Utils/Logger.h>

namespace PeachTests
{
    using PeachCore::Logger;
    using LoggerFlags = PeachCore::Logger::Flags;
    namespace fs = std::filesystem;

    // --------------------------------------------
    // Assertion macro for logger tests
    // --------------------------------------------
    #define PEACH_LOGGER_TEST_ASSERT(logger, cond, msg)                                      \
        do {                                                                                 \
            if (!(cond)) {                                                                   \
                if (logger) {                                                                \
                    (logger)->Error(                                                        \
                        std::string("[ASSERT FAIL] ") + __FUNCTION__ + " : " + (msg),        \
                        "LoggerTests"                                                       \
                    );                                                                       \
                } else {                                                                     \
                    std::cerr << "[ASSERT FAIL] " << __FUNCTION__ << " : " << (msg)         \
                              << "  (line " << __LINE__ << ")\n";                            \
                }                                                                            \
                return false;                                                                \
            }                                                                                \
        } while (0)

    // --------------------------------------------
    // Helpers
    // --------------------------------------------

    inline fs::path MakeTestDir(const std::string& root, const std::string& name)
    {
        fs::path dir = fs::path(root) / name;
        fs::remove_all(dir);
        fs::create_directories(dir);
        return dir;
    }

    inline std::size_t CountLines(const fs::path& file)
    {
        std::ifstream in(file);
        if (!in.is_open()) return 0;
        std::size_t count = 0;
        std::string line;
        while (std::getline(in, line)) ++count;
        return count;
    }

    // If your RingBuffer API differs, tweak this.
    template<typename RingBuf>
    inline std::size_t GetSnapshotSize(const RingBuf& rb)
    {
        return rb.CurrentSize();
    }

    // -----------------------------------------------------------------
    // 1) Basic Create + ALL_LOGS + INFO write
    // -----------------------------------------------------------------
    inline bool Test_Logger_Create_AllLogs_BasicWrite(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "Create_AllLogs_BasicWrite");
        auto logs = dir / "logs";

        auto loggerOpt = Logger::Create("basic_logger", LoggerFlags::ALL_LOGS, logs.string());
        PEACH_LOGGER_TEST_ASSERT(testLogger, loggerOpt.has_value(), "Logger::Create should succeed");

        auto logger = std::move(*loggerOpt);
        logger.Info("hello world", "Test_Create_AllLogs_BasicWrite");
        logger.FlushAllLogs();

        auto base    = logs / "basic_logger";
        auto infoLog = base / "info.log";

        PEACH_LOGGER_TEST_ASSERT(testLogger, fs::exists(infoLog), "info.log should exist");

        std::ifstream in(infoLog);
        PEACH_LOGGER_TEST_ASSERT(testLogger, in.is_open(), "info.log should be readable");

        std::string line;
        std::getline(in, line);
        PEACH_LOGGER_TEST_ASSERT(
            testLogger,
            line.find("[info][Test_Create_AllLogs_BasicWrite]: hello world") != std::string::npos,
            "info.log content mismatch"
        );

        return true;
    }

    // -----------------------------------------------------------------
    // 2) LOG_TO_ONLY_SNAPSHOT_BUFFER – no files, snapshot has data
    // -----------------------------------------------------------------
    inline bool Test_Logger_SnapshotOnly_NoFiles(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "SnapshotOnly_NoFiles");
        auto logs = dir / "logs";

        uint32_t flags = LoggerFlags::ALL_LOGS | LoggerFlags::LOG_TO_ONLY_SNAPSHOT_BUFFER;

        auto loggerOpt = Logger::Create("mem_only_logger", flags, logs.string());
        PEACH_LOGGER_TEST_ASSERT(testLogger, loggerOpt.has_value(), "Logger::Create should succeed (snapshot only)");

        auto logger = std::move(*loggerOpt);
        logger.Info("in memory only", "SnapshotOnly");

        auto base = logs / "mem_only_logger";
        PEACH_LOGGER_TEST_ASSERT(testLogger, !fs::exists(base / "info.log"), "info.log should NOT exist in snapshot-only mode");

        const auto& snap = logger.GetSnapshotBuffer();
        auto snapSize    = GetSnapshotSize(snap);

        PEACH_LOGGER_TEST_ASSERT(testLogger, snapSize == 1, "Snapshot should have exactly 1 entry");

        return true;
    }

    // -----------------------------------------------------------------
    // 3) DONT_CREATE_DIRECTORY – fail if dir missing
    // -----------------------------------------------------------------
    inline bool Test_Logger_DontCreateDirectory_Fails(const std::string& root, Logger* testLogger)
    {
        auto dir = MakeTestDir(root, "DontCreateDirectory_Fails");
        auto nonExisting = dir / "does_not_exist_yet"; // do NOT create this

        uint32_t flags = LoggerFlags::ALL_LOGS | LoggerFlags::DONT_CREATE_DIRECTORY;

        auto loggerOpt = Logger::Create("no_dir_logger", flags, nonExisting.string());
        PEACH_LOGGER_TEST_ASSERT(
            testLogger,
            !loggerOpt.has_value(),
            "Logger::Create should fail when directory does not exist and DONT_CREATE_DIRECTORY is set"
        );

        return true;
    }

    // -----------------------------------------------------------------
    // 4) Active mask: only INFO + ERROR enabled
    // -----------------------------------------------------------------
    inline bool Test_Logger_ActiveMask_InfoAndErrorOnly(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "ActiveMask_InfoAndErrorOnly");
        auto logs = dir / "logs";

        uint32_t flags = LoggerFlags::INFO_LOG | LoggerFlags::ERROR_LOG;

        auto loggerOpt = Logger::Create("mask_logger", flags, logs.string());
        PEACH_LOGGER_TEST_ASSERT(testLogger, loggerOpt.has_value(), "Logger::Create should succeed");
        auto logger = std::move(*loggerOpt);

        logger.Trace("trace msg", "mask");
        logger.Debug("debug msg", "mask");
        logger.Info("info msg", "mask");
        logger.Warning("warn msg", "mask");
        logger.Error("error msg", "mask");
        logger.Fatal("fatal msg", "mask");
        logger.FlushAllLogs();

        auto base = logs / "mask_logger";

        PEACH_LOGGER_TEST_ASSERT(testLogger, !fs::exists(base / "trace.log"),   "trace.log should NOT exist");
        PEACH_LOGGER_TEST_ASSERT(testLogger, !fs::exists(base / "debug.log"),   "debug.log should NOT exist");
        PEACH_LOGGER_TEST_ASSERT(testLogger, !fs::exists(base / "warning.log"), "warning.log should NOT exist");
        PEACH_LOGGER_TEST_ASSERT(testLogger, !fs::exists(base / "fatal.log"),   "fatal.log should NOT exist");
        PEACH_LOGGER_TEST_ASSERT(testLogger,  fs::exists(base / "info.log"),    "info.log SHOULD exist");
        PEACH_LOGGER_TEST_ASSERT(testLogger,  fs::exists(base / "error.log"),   "error.log SHOULD exist");

        const auto& snap = logger.GetSnapshotBuffer();
        auto snapSize    = GetSnapshotSize(snap);
        PEACH_LOGGER_TEST_ASSERT(testLogger, snapSize == 2, "Snapshot should contain two entries (info + error)");

        return true;
    }

    // -----------------------------------------------------------------
    // 5) UpdateActiveMask – change from ALL_LOGS to ERROR only
    // -----------------------------------------------------------------
    inline bool Test_Logger_UpdateActiveMask_AllToErrorOnly(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "UpdateActiveMask_AllToErrorOnly");
        auto logs = dir / "logs";

        auto loggerOpt = Logger::Create("update_mask_logger", LoggerFlags::ALL_LOGS, logs.string());
        PEACH_LOGGER_TEST_ASSERT(testLogger, loggerOpt.has_value(), "Logger::Create should succeed");
        auto logger = std::move(*loggerOpt);

        logger.Info("before mask change", "update");
        logger.Error("before mask change", "update");
        logger.FlushAllLogs();

        auto base = logs / "update_mask_logger";
        PEACH_LOGGER_TEST_ASSERT(testLogger, fs::exists(base / "info.log"),  "info.log should exist pre-change");
        PEACH_LOGGER_TEST_ASSERT(testLogger, fs::exists(base / "error.log"), "error.log should exist pre-change");

        bool ok = logger.UpdateActiveMask(LoggerFlags::ERROR_LOG);
        PEACH_LOGGER_TEST_ASSERT(testLogger, ok, "UpdateActiveMask(ERROR_LOG) should succeed");

        logger.Info("after mask change", "update");
        logger.Error("after mask change", "update");
        logger.FlushAllLogs();

        std::size_t infoLines  = CountLines(base / "info.log");
        std::size_t errorLines = CountLines(base / "error.log");

        PEACH_LOGGER_TEST_ASSERT(testLogger, infoLines == 1, "info.log should still have only the first info entry");
        PEACH_LOGGER_TEST_ASSERT(testLogger, errorLines == 2, "error.log should have both pre- and post-change entries");

        return true;
    }

    // -----------------------------------------------------------------
    // 6) FLUSH_ERROR + FLUSH_FATAL – immediate flushing of those levels
    // -----------------------------------------------------------------
    inline bool Test_Logger_FlushFlags_ErrorAndFatal(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "FlushFlags_ErrorAndFatal");
        auto logs = dir / "logs";

        uint32_t flags = LoggerFlags::ALL_LOGS | LoggerFlags::FLUSH_ERROR | LoggerFlags::FLUSH_FATAL;

        auto loggerOpt = Logger::Create("flush_logger", flags, logs.string());
        PEACH_LOGGER_TEST_ASSERT(testLogger, loggerOpt.has_value(), "Logger::Create should succeed");
        auto logger = std::move(*loggerOpt);

        auto base      = logs / "flush_logger";
        auto errorPath = base / "error.log";
        auto fatalPath = base / "fatal.log";

        // ERROR
        logger.Error("flush error now", "flush_test");
        {
            std::ifstream in(errorPath);
            PEACH_LOGGER_TEST_ASSERT(testLogger, in.is_open(), "error.log should be readable immediately");
            std::string line;
            std::getline(in, line);
            PEACH_LOGGER_TEST_ASSERT(
                testLogger,
                line.find("flush error now") != std::string::npos,
                "error.log should contain ERROR message immediately"
            );
        }

        // FATAL
        logger.Fatal("flush fatal now", "flush_test");
        {
            std::ifstream in(fatalPath);
            PEACH_LOGGER_TEST_ASSERT(testLogger, in.is_open(), "fatal.log should be readable immediately");
            std::string line;
            std::getline(in, line);
            PEACH_LOGGER_TEST_ASSERT(
                testLogger,
                line.find("flush fatal now") != std::string::npos,
                "fatal.log should contain FATAL message immediately"
            );
        }

        return true;
    }

    // -----------------------------------------------------------------
    // 7) FLUSH_EVERY_N_LOGS – stress-ish
    // -----------------------------------------------------------------
    inline bool Test_Logger_FlushEveryNLogs_Stress(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "FlushEveryNLogs_Stress");
        auto logs = dir / "logs";

        auto loggerOpt = Logger::Create("stress_logger", LoggerFlags::ALL_LOGS, logs.string());
        PEACH_LOGGER_TEST_ASSERT(testLogger, loggerOpt.has_value(), "Logger::Create should succeed");
        auto logger = std::move(*loggerOpt);

        auto base    = logs / "stress_logger";
        auto infoLog = base / "info.log";

        const std::size_t totalLogs = Logger::FLUSH_EVERY_N_LOGS * 3 + 17;

        for (std::size_t i = 0; i < totalLogs; ++i)
        {
            logger.Info("stress_msg_" + std::to_string(i), "stress");
        }

        logger.FlushAllLogs();

        PEACH_LOGGER_TEST_ASSERT(testLogger, fs::exists(infoLog), "info.log should exist after stress logging");

        std::size_t lines = CountLines(infoLog);
        PEACH_LOGGER_TEST_ASSERT(
            testLogger,
            lines == totalLogs,
            "info.log should contain all stress messages"
        );

        return true;
    }

    // -----------------------------------------------------------------
    // 8) Thread ownership – wrong thread should be rejected
    // -----------------------------------------------------------------
    inline bool Test_Logger_ThreadOwnership_WrongThreadRejected(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "ThreadOwnership_WrongThreadRejected");
        auto logs = dir / "logs";

        auto loggerPtr = Logger::CreateShared("thread_logger", LoggerFlags::ALL_LOGS, logs.string());
        PEACH_LOGGER_TEST_ASSERT(testLogger, loggerPtr != nullptr, "Logger::CreateShared should succeed");

        loggerPtr->Info("from owner", "thread_test_owner");
        loggerPtr->FlushAllLogs();

        const auto& snapBefore = loggerPtr->GetSnapshotBuffer();
        auto sizeBefore        = GetSnapshotSize(snapBefore);

        std::thread t([loggerPtr]() {
            loggerPtr->Info("from other thread", "thread_test_other");
        });
        t.join();

        const auto& snapAfter = loggerPtr->GetSnapshotBuffer();
        auto sizeAfter        = GetSnapshotSize(snapAfter);

        PEACH_LOGGER_TEST_ASSERT(
            testLogger,
            sizeBefore == sizeAfter,
            "Snapshot size should NOT change when logging from non-owning thread"
        );

        return true;
    }

    // -----------------------------------------------------------------
    // 9) Snapshot capacity – ring buffer wrap
    // -----------------------------------------------------------------
    inline bool Test_Logger_SnapshotCapacity_RingBufferWraps(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "SnapshotCapacity_RingBufferWraps");
        auto logs = dir / "logs";

        auto loggerOpt = Logger::Create("snapshot_logger", LoggerFlags::ALL_LOGS, logs.string());
        PEACH_LOGGER_TEST_ASSERT(testLogger, loggerOpt.has_value(), "Logger::Create should succeed");
        auto logger = std::move(*loggerOpt);

        const int totalLogs = static_cast<int>(Logger::MAX_NUMBER_OF_LOGS) + 25;

        for (int i = 0; i < totalLogs; ++i)
        {
            logger.Info("msg_" + std::to_string(i), "snap_test");
        }

        const auto& snap = logger.GetSnapshotBuffer();
        auto size        = GetSnapshotSize(snap);

        PEACH_LOGGER_TEST_ASSERT(
            testLogger,
            size == Logger::MAX_NUMBER_OF_LOGS,
            "Snapshot buffer size should be capped at MAX_NUMBER_OF_LOGS"
        );

        return true;
    }

    // -----------------------------------------------------------------
    // 10) Truncation of oversized log file
    // -----------------------------------------------------------------
    inline bool Test_Logger_TruncateOversizedLogFile(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "TruncateOversizedLogFile");
        auto logs = dir / "logs";
        auto base = logs / "truncate_logger";

        fs::create_directories(base);
        fs::path infoPath = base / "info.log";

        {   // create oversized file
            std::ofstream out(infoPath, std::ios::out | std::ios::trunc);
            PEACH_LOGGER_TEST_ASSERT(testLogger, out.is_open(), "Pre-create oversized info.log");

            const std::size_t bigSize = Logger::MAX_LOG_FILE_SIZE_BYTES + 1024;
            out.seekp(bigSize - 1);
            out.write("", 1);
        }

        auto sizeBefore = fs::file_size(infoPath);
        PEACH_LOGGER_TEST_ASSERT(
            testLogger,
            sizeBefore > Logger::MAX_LOG_FILE_SIZE_BYTES,
            "info.log should initially be larger than MAX_LOG_FILE_SIZE_BYTES"
        );

        auto loggerOpt = Logger::Create("truncate_logger", LoggerFlags::ALL_LOGS, logs.string());
        PEACH_LOGGER_TEST_ASSERT(testLogger, loggerOpt.has_value(), "Logger::Create should succeed");
        auto logger = std::move(*loggerOpt);

        auto sizeAfter = fs::file_size(infoPath);
        PEACH_LOGGER_TEST_ASSERT(
            testLogger,
            sizeAfter <= Logger::MAX_LOG_FILE_SIZE_BYTES,
            "info.log should have been truncated on logger creation"
        );

        logger.Info("after truncate", "truncate_test");
        logger.FlushAllLogs();

        std::size_t lineCount = CountLines(infoPath);
        PEACH_LOGGER_TEST_ASSERT(
            testLogger,
            lineCount >= 1,
            "info.log should contain at least one line after logging"
        );

        return true;
    }

    // -----------------------------------------------------------------
    // 11) Destructor flush
    // -----------------------------------------------------------------
    inline bool Test_Logger_DestructorFlushes(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "DestructorFlushes");
        auto logs = dir / "logs";
        fs::path infoPath;

        {
            auto loggerOpt = Logger::Create("destructor_logger", LoggerFlags::ALL_LOGS, logs.string());
            PEACH_LOGGER_TEST_ASSERT(testLogger, loggerOpt.has_value(), "Logger::Create should succeed");

            auto logger = std::move(*loggerOpt);
            logger.Info("written before destruction", "dtor_test");

            auto base = logs / "destructor_logger";
            infoPath  = base / "info.log";
            // rely on ~Logger to flush
        }

        PEACH_LOGGER_TEST_ASSERT(testLogger, fs::exists(infoPath), "info.log should exist after logger destruction");

        std::ifstream in(infoPath);
        PEACH_LOGGER_TEST_ASSERT(testLogger, in.is_open(), "info.log should be readable after destruction");
        std::string line;
        std::getline(in, line);
        PEACH_LOGGER_TEST_ASSERT(
            testLogger,
            line.find("written before destruction") != std::string::npos,
            "info.log should contain message written before destruction"
        );

        return true;
    }

    // -----------------------------------------------------------------
    // 12) Snapshot-only + UpdateActiveMask – still no files
    // -----------------------------------------------------------------
    inline bool Test_Logger_SnapshotOnly_UpdateActiveMask_NoFiles(const std::string& root, Logger* testLogger)
    {
        auto dir  = MakeTestDir(root, "SnapshotOnly_UpdateActiveMask_NoFiles");
        auto logs = dir / "logs";

        uint32_t flags = LoggerFlags::ALL_LOGS | LoggerFlags::LOG_TO_ONLY_SNAPSHOT_BUFFER;

        auto loggerOpt = Logger::Create("snapshot_only_logger", flags, logs.string());
        PEACH_LOGGER_TEST_ASSERT(testLogger, loggerOpt.has_value(), "Logger::Create should succeed");
        auto logger = std::move(*loggerOpt);

        bool ok = logger.UpdateActiveMask(LoggerFlags::INFO_LOG | LoggerFlags::ERROR_LOG);
        PEACH_LOGGER_TEST_ASSERT(testLogger, ok, "UpdateActiveMask should succeed for snapshot-only logger");

        logger.Info("snapshot only info", "snap_only");
        logger.Error("snapshot only error", "snap_only");

        auto base = logs / "snapshot_only_logger";
        PEACH_LOGGER_TEST_ASSERT(testLogger, !fs::exists(base / "info.log"),  "info.log should NOT exist in snapshot-only");
        PEACH_LOGGER_TEST_ASSERT(testLogger, !fs::exists(base / "error.log"), "error.log should NOT exist in snapshot-only");

        const auto& snap = logger.GetSnapshotBuffer();
        auto size        = GetSnapshotSize(snap);
        PEACH_LOGGER_TEST_ASSERT(testLogger, size == 2, "Snapshot should have two entries");

        return true;
    }

    // -----------------------------------------------------------------
    // Master runner (like RunSerializerPODTests)
    // root = e.g. f_TestsRootDir + "/logging"
    // -----------------------------------------------------------------
    inline void RunLoggerTests(const std::string& root, Logger* logger)
    {
        using PeachCore::Colours;
        using PeachCore::Print;

        if (!logger)
        {
            PeachCore::PrintError("RunLoggerTests called with nullptr logger!", Colours::Magenta);
            return;
        }

        logger->Info("===== Running Logger tests =====", "LoggerTests");

        struct Case
        {
            const char* name;
            bool (*fn)(const std::string&, Logger*);
        };

        std::vector<Case> tests = {
            { "Create_AllLogs_BasicWrite",              &Test_Logger_Create_AllLogs_BasicWrite },
            { "SnapshotOnly_NoFiles",                   &Test_Logger_SnapshotOnly_NoFiles },
            { "DontCreateDirectory_Fails",              &Test_Logger_DontCreateDirectory_Fails },
            { "ActiveMask_InfoAndErrorOnly",            &Test_Logger_ActiveMask_InfoAndErrorOnly },
            { "UpdateActiveMask_AllToErrorOnly",        &Test_Logger_UpdateActiveMask_AllToErrorOnly },
            { "FlushFlags_ErrorAndFatal",               &Test_Logger_FlushFlags_ErrorAndFatal },
            { "FlushEveryNLogs_Stress",                 &Test_Logger_FlushEveryNLogs_Stress },
            { "ThreadOwnership_WrongThreadRejected",    &Test_Logger_ThreadOwnership_WrongThreadRejected },
            { "SnapshotCapacity_RingBufferWraps",       &Test_Logger_SnapshotCapacity_RingBufferWraps },
            { "TruncateOversizedLogFile",               &Test_Logger_TruncateOversizedLogFile },
            { "DestructorFlushes",                      &Test_Logger_DestructorFlushes },
            { "SnapshotOnly_UpdateActiveMask_NoFiles",  &Test_Logger_SnapshotOnly_UpdateActiveMask_NoFiles },
        };

        std::size_t passed = 0;
        std::size_t failed = 0;

        logger->Info("----- Starting Logger test suite -----", "LoggerTests");

        for (const auto& t : tests)
        {
            logger->Info(std::string("[RUN ] ") + t.name, "LoggerTests");
            bool ok = t.fn(root, logger);
            if (ok)
            {
                ++passed;
                logger->Info(std::string("[PASS] ") + t.name, "LoggerTests");
            }
            else
            {
                ++failed;
                logger->Error(std::string("[FAIL] ") + t.name, "LoggerTests");
            }
        }

        std::string summary = "[RESULT] Logger tests: " +
                              std::to_string(passed) + " passed, " +
                              std::to_string(failed) + " failed.";

        if (failed == 0)
            logger->Info(summary, "LoggerTests");
        else
            logger->Error(summary, "LoggerTests");

        logger->Info("===== Logger tests finished =====", "LoggerTests");
    }

} // namespace PeachTests
