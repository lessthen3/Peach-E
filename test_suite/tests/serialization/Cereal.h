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

#include <cassert>
#include <limits>
#include <string>
#include <vector>
#include <unordered_map>

#include <Utils/Serializer.h>
#include <Utils/Logger.h>

using namespace PeachCore;

namespace PeachTests {

    // ---------- Basic helpers ----------

    // Tiny helper to build a path (Windows-style for you)
    inline string JoinPath(const string& dir, const string& nameWithExt)
    {
        if (dir.empty()) return nameWithExt;
        if (dir.back() == '/' || dir.back() == '\\') return dir + nameWithExt;
        return dir + "/" + nameWithExt;
    }

    // ---------- Test PODs ----------

    struct SimplePOD
    {
        int          i;
        double       d;
        bool         b;
        string       s;

        PEACH_SERIALIZABLE(
            PEACH_FIELD(i),
            PEACH_FIELD(d),
            PEACH_FIELD(b),
            PEACH_FIELD(s)
        )
    };

    inline bool operator==(const SimplePOD& a, const SimplePOD& b)
    {
        return a.i == b.i &&
               a.d == b.d &&
               a.b == b.b &&
               a.s == b.s;
    }

    struct ContainerPOD
    {
        vector<int>                 ints;
        vector<double>              doubles;
        vector<string>              strings;
        vector<vector<int>>         nestedInts;
        unordered_map<string, int>  counts;

        PEACH_SERIALIZABLE(
            PEACH_FIELD(ints),
            PEACH_FIELD(doubles),
            PEACH_FIELD(strings),
            PEACH_FIELD(nestedInts),
            PEACH_FIELD(counts)
        )
    };

    inline bool operator==(const ContainerPOD& a, const ContainerPOD& b)
    {
        return a.ints       == b.ints
            && a.doubles    == b.doubles
            && a.strings    == b.strings
            && a.nestedInts == b.nestedInts
            && a.counts     == b.counts;
    }

    struct InnerPOD
    {
        int     id;
        float   value;
        string  name;

        PEACH_SERIALIZABLE(
            PEACH_FIELD(id),
            PEACH_FIELD(value),
            PEACH_FIELD(name)
        )
    };

    inline bool operator==(const InnerPOD& a, const InnerPOD& b)
    {
        return a.id == b.id &&
               a.value == b.value &&
               a.name == b.name;
    }

    struct NestedPOD
    {
        InnerPOD                          core;
        vector<InnerPOD>                  list;
        unordered_map<string, InnerPOD>   map;
        unordered_map<string, vector<int>> buckets;

        PEACH_SERIALIZABLE(
            PEACH_FIELD(core),
            PEACH_FIELD(list),
            PEACH_FIELD(map),
            PEACH_FIELD(buckets)
        )
    };

    inline bool operator==(const NestedPOD& a, const NestedPOD& b)
    {
        return a.core    == b.core
            && a.list    == b.list
            && a.map     == b.map
            && a.buckets == b.buckets;
    }

    // ---------- One derived POD to hit PEACH_SERIALIZABLE_DERIVED ----------

    struct BasePOD
    {
        int    baseHealth;
        string baseName;

        PEACH_SERIALIZABLE(
            PEACH_FIELD(baseHealth),
            PEACH_FIELD(baseName)
        )
    };

    struct DerivedPOD : BasePOD
    {
        float        speed;
        vector<int>  inventory;

        PEACH_SERIALIZABLE_DERIVED(BasePOD,
            PEACH_FIELD(speed),
            PEACH_FIELD(inventory)
        )
    };

    inline bool operator==(const DerivedPOD& a, const DerivedPOD& b)
    {
        const BasePOD& ba = a;
        const BasePOD& bb = b;

        return ba.baseHealth == bb.baseHealth
            && ba.baseName   == bb.baseName
            && a.speed       == b.speed
            && a.inventory   == b.inventory;
    }

    // ---------- Individual tests ----------

    bool 
        Test_SimplePOD_Roundtrip
        (
            const string& dir, 
            Logger* logger
        )
    {
        Serializer f_Serializer;

        SimplePOD original{};
        original.i = -123456;
        original.d = 3.141592653589793;
        original.b = true;
        original.s = "hello\n\t\"peach\"\\engine 🍑";

        const string fileName = "test_simple_pod";
        const string jsonFile = JoinPath(dir, fileName + ".json");

        bool ok = f_Serializer.ToJSON(original, fileName, dir, logger);
        assert(ok);

        SimplePOD restored{};
        ok = f_Serializer.FromJSON(restored, jsonFile, logger);
        assert(ok);

        assert(restored == original);
        return true;
    }

    bool Test_ContainerPOD_Roundtrip(const string& dir, Logger* logger)
    {
        Serializer s;

        ContainerPOD original{};

        original.ints = {
            0,
            1,
            -1,
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min()
        };

        original.doubles = {
            0.0,
            -0.0,
            1.0,
            -12345.6789,
            std::numeric_limits<double>::max() / 2.0
        };

        original.strings = {
            "",
            "simple",
            "with spaces",
            "weird\nnewline",
            "tab\tand\"quotes\"\\backslash"
        };

        original.nestedInts = {
            {},
            {1, 2, 3},
            {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()}
        };

        original.counts = {
            {"zero", 0},
            {"neg", -5},
            {"big", std::numeric_limits<int>::max()}
        };

        const string fileName = "test_container_pod";
        const string jsonFile = JoinPath(dir, fileName + ".json");

        bool ok = s.ToJSON(original, fileName, dir, logger);
        assert(ok);

        ContainerPOD restored{};
        ok = s.FromJSON(restored, jsonFile, logger);
        assert(ok);

        assert(restored == original);
        return true;
    }

    bool Test_NestedPOD_Roundtrip(const string& dir, Logger* logger)
    {
        Serializer s;

        NestedPOD original{};

        original.core = InnerPOD{
            42,
            0.5f,
            "core-peach"
        };

        original.list = {
            InnerPOD{1, 10.0f, "one"},
            InnerPOD{-5, -0.001f, "minus"},
            InnerPOD{999999, 1234.5678f, "big"}
        };

        original.map = {
            {"first",  InnerPOD{10, 1.0f, "first"}},
            {"second", InnerPOD{-10, -1.0f, "second"}}
        };

        original.buckets = {
            {"empty", {}},
            {"small", {1,2,3}},
            {"edge", {
                std::numeric_limits<int>::min(),
                0,
                std::numeric_limits<int>::max()
            }}
        };

        const string fileName = "test_nested_pod";
        const string jsonFile = JoinPath(dir, fileName + ".json");

        bool ok = s.ToJSON(original, fileName, dir, logger);
        assert(ok);

        NestedPOD restored{};
        ok = s.FromJSON(restored, jsonFile, logger);
        assert(ok);

        assert(restored == original);
        return true;
    }

    bool Test_DerivedPOD_Roundtrip(const string& dir, Logger* logger)
    {
        Serializer s;

        DerivedPOD original{};
        original.baseHealth = 1337;
        original.baseName   = "BasePeach";
        original.speed      = 9.81f;
        original.inventory  = {1, 2, 3, 5, 8, 13};

        const string fileName = "test_derived_pod";
        const string jsonFile = JoinPath(dir, fileName + ".json");

        bool ok = s.ToJSON(original, fileName, dir, logger);
        assert(ok);

        DerivedPOD restored{};
        ok = s.FromJSON(restored, jsonFile, logger);
        assert(ok);

        assert(restored == original);
        return true;
    }

    // ---------- Master entry point ----------

    inline void RunSerializerPODTests(const string& dir, Logger* logger)
    {
        assert(logger && "Logger must not be nullptr in these tests (Serializer expects it).");

        logger->Info("===== Running Serializer POD tests =====", "SerializerTests");

        Test_SimplePOD_Roundtrip(dir, logger);
        logger->Info("SimplePOD roundtrip OK", "SerializerTests");

        Test_ContainerPOD_Roundtrip(dir, logger);
        logger->Info("ContainerPOD roundtrip OK", "SerializerTests");

        Test_NestedPOD_Roundtrip(dir, logger);
        logger->Info("NestedPOD roundtrip OK", "SerializerTests");

        Test_DerivedPOD_Roundtrip(dir, logger);
        logger->Info("DerivedPOD roundtrip OK", "SerializerTests");

        logger->Info("===== ALL Serializer POD tests passed =====", "SerializerTests");
    }

} // namespace PeachTests
