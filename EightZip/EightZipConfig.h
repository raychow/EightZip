// EightZipConfig.h

#ifndef EIGHTZIPCONFIG_H
#define EIGHTZIPCONFIG_H

#include <memory>

#include "SevenZipCore/TString.h"

enum struct ConfigIndex
{
    Nothing = -1,

    EightZipLocationX = 0,
    EightZipLocationY,
    EightZipWidth,
    EightZipHeight,
    EightZipIsMaximized,
    ExtractWidth,
    ExtractHeight,
    ExtractIsMaximized,
    Path,

    ConfigCount
};

class EightZipConfig
{
public:
    bool GetBoolean(ConfigIndex index) const;
    bool GetBoolean(ConfigIndex index, bool *pValue) const;
    int GetInteger(ConfigIndex index) const;
    bool GetInteger(ConfigIndex index, int *pValue) const;
    TString GetString(ConfigIndex index) const;
    bool GetString(ConfigIndex index, TString *pValue) const;

    void Set(ConfigIndex index, bool value);
    void Set(ConfigIndex index, int value);
    void Set(ConfigIndex index, const TString &value);

    void Save();

    ~EightZipConfig() { Save(); }

    static EightZipConfig &GetInstance();
    // Must call before the wxApp destructor.
    static void DestroyInstance();

private:
    class Cache
    {
    public:
        Cache() {}
        Cache(bool value) : m_integerValue(value) {}
        Cache(int value) : m_integerValue(value) {}
        Cache(const TString &value) : m_tstrValue(value) {}

        bool operator==(bool value) const
        {
            return m_booleanValue == value;
        }
        bool operator==(int value) const
        {
            return m_integerValue == value;
        }
        bool operator==(const TString &value) const
        {
            return m_tstrValue == value;
        }
        operator bool() const { return m_hasValue; }
        Cache &operator=(bool value)
        {
            Reset(value);
            return *this;
        }
        Cache &operator=(int value)
        {
            Reset(value);
            return *this;
        }
        Cache &operator=(TString const& value)
        {
            Reset(value);
            return *this;
        }
        void Reset()
        {
            m_hasValue = false;
            m_booleanValue = false;
            m_integerValue = 0;
            m_tstrValue.clear();
        }
        void Reset(bool value)
        {
            m_hasValue = true;
            m_booleanValue = value;
            m_integerValue = 0;
            m_tstrValue = wxEmptyString;
        }
        void Reset(int value)
        {
            m_hasValue = true;
            m_booleanValue = false;
            m_integerValue = value;
            m_tstrValue = wxEmptyString;
        }
        void Reset(TString const& value)
        {
            m_hasValue = true;
            m_booleanValue = false;
            m_integerValue = 0;
            m_tstrValue = value;
        }

        bool GetBoolean() const { return m_booleanValue; }
        int GetInt() const { return m_integerValue; }
        const TString &GetString() const { return m_tstrValue; }

    private:
        bool m_hasValue = false;

        bool m_booleanValue = false;
        int m_integerValue = 0;
        TString m_tstrValue;

    };

    enum struct ConfigType
    {
        Boolean,
        Integer,
        String
    };

    struct Definition
    {
        const TString Name;
        const ConfigType Type;
        const ConfigIndex Index;
        const Cache DefaultValue;
    };

    static bool m_isDestroyed;
    static std::unique_ptr<EightZipConfig> m_upInstance;

    static const int CONFIG_COUNT = static_cast<int>(ConfigIndex::ConfigCount);
    
    static const Definition m_definitions[CONFIG_COUNT];
    Cache m_configs[CONFIG_COUNT];

    EightZipConfig();

};

#endif // EIGHTZIPCONFIG_H
