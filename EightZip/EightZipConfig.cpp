#include "stdwx.h"
#include "EightZipConfig.h"

std::unique_ptr<EightZipConfig> EightZipConfig::m_upInstance;

const EightZipConfig::Definition EightZipConfig::m_definitions[] = {
    { "Layout/LocationX", ConfigType::Integer, ConfigIndex::LocationX },
    { "Layout/LocationY", ConfigType::Integer, ConfigIndex::LocationY },
    { "Layout/Width", ConfigType::Integer, ConfigIndex::Width, 1000 },
    { "Layout/Height", ConfigType::Integer, ConfigIndex::Height, 560 },
    { "Layout/IsMaximized", ConfigType::Boolean, ConfigIndex::IsMaximized, true },
    { "Path", ConfigType::String, ConfigIndex::Path, wxFILE_SEP_PATH }
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
                config = strValue;
            }
            else
            {
                config = definition.DefaultValue;
            }
        }
    }
}

bool EightZipConfig::Get(ConfigIndex index, bool defaultValue) const
{
    bool result = false;
    if (!Get(index, &result))
    {
        return defaultValue;
    }
    return result;
}

bool EightZipConfig::Get(ConfigIndex index, bool *pValue) const
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

int EightZipConfig::Get(ConfigIndex index, int defaultValue) const
{
    int result = 0;
    if (!Get(index, &result))
    {
        return defaultValue;
    }
    return result;
}

bool EightZipConfig::Get(ConfigIndex index, int *pValue) const
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

wxString EightZipConfig::Get(ConfigIndex index, const wxString &defaultValue) const
{
    wxString result;
    if (!Get(index, &result))
    {
        return defaultValue;
    }
    return result;
}

bool EightZipConfig::Get(ConfigIndex index, wxString *pValue) const
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

void EightZipConfig::Set(ConfigIndex index, wxString &value)
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
            fileConfig.Write(definition.Name, config.GetString());
        }
    }
}

EightZipConfig &EightZipConfig::GetInstance()
{
    if (!m_upInstance)
    {
        m_upInstance.reset(new EightZipConfig);
    }
    return *m_upInstance;
}

void EightZipConfig::DestroyInstance()
{
    if (m_upInstance)
    {
        m_upInstance.reset();
    }
}
