#include "stdwx.h"
#include "EightZipConfig.h"

bool EightZipConfig::m_isDestroyed = false;
std::unique_ptr<EightZipConfig> EightZipConfig::m_upInstance;

const EightZipConfig::Definition EightZipConfig::m_definitions[] = {
    { wxT("Layout/EightZipLocationX"), ConfigType::Integer, ConfigIndex::EightZipLocationX },
    { wxT("Layout/EightZipLocationY"), ConfigType::Integer, ConfigIndex::EightZipLocationY },
    { wxT("Layout/EightZipWidth"), ConfigType::Integer, ConfigIndex::EightZipWidth, 1000 },
    { wxT("Layout/EightZipHeight"), ConfigType::Integer, ConfigIndex::EightZipHeight, 560 },
    { wxT("Layout/EightZipIsMaximized"), ConfigType::Boolean, ConfigIndex::EightZipIsMaximized, false },
    { wxT("Layout/ExtractLocationX"), ConfigType::Integer, ConfigIndex::ExtractLocationX },
    { wxT("Layout/ExtractLocationY"), ConfigType::Integer, ConfigIndex::ExtractLocationY },
    { wxT("Layout/ExtractWidth"), ConfigType::Integer, ConfigIndex::ExtractWidth, 500 },
    { wxT("Layout/ExtractHeight"), ConfigType::Integer, ConfigIndex::ExtractHeight, 240 },
    { wxT("Layout/ExtractIsMaximized"), ConfigType::Boolean, ConfigIndex::ExtractIsMaximized, false },
    { wxT("Path"), ConfigType::String, ConfigIndex::Path, wxFILE_SEP_PATH }
};

EightZipConfig::EightZipConfig()
{
    wxFileConfig fileConfig(EIGHTZIP_PROJECT_NAME);
    bool bValue;
    int nValue;
    wxString strValue;
    for (int i = 0; i != CONFIG_COUNT; ++i)
    {
        const auto &definition = m_definitions[i];
        auto &config = m_configs[i];
        switch (definition.Type)
        {
        case ConfigType::Boolean:
            if (fileConfig.Read(definition.Name, &bValue))
            {
                config = bValue;
            }
            else
            {
                config = definition.DefaultValue;
            }
            break;
        case ConfigType::Integer:
            if (fileConfig.Read(definition.Name, &nValue))
            {
                config = nValue;
            }
            else
            {
                config = definition.DefaultValue;
            }
            break;
        case ConfigType::String:
            if (fileConfig.Read(definition.Name, &strValue))
            {
                config = strValue.ToStdWstring();
            }
            else
            {
                config = definition.DefaultValue;
            }
        }
    }
}

bool EightZipConfig::GetBoolean(ConfigIndex index) const
{
    bool result = false;
    if (!GetBoolean(index, &result))
    {
        return m_definitions[static_cast<int>(index)].DefaultValue.GetBoolean();
    }
    return result;
}

bool EightZipConfig::GetBoolean(ConfigIndex index, bool *pValue) const
{
    assert(m_definitions[static_cast<int>(index)].Type == ConfigType::Boolean);
    const auto &config = m_configs[static_cast<int>(index)];
    if (config)
    {
        if (pValue)
        {
            *pValue = config.GetBoolean();
        }
        return true;
    }
    return false;
}

int EightZipConfig::GetInteger(ConfigIndex index) const
{
    int result = 0;
    if (!GetInteger(index, &result))
    {
        return m_definitions[static_cast<int>(index)].DefaultValue.GetInt();
    }
    return result;
}

bool EightZipConfig::GetInteger(ConfigIndex index, int *pValue) const
{
    assert(m_definitions[static_cast<int>(index)].Type == ConfigType::Integer);
    const auto &config = m_configs[static_cast<int>(index)];
    if (config)
    {
        if (pValue)
        {
            *pValue = config.GetInt();
        }
        return true;
    }
    return false;
}

TString EightZipConfig::GetString(ConfigIndex index) const
{
    TString result;
    if (!GetString(index, &result))
    {
        return m_definitions[static_cast<int>(index)].DefaultValue.GetString();
    }
    return result;
}

bool EightZipConfig::GetString(ConfigIndex index, TString *pValue) const
{
    assert(m_definitions[static_cast<int>(index)].Type == ConfigType::String);
    const auto &config = m_configs[static_cast<int>(index)];
    if (config)
    {
        if (pValue)
        {
            *pValue = config.GetString();
        }
        return true;
    }
    return false;
}

void EightZipConfig::Set(ConfigIndex index, bool value)
{
    assert(m_definitions[static_cast<int>(index)].Type == ConfigType::Boolean);
    m_configs[static_cast<int>(index)].Reset(value);
}

void EightZipConfig::Set(ConfigIndex index, int value)
{
    assert(m_definitions[static_cast<int>(index)].Type == ConfigType::Integer);
    m_configs[static_cast<int>(index)].Reset(value);
}

void EightZipConfig::Set(ConfigIndex index, const TString &value)
{
    assert(m_definitions[static_cast<int>(index)].Type == ConfigType::String);
    m_configs[static_cast<int>(index)].Reset(value);
}

void EightZipConfig::Save()
{
    wxFileConfig fileConfig(EIGHTZIP_PROJECT_NAME);
    for (int i = 0; i != CONFIG_COUNT; ++i)
    {
        const auto &definition = m_definitions[i];
        const auto &config = m_configs[i];
        switch (definition.Type)
        {
        case ConfigType::Boolean:
            fileConfig.Write(definition.Name, config.GetBoolean());
            break;
        case ConfigType::Integer:
            fileConfig.Write(definition.Name, config.GetInt());
            break;
        case ConfigType::String:
            fileConfig.Write(definition.Name, wxString(config.GetString()));
        }
    }
}

EightZipConfig &EightZipConfig::GetInstance()
{
    assert(!m_isDestroyed);
    if (!m_upInstance)
    {
        m_upInstance.reset(new EightZipConfig);
    }
    return *m_upInstance;
}

void EightZipConfig::DestroyInstance()
{
    m_isDestroyed = true;
    if (m_upInstance)
    {
        m_upInstance.reset();
    }
}
