// ScopeGuard.h

#ifndef SCOPEGUARD_H
#define SCOPEGUARD_H

#include <functional>

#define SCOPEGUARD_LINENAME(name, line) name##line
#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPEGUARD_LINENAME(SCOPEEXIT, __LINE__) { callback }

class ScopeGuard
{
public:
    explicit ScopeGuard(std::function<void()> onExitScope)
        : m_onExitScope(onExitScope)
    {
    }

    ~ScopeGuard()
    {
        if (!m_isCancelled)
        {
            m_onExitScope();
        }
    }

    void Cancel()
    {
        m_isCancelled = true;
    }

private:
    std::function<void()> m_onExitScope;
    bool m_isCancelled = false;

    ScopeGuard(const ScopeGuard &) = delete;
    ScopeGuard& operator=(const ScopeGuard &) = delete;

};

#endif // SCOPEGUARD_H
