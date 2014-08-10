// EightZipConfig.h

#ifndef EIGHTZIPCONFIG_H
#define EIGHTZIPCONFIG_H

#include <memory>

enum struct ConfigIndex
{
    LocationX = 0,
    LocationY,
    Width,
    Height,
    IsMaximized,
    Path,

    ConfigCount
};

class EightZipConfig
{
public:
    bool Get(ConfigIndex index, bool defaultValue) const;
    bool Get(ConfigIndex index, bool *pValue) const;
    int Get(ConfigIndex index, int defaultValue) const;
    bool Get(ConfigIndex index, int *pValue) const;
    wxString Get(ConfigIndex index, const wxString &defaultValue) const;
    bool Get(ConfigIndex index, wxString *pValue) const;

    void Set(ConfigIndex index, bool value);
    void Set(ConfigIndex index, int value);
    void Set(ConfigIndex index, wxString &value);

    void Save();

    ~EightZipConfig() { Save(); }

    static EightZipConfig &GetInstance();
    static void DestroyInstance();

private:
    class Cache
    {
    public:
        Cache() {}
        Cache(bool value) : m_integerValue(value) {}
        Cache(int value) : m_integerValue(value) {}
        Cache(const wxString &value) : m_stringValue(value) {}

        bool operator==(bool value) const
        {
            return m_booleanValue == value;
        }
        bool operator==(int value) const
        {
            return m_integerValue == value;
        }
        bool operator==(wxString const& value) const
        {
            return m_stringValue == value;
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
        Cache &operator=(wxString const& value)
        {
            Reset(value);
            return *this;
        }
        void Reset()
        {
            m_hasValue = false;
            m_booleanValue = false;
            m_integerValue = 0;
            m_stringValue.clear();
        }
        void Reset(bool value)
        {
            m_hasValue = true;
            m_booleanValue = value;
            m_integerValue = 0;
            m_stringValue = wxEmptyString;
        }
        void Reset(int value)
        {
            m_hasValue = true;
            m_booleanValue = false;
            m_integerValue = value;
            m_stringValue = wxEmptyString;
        }
        void Reset(wxString const& value)
        {
            m_hasValue = true;
            m_booleanValue = false;
            m_integerValue = 0;
            m_stringValue = value;
        }

        bool GetBoolean() const { return m_booleanValue; }
        int GetInt() const { return m_integerValue; }
        const wxString &GetString() const { return m_stringValue; }

    private:
        bool m_hasValue = false;

        bool m_booleanValue = false;
        int m_integerValue = 0;
        wxString m_stringValue;

    };

    enum struct ConfigType
    {
        Boolean,
        Integer,
        String
    };

    struct Definition
    {
        const wxString Name;
        const ConfigType Type;
        const ConfigIndex Index;
        const Cache DefaultValue;
    };

    static std::unique_ptr<EightZipConfig> m_upInstance;

    static const int CONFIG_COUNT = static_cast<int>(ConfigIndex::ConfigCount);
    
    static const Definition m_definitions[CONFIG_COUNT];
    Cache m_configs[CONFIG_COUNT];

    EightZipConfig();

};

#endif // EIGHTZIPCONFIG_H
