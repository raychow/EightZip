// Adapter.h

#ifndef SEVENZIPCORE_ADAPTER_H
#define SEVENZIPCORE_ADAPTER_H

#define BEGIN_DECLARE_ADAPTER_CLASS(target_name) \
    class target_name##Adapter \
    { \
    public: \
        target_name##Adapter(); \
        target_name##Adapter(std::shared_ptr<target_name> sp##target_name); \

#define END_DECLARE_ADAPTER_CLASS(target_name) \
    public: \
        template<typename T> \
        std::shared_ptr<T> QueryInterface(REFGUID iid); \
    protected: \
        std::shared_ptr<target_name> m_spTarget; \
    }; \
    template<typename T> \
    std::shared_ptr<T> target_name##Adapter::QueryInterface(REFGUID iid) \
    { \
        T *pResult = nullptr; \
        if (S_OK == m_spTarget->QueryInterface( \
            iid, reinterpret_cast<void **>(&pResult))) \
        { \
            return MakeComPtr(pResult, false); \
        } \
        return nullptr; \
    } \

#define DECLARE_ADAPTER_CLASS1(target_name, class_body1) \
    BEGIN_DECLARE_ADAPTER_CLASS(target_name) \
        class_body1 \
    END_DECLARE_ADAPTER_CLASS(target_name) \

#define DECLARE_ADAPTER_CLASS2(target_name, class_body1, class_body2) \
    BEGIN_DECLARE_ADAPTER_CLASS(target_name) \
        class_body1 \
        class_body2 \
    END_DECLARE_ADAPTER_CLASS(target_name) \

#define DECLARE_ADAPTER_CLASS3( \
    target_name, class_body1, class_body2, class_body3) \
    BEGIN_DECLARE_ADAPTER_CLASS(target_name) \
        class_body1 \
        class_body2 \
        class_body3 \
    END_DECLARE_ADAPTER_CLASS(target_name) \

#define DECLARE_ADAPTER_CLASS4( \
    target_name, class_body1, class_body2, class_body3, class_body4) \
    BEGIN_DECLARE_ADAPTER_CLASS(target_name) \
        class_body1 \
        class_body2 \
        class_body3 \
        class_body4 \
    END_DECLARE_ADAPTER_CLASS(target_name) \

#define DECLARE_ADAPTER_CLASS5( \
    target_name, \
    class_body1, \
    class_body2, \
    class_body3, \
    class_body4, \
    class_body5) \
    BEGIN_DECLARE_ADAPTER_CLASS(target_name) \
        class_body1 \
        class_body2 \
        class_body3 \
        class_body4 \
        class_body5 \
    END_DECLARE_ADAPTER_CLASS(target_name) \

#define IMPLEMENT_ADAPTER_CONSTRUCTOR(target_name) \
    target_name##Adapter::target_name##Adapter() : m_spTarget() {} \
    target_name##Adapter::target_name##Adapter( \
        std::shared_ptr<target_name> sp##target_name) \
    : m_spTarget(std::move(sp##target_name)) {} \
    
#define CHECK_OK(result, exception_class, message) \
    if (S_OK != (result)) \
    { \
        throw exception_class(message); \
    } \

#endif // SEVENZIPCORE_ADAPTER_H
